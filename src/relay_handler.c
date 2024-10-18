#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "message.h"  // Incluye las definiciones de mensajes y funciones para serializarlos/deserializarlos

#define SERVER_PORT 2000  
#define CLIENT_PORT 1068  
#define RELAY_PORT 1067 // Puerto para el relay 

void forward_to_server(int sockfd, struct sockaddr_in *server_addr, char *buffer) {
    // Reenviar el mensaje DHCP Discover/Request al servidor
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("Error reenviando mensaje al servidor DHCP");
    } else {
        printf("Mensaje reenviado al servidor DHCP\n");
    }
}

void forward_to_client(int sockfd, struct sockaddr_in *client_addr, char *buffer) {
    // Reenviar el mensaje DHCP Offer/ACK al cliente
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)client_addr, sizeof(*client_addr)) == -1) {
        perror("Error reenviando mensaje al cliente");
    } else {
        printf("Mensaje reenviado al cliente DHCP\n");
    }
}

void relay_dhcp(int sockfd, struct sockaddr_in *server_addr) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[512];

    // Escuchar mensajes DHCP Discover o Request desde el cliente
    while (1) {
        int recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len == -1) {
            perror("Error recibiendo mensaje DHCP del cliente");
            continue;
        }

        printf("Mensaje recibido del cliente, reenviando al servidor DHCP...\n");
        forward_to_server(sockfd, server_addr, buffer);  // Reenviar al servidor

        // Recibir la respuesta del servidor DHCP (Offer o ACK)
        recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (recv_len == -1) {
            perror("Error recibiendo respuesta del servidor DHCP");
            continue;
        }

        printf("Respuesta recibida del servidor, reenviando al cliente...\n");
        forward_to_client(sockfd, &client_addr, buffer);  // Reenviar al cliente
    }
}

int main() {
    int sockfd;
    struct sockaddr_in relay_addr, server_addr;

    // Crear un socket UDP para el relay
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Error creando socket para DHCP Relay");
        exit(1);
    }

    // Configurar la dirección del relay
    memset(&relay_addr, 0, sizeof(relay_addr));
    relay_addr.sin_family = AF_INET;
    relay_addr.sin_port = htons(RELAY_PORT);
    relay_addr.sin_addr.s_addr = INADDR_ANY;

    // Enlazar el socket a la dirección del relay
    if (bind(sockfd, (struct sockaddr *)&relay_addr, sizeof(relay_addr)) == -1) {
        perror("Error enlazando el socket DHCP Relay");
        close(sockfd);
        exit(1);
    }

    // Configurar la dirección del servidor DHCP
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "192.168.1.10", &server_addr.sin_addr);  // Dirección IP del servidor DHCP

    // Iniciar el proceso de relay
    printf("Relay DHCP en funcionamiento...\n");
    relay_dhcp(sockfd, &server_addr);

    close(sockfd);
    return 0;
}
