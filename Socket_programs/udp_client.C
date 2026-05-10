#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5001
#define VC_SIZE 2   // [0] = client, [1] = server

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
    struct sockaddr_in servaddr;

    packet pkt;
    int client_vc[VC_SIZE] = {0, 0};
    int pid = getpid();

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    for (int i = 1; i <= 5; i++) {

        /* ---- SEND EVENT ---- */
        client_vc[0]++;   // increment own clock

        pkt.pid = pid;
        pkt.vc[0] = client_vc[0];
        pkt.vc[1] = client_vc[1];

        sprintf(pkt.msg, "Message %d from client %d", i, pid);

        sendto(sockfd, &pkt, sizeof(pkt), 0,
               (struct sockaddr *)&servaddr, sizeof(servaddr));

        printf("Client PID %d sent VC: [%d, %d]\n",
               pid, client_vc[0], client_vc[1]);

        /* ---- RECEIVE EVENT ---- */
        recvfrom(sockfd, &pkt, sizeof(pkt), 0, NULL, NULL);

        // Merge vector clocks
        for (int j = 0; j < VC_SIZE; j++) {
            client_vc[j] = max(client_vc[j], pkt.vc[j]);
        }

        client_vc[0]++;  // increment after receive

        printf("Client PID %d received echo\n", pid);
        printf("Updated Client VC: [%d, %d]\n\n",
               client_vc[0], client_vc[1]);

        sleep(5);
    }

    close(sockfd);
    return 0;
}
