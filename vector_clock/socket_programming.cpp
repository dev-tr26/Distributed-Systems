#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main() {
    int sockfd, fd;

    // Create a TCP socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    cout << "socket fd: " << sockfd << endl;

    // Open a file named "data"
    fd = open("data", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    cout << "file fd: " << fd << endl;

    // Keep process alive for inspection
    for (;;);

    // Cleanup (unreachable due to infinite loop)
    close(sockfd);
    close(fd);

    return 0;
}