#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5001
#define VC_SIZE 2

typedef struct {
    int vc[VC_SIZE];
    char msg[100];
} packet;

int max(int a, int b) {
    return (a > b) ? a : b;
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len = sizeof(cliaddr);

    packet pkt;
    int server_vc[VC_SIZE] = {0, 0};

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    printf("UDP Echo Server with Vector Clock started...\n");

    for (int i = 0; i < 10; i++) {
        recvfrom(sockfd, &pkt, sizeof(pkt), 0,
                 (struct sockaddr *)&cliaddr, &len);

        // Update vector clock on receive
        server_vc[1]++;
        for (int j = 0; j < VC_SIZE; j++) {
            server_vc[j] = max(server_vc[j], pkt.vc[j]);
        }

        printf("Server received: %s\n", pkt.msg);
        printf("Server VC: [%d, %d]\n",
               server_vc[0], server_vc[1]);

        // Prepare echo
        strcpy(pkt.msg, "Echo from server");
        pkt.vc[0] = server_vc[0];
        pkt.vc[1] = server_vc[1];

        sendto(sockfd, &pkt, sizeof(pkt), 0,
               (struct sockaddr *)&cliaddr, len);
    }

    close(sockfd);
    return 0;
}
