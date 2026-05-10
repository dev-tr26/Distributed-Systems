#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERV_PORT 5000

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendbuf[512], recvbuf[512];
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    sleep(30);

    printf("Enter message: ");
    fgets(sendbuf, sizeof(sendbuf), stdin);

    write(sockfd, sendbuf, strlen(sendbuf));
    n = read(sockfd, recvbuf, sizeof(recvbuf));
    recvbuf[n] = '\0';

    printf("Echo from server: %s\n", recvbuf);

    close(sockfd);
    return 0;
}
