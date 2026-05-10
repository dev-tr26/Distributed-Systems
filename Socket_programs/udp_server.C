#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5001
#define BUFFER_SIZE 1024
#define VC_SIZE 2
#define TOTAL_MESSAGES 15   // 3 clients × 5 messages each

typedef struct {
    int pid;
    int vc[VC_SIZE];
    char msg[100];
} packet;

int max(int a, int b) {
    return (a > b) ? a : b;
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    packet pkt;

    int server_vc[VC_SIZE] = {0, 0};

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    printf("UDP Server started on port %d...\n", PORT);

    for (int i = 0; i < TOTAL_MESSAGES; i++) {

        len = sizeof(cliaddr);   // MUST reset inside loop

        recvfrom(sockfd, &pkt, sizeof(pkt), 0,
                 (struct sockaddr *)&cliaddr, &len);

        /* ---- Vector Clock Update (RECEIVE EVENT) ---- */
        for (int j = 0; j < VC_SIZE; j++) {
            server_vc[j] = max(server_vc[j], pkt.vc[j]);
        }
        server_vc[1]++;

        printf("\n[%d] Server received from client PID %d\n", i + 1, pkt.pid);
        printf("Message: %s\n", pkt.msg);
        printf("Updated Server VC: [%d, %d]\n",
               server_vc[0], server_vc[1]);

        /* ---- Prepare echo ---- */
        pkt.vc[0] = server_vc[0];
        pkt.vc[1] = server_vc[1];
        strcpy(pkt.msg, "Echo from server");

        sendto(sockfd, &pkt, sizeof(pkt), 0,
               (struct sockaddr *)&cliaddr, len);
    }

    printf("\nServer processed %d messages. Exiting...\n", TOTAL_MESSAGES);

    close(sockfd);
    return 0;
}
