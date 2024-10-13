#include "dhcp_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_PORT 1067  // Use a port above 1024 for testing
#define BUFFER_SIZE 1024

void *handle_client(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        printf("Waiting for DHCPDISCOVER...\n");
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Error receiving data");
            continue;
        }
        printf("Received DHCPDISCOVER from client.\n");

        // Process DHCPDISCOVER and send DHCPOFFER
        // For simplicity, we assume the message is DHCPDISCOVER and respond with DHCPOFFER
        char dhcp_offer[BUFFER_SIZE];
        memset(dhcp_offer, 0, BUFFER_SIZE);
        // Fill the DHCPOFFER message as needed

        printf("Sending DHCPOFFER to client...\n");
        if (sendto(sockfd, dhcp_offer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, addr_len) < 0) {
            perror("Error sending DHCPOFFER");
            continue;
        }
        printf("DHCPOFFER sent.\n");

        printf("Waiting for DHCPREQUEST...\n");
        n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Error receiving data");
            continue;
        }
        printf("Received DHCPREQUEST from client.\n");

        // Process DHCPREQUEST and send DHCPACK
        char dhcp_ack[BUFFER_SIZE];
        memset(dhcp_ack, 0, BUFFER_SIZE);
        // Fill the DHCPACK message as needed

        printf("Sending DHCPACK to client...\n");
        if (sendto(sockfd, dhcp_ack, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, addr_len) < 0) {
            perror("Error sending DHCPACK");
            continue;
        }
        printf("DHCPACK sent.\n");
    }

    close(sockfd);
    return NULL;
}

int start_dhcp_server() {
    int sockfd;
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(sockfd);
        return -1;
    }

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, handle_client, (void *)&sockfd);
    pthread_join(thread_id, NULL);

    close(sockfd);
    return 0;
}