#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int n;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connection failed");
        exit(1);
    }

    char message[] = "Hi from iitram ";
    write(sockfd, message, strlen(message));

    n = read(sockfd, buffer, BUFFER_SIZE);
    buffer[n] = '\0';
    printf("server msg : %s\n", buffer);


    printf("Client calls SHUT_WR for closing descriptor for riting only \n");
    shutdown(sockfd, SHUT_WR);

    n = read(sockfd, buffer, BUFFER_SIZE);
    printf("1st read after shutdown returned: %d\n", n);

    n = read(sockfd, buffer, BUFFER_SIZE);
    printf("2nd read after shutdown returned: %d (FIN received)\n", n);

    close(sockfd);
    return 0;
}
