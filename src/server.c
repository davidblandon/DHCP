#include "server.h"
#include "message.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 67
#define CLIENT_PORT 68

// Constructor
void init(Server* server, const char* ip, int lease_time_default, const char* dns, const char* gateway, const char* subnet_mask) {
    strncpy(server->ip, ip, sizeof(server->ip));
    server->lease_time_default = lease_time_default;
    strncpy(server->dns, dns, sizeof(server->dns));
    strncpy(server->gateway, gateway, sizeof(server->gateway));
    strncpy(server->subnet_mask, subnet_mask, sizeof(server->subnet_mask));
    server->leased_ip_count = 0;
    server->client_count = 0;
    server->ip_pool_count = 0;
    server->relay_agent_count = 0;
}

void* handle_discover(void* arg) {
    Server* server = (Server*)arg;
    char buffer[512];
    struct Message msg;

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Crear socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configurar dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Enlazar el socket con la dirección del servidor
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Recibir mensaje discover
    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
    deserialize_message(buffer, &msg);

    // Revisar si la MAC ya tiene un préstamo actual
    for (int i = 0; i < server->leased_ip_count; i++) {
        if (strcmp(server->leased_ips[i].mac, msg.client_mac) == 0) {
            printf("Error: MAC %s ya tiene un préstamo actual.\n", msg.client_mac);
            close(sockfd);
            return NULL;
        }
    }

    if (msg.message_type == DHCP_DISCOVER) {
        printf("Received DHCP Discover from %s\n", msg.client_mac);
        send_offer(server, &client_addr, sockfd);
    }

    close(sockfd);
    return NULL;
}

void listen_for_discover(Server* server) {
    // Implementación para escuchar mensajes discover
    while (1) {
        pthread_t thread;
        pthread_create(&thread, NULL, handle_discover, (void*)server);
        pthread_detach(thread);
    }
}

void send_offer(Server* server, struct sockaddr_in* client_addr, int sockfd) {
    // Implementación para enviar mensaje offer
    struct Message msg;
    char buffer[512];

    // Asignar una IP del pool utilizando FLSM
    char assigned_ip[16] = "";
    for (int i = 0; i < server->ip_pool_count; i++) {
        bool ip_in_use = false;
        for (int j = 0; j < server->leased_ip_count; j++) {
            if (strcmp(server->ip_pool[i], server->leased_ips[j].ip) == 0) {
                ip_in_use = true;
                break;
            }
        }
        if (!ip_in_use) {
            strncpy(assigned_ip, server->ip_pool[i], sizeof(assigned_ip));
            break;
        }
    }

    if (strlen(assigned_ip) == 0) {
        printf("No available IP addresses in the pool.\n");
        return;
    }

    msg.message_type = DHCP_OFFER;
    strncpy(msg.ip_address, assigned_ip, sizeof(msg.ip_address));
    strncpy(msg.dns, server->dns, sizeof(msg.dns));
    strncpy(msg.gateway, server->gateway, sizeof(msg.gateway));
    strncpy(msg.subnet_mask, server->subnet_mask, sizeof(msg.subnet_mask));
    msg.lease_time = server->lease_time_default;
    strncpy(msg.sender, "Server", sizeof(msg.sender));

    serialize_message(&msg, buffer);
    printf("Sending DHCP Offer: %s\n", buffer);

    // Enviar mensaje offer
    sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *)client_addr, sizeof(*client_addr));
}

void process_request(Server* server) {
    // Implementación para procesar request
    char buffer[512];
    struct Message msg;

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Crear socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configurar dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Enlazar el socket con la dirección del servidor
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Recibir mensaje request
    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
    deserialize_message(buffer, &msg);

    if (msg.message_type == DHCP_REQUEST) {
        printf("Received DHCP Request from %s\n", msg.client_mac);

        // Comprobar si la MAC está en el historial clients[]
        for (int i = 0; i < server->client_count; i++) {
            if (strcmp(server->clients[i].mac, msg.client_mac) == 0) {
                // La MAC está en el historial, llamar a process_renew_request
                process_renew_request(server, msg.client_mac);
                close(sockfd);
                return;
            }
        }

        // Si la MAC no está en el historial, proceder con el ACK
        send_ack(server, &client_addr, sockfd, msg.ip_address, msg.lease_time);
    }

    close(sockfd);
}

