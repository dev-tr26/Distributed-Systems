#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>


#define RETRY_USEC      100000   /* 100 ms */

/* ── node status ── */
typedef enum { CANDIDATE=0, LOST=1, WON=2 } Status;
static const char *SNAME[] = { "CANDIDATE", "LOST", "WON" };

static int g_sent = 0, g_recv = 0;

/* ════════════════ socket helpers ════════════════════════════════════════ */

static int server_socket(int id)
{
    int fd, opt = 1;
    struct sockaddr_in a;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); exit(1); }
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    memset(&a, 0, sizeof a);
    a.sin_family      = AF_INET;
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.sin_port        = htons(3000 + id);
    if (bind(fd, (struct sockaddr*)&a, sizeof a) < 0) { perror("bind"); exit(1); }
    if (listen(fd, 32) < 0) { perror("listen"); exit(1); }
    return fd;
}

static void msg_send(int from, int to, const char *msg)
{
    int fd, tries = 0;
    struct sockaddr_in a;
    memset(&a, 0, sizeof a);
    a.sin_family      = AF_INET;
    a.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.sin_port        = htons(3000 + to);
    for (;;) {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd < 0) { perror("socket"); exit(1); }
        if (connect(fd, (struct sockaddr*)&a, sizeof a) == 0) break;
        close(fd);
        if (++tries >= 100) {
            fprintf(stderr, "Node %d: cannot reach node %d after %d tries\n",
                    from, to, tries);
            exit(1);
        }
        usleep(100000); // retry ...
    }
    send(fd, msg, strlen(msg), 0);
    close(fd);
    printf("(%d) sending message to (%d): [%s]\n", from, to, msg);
    fflush(stdout);
    g_sent++;
}

/* Accept one connection and read its message into buf. */
static void msg_recv(int srv, char *buf, int sz)
{
    struct sockaddr_in ca; socklen_t cl = sizeof ca;
    int cfd = accept(srv, (struct sockaddr*)&ca, &cl);
    if (cfd < 0) { perror("accept"); exit(1); }
    memset(buf, 0, sz);
    int r = recv(cfd, buf, sz - 1, 0);
    close(cfd);
    if (r <= 0) buf[0] = '\0';
}

/* ════════════════ message helpers ══════════════════════════════════════ */

static void probe_build(char *b, const char *type, int cand, int ttl,
                        const char *dir)
{ snprintf(b, 256, "%s %d %d %s", type, cand, ttl, dir); }

/* returns 0=probe, 1=LEADER, -1=error */
static int probe_parse(const char *b, char *type, int *cand, int *ttl,
                       char *dir)
{
    if (strncmp(b, "LEADER", 6) == 0) return 1;
    return (sscanf(b, "%31s %d %d %15s", type, cand, ttl, dir) == 4) ? 0 : -1;
}

/* ════════════════ main ══════════════════════════════════════════════════ */

