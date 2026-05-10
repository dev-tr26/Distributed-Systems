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
    struct sockaddr_in servaddr;

    packet pkt;
    int client_vc[VC_SIZE] = {0, 0};

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    for (int i = 1; i <= 10; i++) {
        // Update VC on send
        client_vc[0]++;

        sprintf(pkt.msg, "Message %d from client", i);
        pkt.vc[0] = client_vc[0];
        pkt.vc[1] = client_vc[1];

        sendto(sockfd, &pkt, sizeof(pkt), 0,
               (struct sockaddr *)&servaddr, sizeof(servaddr));

        recvfrom(sockfd, &pkt, sizeof(pkt), 0, NULL, NULL);

        // Update VC on receive
        client_vc[0]++;
        for (int j = 0; j < VC_SIZE; j++) {
            client_vc[j] = max(client_vc[j], pkt.vc[j]);
        }

        printf("Client received: %s\n", pkt.msg);
        printf("Client VC: [%d, %d]\n\n",
               client_vc[0], client_vc[1]);

        sleep(1);
    }

    close(sockfd);
    return 0;
}
