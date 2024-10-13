#include "dhcp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 1067  // Asegúrate de que el cliente use el mismo puerto que el servidor
#define CLIENT_PORT 1068  // Usa un puerto por encima de 1024 para el cliente
#define BUFFER_SIZE 1024

int create_socket() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return -1;
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(CLIENT_PORT);

    if (bind(sockfd, (const struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("Error binding socket");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

int dhcp_discover() {
    int sockfd = create_socket();
    if (sockfd < 0) {
        return -1;
    }

    char dhcp_discover[BUFFER_SIZE];
    memset(dhcp_discover, 0, BUFFER_SIZE);
    // Crear mensaje DHCPDISCOVER
    dhcp_discover[0] = 0x01; // Message type: Boot Request (1)
    dhcp_discover[1] = 0x01; // Hardware type: Ethernet (1)
    dhcp_discover[2] = 0x06; // Hardware address length: 6
    dhcp_discover[3] = 0x00; // Hops: 0
    // Fill the rest of the DHCPDISCOVER message as needed

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_BROADCAST;
    server_addr.sin_port = htons(SERVER_PORT);

    int broadcastEnable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("Error setting socket options");
        close(sockfd);
        return -1;
    }

    printf("Sending DHCPDISCOVER...\n");
    if (sendto(sockfd, dhcp_discover, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error sending DHCPDISCOVER");
        close(sockfd);
        return -1;
    }
    printf("DHCPDISCOVER sent.\n");

    close(sockfd);
    return 0;
}

int dhcp_request() {
    int sockfd = create_socket();
    if (sockfd < 0) {
        return -1;
    }

    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    printf("Waiting for DHCPOFFER...\n");
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
    if (n < 0) {
        perror("Error receiving DHCPOFFER");
        close(sockfd);
        return -1;
    }
    printf("Received DHCPOFFER from server.\n");

    // Procesar mensaje DHCPOFFER y crear mensaje DHCPREQUEST
    char dhcp_request[BUFFER_SIZE];
    memset(dhcp_request, 0, BUFFER_SIZE);
    // Fill the DHCPREQUEST message as needed

    printf("Sending DHCPREQUEST...\n");
    if (sendto(sockfd, dhcp_request, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error sending DHCPREQUEST");
        close(sockfd);
        return -1;
    }
    printf("DHCPREQUEST sent.\n");

    printf("Waiting for DHCPACK...\n");
    n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
    if (n < 0) {
        perror("Error receiving DHCPACK");
        close(sockfd);
        return -1;
    }
    printf("Received DHCPACK from server.\n");

    close(sockfd);
    return 0;
}