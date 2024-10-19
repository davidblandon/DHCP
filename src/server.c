#include "server.h"
#include "message.h"
#include <stdlib.h>
#include "utils.c"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 2000
#define CLIENT_PORT 1068

// Estructura para pasar el servidor y el socket a los hilos
struct ServerThreadArgs {
    Server* server;
    int sockfd;
    struct sockaddr_in client_addr;  // Agregar para almacenar la dirección del cliente
    struct Message msg;
};

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
    struct ServerThreadArgs* args = (struct ServerThreadArgs*)arg;
    Server* server = args->server;
    struct sockaddr_in client_addr = args->client_addr;  // Usar la dirección del cliente

    // Crear un nuevo socket temporal para enviar la respuesta al cliente
    int temp_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (temp_sockfd == -1) {
        perror("No se pudo crear el socket temporal");
        free(args);
        return NULL;
    }

    // Enviar la respuesta al cliente
    send_offer(server, &client_addr, temp_sockfd);

    close(temp_sockfd);
    free(args);
    return NULL;
}



void* handle_request(void* arg) {
    printf("initializing handle_request\n");
    struct ServerThreadArgs* args = (struct ServerThreadArgs*)arg;
    Server* server = args->server;
    struct sockaddr_in client_addr = args->client_addr;  // Usar la dirección del cliente
    struct Message msg = args->msg;  // Obtener el mensaje DHCP_REQUEST

    // Comprobar si es un mensaje DHCP_REQUEST
    printf("Procesando DHCP Request del cliente %s\n", inet_ntoa(client_addr.sin_addr));

    // Verificar que la IP en el mensaje de solicitud sea válida
    char* requested_ip = msg.ip_address;
    bool ip_assigned = false;

    // Comprobar si la IP solicitada está asignada al cliente en el historial
    for (int i = 0; i < server->client_count; i++) {
                if (strcmp(server->clients[i].mac, msg.client_mac) == 0) {
                    // La MAC está en el historial, llamar a process_renew_request
                    printf("Proceso de renew request");
                    process_renew_request(server, msg.client_mac);
                    return NULL;
                }
            }

    printf("Confirmando IP asignada %s para el cliente %s\n", requested_ip, msg.client_mac);

    int temp_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (temp_sockfd == -1) {
        perror("No se pudo crear el socket temporal");
        free(args);
        return NULL;
    }   

    
    send_ack(server, &client_addr, temp_sockfd, requested_ip, server->lease_time_default);
    close(temp_sockfd);
    free(args);
    return NULL;
}




void listen_for_discover(Server* server, int sockfd) {
    struct ServerThreadArgs args;
    args.server = server;
    args.sockfd = sockfd;

    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[512];
    struct Message msg;

    while (1) {
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
        deserialize_message(buffer, &msg);



        print_message_received(&msg);
        if (msg.message_type == DHCP_DISCOVER || msg.message_type == DHCP_REQUEST) {
            pthread_t thread;
            struct ServerThreadArgs* thread_args = malloc(sizeof(struct ServerThreadArgs));
            thread_args->server = server;
            thread_args->client_addr = client_addr;
            thread_args->msg = msg; 

            
            if (msg.message_type == DHCP_DISCOVER) {

                pthread_create(&thread, NULL, handle_discover, (void*)thread_args);
            } else if (msg.message_type == DHCP_REQUEST) {

                pthread_create(&thread, NULL, handle_request, (void*)thread_args);
            }

            pthread_detach(thread);
        }
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
    print_message_sent(&msg);

    // Enviar mensaje offer
    sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *)client_addr, sizeof(*client_addr));
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
    print_message_sent(&msg);

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

            send_ack(server, &client_addr, -1, server->clients[i].ip, server->lease_time_default);
            return;
        }
    }
    printf("No lease found for MAC: %s\n", client_mac);
}

void process_release(Server* server, int sockfd) {
    // Implementación para procesar release
    char buffer[512];
    struct Message msg;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Recibir mensaje release usando el socket compartido
    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &addr_len);
    deserialize_message(buffer, &msg);

    if (msg.message_type == DHCP_RELEASE) {
        printf("Received DHCP Release from %s\n", msg.client_mac);
        reclaim_ip(server, msg.client_mac);
    }
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

int main() {
    // Crear una instancia del servidor DHCP
    Server server;
    init(&server, "192.168.1.1", 120, "8.8.8.8", "192.168.1.254", "255.255.255.0");

    // Agregar algunas IPs al pool de IPs disponibles para asignar
    strncpy(server.ip_pool[0], "192.168.1.100", sizeof(server.ip_pool[0]));
    strncpy(server.ip_pool[1], "192.168.1.101", sizeof(server.ip_pool[1]));
    strncpy(server.ip_pool[2], "192.168.1.102", sizeof(server.ip_pool[2]));
    server.ip_pool_count = 3;

    // Crear un socket para la comunicación DHCP
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("No se pudo crear el socket");
        return 1;
    }

    // Configurar opciones del socket
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt falló");
        close(sockfd);
        return 1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // Realizar bind una sola vez
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind falló");
        close(sockfd);
        return 1;
    }

    // Iniciar escucha para DHCP Discover y Request
    listen_for_discover(&server, sockfd);

    close(sockfd);
    return 0;
}
