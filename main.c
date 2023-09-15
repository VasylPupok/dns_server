
#include <arpa/inet.h>
#include <dns/server/server.h>
#include <dns/utils/hostname.h>
#include <stdio.h>
#include <unistd.h>

#define DNS_PORT 53
#define BUFFER_SIZE 512

int main(int argc, char** argv) {
    //dns_server_t* server = dns_server_new("../conf/dns.conf");
    dns_server_t* server = dns_server_new(argv[1]);



    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    size_t response_len;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server address configuration
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DNS_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("DNS server listening on port %d...\n", DNS_PORT);

    ssize_t recv_len;
    dns_message_t* msg;
    ssize_t sent_len;

    while (true) {
        // Receive DNS query from client
        memset(buffer, 0, sizeof(buffer));
        recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_addr_len);
        if (recv_len == -1) {
            perror("Receive error");
            continue;
        }

        printf("Received DNS query from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        if (recv_len != 0) {
            msg = dns_message_new(buffer);

            response_len = dns_server_resolve_as_bytearray(server, msg, response, BUFFER_SIZE);
            sent_len = sendto(sockfd, response, response_len, 0, (struct sockaddr*)&client_addr, client_addr_len);

            dns_message_delete(msg);
            msg = NULL;

            if (sent_len == -1) {
                perror("Send error");
            } else {
                printf("Sent DNS response to %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
        }
    }

    dns_server_delete(server);
    close(sockfd);

    return EXIT_SUCCESS;
}