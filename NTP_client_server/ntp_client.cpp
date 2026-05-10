#include <iostream>
#include <cstring>
#include <ctime>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

#define NTP_TIMESTAMP_DELTA 2208988800ull

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        cout << "Usage: ./ntp_client <ntp_server_url>\n";
        return 1;
    }

    const char* server = argv[1];

    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *host;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    host = gethostbyname(server);
    if(host == NULL)
    {
        cout << "Failed to resolve hostname\n";
        return 1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(123); // NTP port
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);

    // NTP request packet (48 bytes)
    unsigned char packet[48] = {0};

    /*
    LI = 0
    VN = 3
    Mode = 3 (client)
    */
    packet[0] = 0x1B;

    // Send packet
    sendto(sockfd, packet, sizeof(packet), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Receive response
    socklen_t addr_len = sizeof(server_addr);
    recvfrom(sockfd, packet, sizeof(packet), 0,
             (struct sockaddr*)&server_addr, &addr_len);

    // Extract transmit timestamp (bytes 40-43)
    unsigned long seconds;
    memcpy(&seconds, &packet[40], sizeof(seconds));
    seconds = ntohl(seconds);

    time_t txTm = seconds - NTP_TIMESTAMP_DELTA;

    // UTC time
    cout << "Time received from NTP server (UTC): "
         << ctime(&txTm);

    // IST time
    time_t ist = txTm + (5 * 3600 + 30 * 60);
    cout << "Time received from NTP server (IST): "
         << ctime(&ist);

    // Local machine time
    time_t local = time(NULL);
    cout << "Local machine time: "
         << ctime(&local);

    close(sockfd);

    return 0;
}