#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int sockfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUFFER_SIZE];

    // Prevent zombie processes
    signal(SIGCHLD, SIG_IGN);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    listen(sockfd, 5);
    printf("server listening on port %d.\n", PORT);

    addr_len = sizeof(client_addr);

    while (1) {
        connfd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
        if (connfd < 0) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) {
            close(sockfd);  

            printf("Client connected.\n");

            read(connfd, buffer, BUFFER_SIZE);
            printf("Client says: %s\n", buffer);

            char reply[] = "Message received by server";
            write(connfd, reply, strlen(reply));

            printf("Server: calling shutdown(SHUT_WR)\n");

            shutdown(connfd, SHUT_WR);

            printf("Server: FIN sent. Sleeping 12 secs for observation.\n");
            sleep(12); 

            close(connfd);
            printf("Server: connection closed completely\n");

            exit(0);
        }

        close(connfd);
    }

    close(sockfd);
    return 0;
}