int main(int argc, char *argv[])
{
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <N> <my_id> <left_id> <right_id>\n", argv[0]);
        return 1;
    }
    int N        = atoi(argv[1]);
    int my_id    = atoi(argv[2]);
    int left_id  = atoi(argv[3]);
    int right_id = atoi(argv[4]);

    printf("=== Node %d starting  (N=%d, left=%d, right=%d) ===\n",
           my_id, N, left_id, right_id);
    fflush(stdout);

    Status status     = CANDIDATE;
    int    phase      = 0;
    int    echo_right = 0;
    int    echo_left  = 0;

    int srv = server_socket(my_id);
    usleep(800000);   /* wait for all nodes to open their server sockets */

    /* ── Phase 0 kick-off ── */
    {
        char m[256];
        probe_build(m, "SENDBOTH", my_id, 1 /*2^0*/, "R");
        msg_send(my_id, right_id, m);  echo_right = 1;
        probe_build(m, "SENDBOTH", my_id, 1, "L");
        msg_send(my_id, left_id,  m);  echo_left  = 1;
    }

    char buf[256], type[32], dir[16];
    int  cand, ttl;

    /* ════ main event loop ════ */
    for (;;) {

        msg_recv(srv, buf, sizeof buf);
        if (!buf[0]) continue;

        int r = probe_parse(buf, type, &cand, &ttl, dir);

        /* ── LEADER broadcast ── */
        if (r == 1) {
            int winner;
            sscanf(buf, "%*s %d", &winner);
            g_recv++;
            printf("(%d) received message from neighbour: [%s]; Status: %s\n",
                   my_id, buf, SNAME[status]);
            fflush(stdout);

            if (winner != my_id) {
                msg_send(my_id, right_id, buf);   /* relay around ring */
                status = LOST;
            } else {
                /* Our own LEADER msg has gone all the way around */
                printf("(%d) LEADER message returned to origin. "
                       "Election complete. Status: WON\n", my_id);
                fflush(stdout);
            }
            break;
        }
        if (r < 0) continue;

        g_recv++;
        printf("(%d) received message from neighbour: [%s]; Status: %s\n",
               my_id, buf, SNAME[status]);
        fflush(stdout);

        /* ── SENDECHO ── */
        if (strcmp(type, "SENDECHO") == 0) {

            if (cand == my_id && status == CANDIDATE) {
                /* one of our echoes returned */
                if      (strcmp(dir,"R") == 0) echo_right = 0;
                else if (strcmp(dir,"L") == 0) echo_left  = 0;

                if (!echo_right && !echo_left) {
                    /* both sides echoed → survived; advance phase */
                    phase++;
                    int new_ttl = 1;
                    for (int i = 0; i < phase; i++) new_ttl *= 2;

                    if (new_ttl >= N) {
                        /* probe spans full ring → last survivor = LEADER */
                        status = WON;
                        printf("(%d) Phase %d: 2^phase=%d >= N=%d "
                               "→ I am LEADER! Status: WON\n",
                               my_id, phase, new_ttl, N);
                        fflush(stdout);
                        char lm[256];
                        snprintf(lm, sizeof lm, "LEADER %d", my_id);
                        msg_send(my_id, right_id, lm);
                        /* keep server open; wait for LEADER to return */
                        continue;
                    }

                    /* start new phase */
                    char m[256];
                    probe_build(m, "SENDBOTH", my_id, new_ttl, "R");
                    msg_send(my_id, right_id, m);  echo_right = 1;
                    probe_build(m, "SENDBOTH", my_id, new_ttl, "L");
                    msg_send(my_id, left_id,  m);  echo_left  = 1;
                }
            } else {
                /* relay echo toward origin (same dir label, same travel dir) */
                char fwd[256];
                probe_build(fwd, "SENDECHO", cand, 0, dir);
                if (strcmp(dir,"R") == 0) msg_send(my_id, right_id, fwd);
                else                      msg_send(my_id, left_id,  fwd);
            }
            continue;
        }

        /* ── SENDBOTH or SENDPASS ── */
        if (strcmp(type,"SENDBOTH")==0 || strcmp(type,"SENDPASS")==0) {

            if (cand == my_id) {
                /* Own probe circled the ring (edge case) */
                status = WON;
                printf("(%d) Own probe returned - last candidate. "
                       "Status: WON\n", my_id);
                fflush(stdout);
                char lm[256];
                snprintf(lm, sizeof lm, "LEADER %d", my_id);
                msg_send(my_id, right_id, lm);
                continue;   /* wait for LEADER to come back */
            }

            if (cand < my_id) {
                printf("(%d) Discarding probe for smaller cand=%d\n",
                       my_id, cand);
                fflush(stdout);
                continue;
            }

            /* cand > my_id */
            if (status == CANDIDATE) {
                status = LOST;
                printf("(%d) Becoming LOST (larger id %d seen)\n",
                       my_id, cand);
                fflush(stdout);
            }

            if (ttl > 1) {
                /* forward with ttl-1 */
                char fwd[256];
                probe_build(fwd, "SENDPASS", cand, ttl - 1, dir);
                if (strcmp(dir,"R") == 0) msg_send(my_id, right_id, fwd);
                else                      msg_send(my_id, left_id,  fwd);
            } else {
                /* ttl == 1: turn around → SENDECHO
                 * Travel direction reverses; dir label stays same.
                 * probe dir=R (came from left) → echo goes left
                 * probe dir=L (came from right)→ echo goes right        */
                char echo[256];
                probe_build(echo, "SENDECHO", cand, 0, dir);
                if (strcmp(dir,"R") == 0) msg_send(my_id, left_id,  echo);
                else                      msg_send(my_id, right_id, echo);
            }
        }

    }

    printf("(%d) DONE. Status=%-10s  Sent=%d  Received=%d\n",
           my_id, SNAME[status], g_sent, g_recv);
    fflush(stdout);
    close(srv);
    return 0;
}