void send_ack(Server* server, struct sockaddr_in* client_addr, int sockfd, const char* ip_address, int lease_time) {
    // Implementación para enviar mensaje ack
    struct Message msg;
    char buffer[512];

    // Guardar la IP en leased_ips
    bool lease_exists = false;
    for (int i = 0; i < server->leased_ip_count; i++) {
        if (strcmp(server->leased_ips[i].ip, ip_address) == 0 && strcmp(server->leased_ips[i].mac, inet_ntoa(client_addr->sin_addr)) == 0) {
            lease_exists = true;
            break;
        }
    }
    if (!lease_exists && server->leased_ip_count < MAX_IPS) {
        strncpy(server->leased_ips[server->leased_ip_count].ip, ip_address, sizeof(server->leased_ips[server->leased_ip_count].ip));
        strncpy(server->leased_ips[server->leased_ip_count].mac, inet_ntoa(client_addr->sin_addr), sizeof(server->leased_ips[server->leased_ip_count].mac));
        server->leased_ips[server->leased_ip_count].lease_time = lease_time;
        server->leased_ip_count++;
    } else if (!lease_exists) {
        printf("No more IP addresses can be leased.\n");
    }

    // Guardar la IP en clients (historial) si no está ya
    bool client_exists = false;
    for (int i = 0; i < server->client_count; i++) {
        if (strcmp(server->clients[i].mac, inet_ntoa(client_addr->sin_addr)) == 0 && strcmp(server->clients[i].ip, ip_address) == 0) {
            client_exists = true;
            break;
        }
    }
    if (!client_exists && server->client_count < MAX_IPS) {
        strncpy(server->clients[server->client_count].ip, ip_address, sizeof(server->clients[server->client_count].ip));
        strncpy(server->clients[server->client_count].mac, inet_ntoa(client_addr->sin_addr), sizeof(server->clients[server->client_count].mac));
        server->clients[server->client_count].lease_time = lease_time;
        server->client_count++;
    } else if (!client_exists) {
        printf("No more clients can be stored.\n");
    }

    msg.message_type = DHCP_ACK;
    strncpy(msg.ip_address, ip_address, sizeof(msg.ip_address));
    strncpy(msg.dns, server->dns, sizeof(msg.dns));
    strncpy(msg.gateway, server->gateway, sizeof(msg.gateway));
    strncpy(msg.subnet_mask, server->subnet_mask, sizeof(msg.subnet_mask));
    msg.lease_time = lease_time;
    strncpy(msg.sender, "Server", sizeof(msg.sender));

    serialize_message(&msg, buffer);
    printf("Sending DHCP Ack: %s\n", buffer);

    // Enviar mensaje ack
    sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *)client_addr, sizeof(*client_addr));
}

void process_renew_request(Server* server, const char* client_mac) {
    // Implementación para procesar solicitud de renovación
    for (int i = 0; i < server->client_count; i++) {
        if (strcmp(server->clients[i].mac, client_mac) == 0) {
            // Encontrar la IP asignada a la MAC y renovar el lease
            struct sockaddr_in client_addr;
            socklen_t addr_len = sizeof(client_addr);

            // Configurar dirección del cliente
            memset(&client_addr, 0, sizeof(client_addr));
            client_addr.sin_family = AF_INET;
            client_addr.sin_addr.s_addr = inet_addr(server->clients[i].ip);
            client_addr.sin_port = htons(CLIENT_PORT);

            send_ack(server, &client_addr, -1, server->clients[i].ip, server->clients[i].lease_time);
            return;
        }
    }
    printf("No lease found for MAC: %s\n", client_mac);
}

void process_release(Server* server) {
    // Implementación para procesar release
    char buffer[512];
    struct Message msg;

    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Crear socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configurar dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Enlazar el socket con la dirección del servidor
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Recibir mensaje release
    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
    deserialize_message(buffer, &msg);

    if (msg.message_type == DHCP_RELEASE) {
        printf("Received DHCP Release from %s\n", msg.client_mac);
        reclaim_ip(server, msg.client_mac);
    }

    close(sockfd);
}

void reclaim_ip(Server* server, const char* client_mac) {
    // Implementación para recolectar IPs expiradas
    for (int i = 0; i < server->leased_ip_count; i++) {
        if (strcmp(server->leased_ips[i].mac, client_mac) == 0) {
            printf("Reclaiming IP: %s from MAC: %s\n", server->leased_ips[i].ip, client_mac);
            // Eliminar la IP de leased_ips
            for (int j = i; j < server->leased_ip_count - 1; j++) {
                server->leased_ips[j] = server->leased_ips[j + 1];
            }
            server->leased_ip_count--;
            return;
        }
    }
    printf("No lease found for MAC: %s\n", client_mac);
}

void manage_leases(Server* server) {
    // Implementación para monitorear leases
    while (1) {
        time_t current_time = time(NULL);
        for (int i = 0; i < server->leased_ip_count; i++) {
            if (difftime(current_time, server->leased_ips[i].lease_time) >= 0) {
                printf("Lease expired for IP: %s, MAC: %s\n", server->leased_ips[i].ip, server->leased_ips[i].mac);
                reclaim_ip(server, server->leased_ips[i].mac);
            }
        }
        sleep(1); // Esperar 1 segundo antes de la siguiente comprobación
    }
}

