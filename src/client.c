#include <stdio.h>
#include "utils.c"
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include "client.h"
#include "message.h"

// Variables globales
int sockfd;
struct Client client;
struct sockaddr_in server_addr;

// Función para obtener la dirección MAC real
int get_mac_address(char *mac_address) {
    int fd;
    struct ifreq ifr;
    char *iface = "eth0";  // Cambia esto según la interfaz de red en tu sistema

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("Error abriendo el socket para MAC");
        return -1;
    }

    strncpy(ifr.ifr_name, iface, IFNAMSIZ-1);

    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
        perror("Error obteniendo la dirección MAC");
        close(fd);
        return -1;
    }

    close(fd);

    // Convertir la dirección MAC en formato legible
    unsigned char *mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;
    sprintf(mac_address, "%02x:%02x:%02x:%02x:%02x:%02x",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return 0;
}

// Inicializar los atributos del cliente DHCP
void init_client(struct Client *client) {
    get_mac_address(client->mac_address);
    strncpy(client->assigned_ip, "0.0.0.0", sizeof(client->assigned_ip));
    strncpy(client->assigned_dns, "0.0.0.0", sizeof(client->assigned_dns));
    strncpy(client->assigned_gateway, "0.0.0.0", sizeof(client->assigned_gateway));
    strncpy(client->assigned_subnet_mask, "0.0.0.0", sizeof(client->assigned_subnet_mask));
    client->lease_time = 0;
    strncpy(client->server_ip, "192.168.1.1", sizeof(client->server_ip));  // Cambiar según el servidor
    strncpy(client->relay_ip, "0.0.0.0", sizeof(client->relay_ip));  // Si aplica
}

// Función para enviar DHCP Discover
void send_discover(int sockfd, struct sockaddr_in *server_addr, struct Client *client) {
    struct Message msg;
    char buffer[1024];

    msg.message_type = DHCP_DISCOVER;
    strncpy(msg.client_mac, client->mac_address, sizeof(msg.client_mac));
    strncpy(msg.ip_address, client->assigned_ip, sizeof(msg.ip_address));
    strncpy(msg.dns, client->assigned_dns, sizeof(msg.dns));
    strncpy(msg.gateway, client->assigned_gateway, sizeof(msg.gateway));
    strncpy(msg.subnet_mask, client->assigned_subnet_mask, sizeof(msg.subnet_mask));
    strncpy(msg.sender, "Client", sizeof(msg.sender));

    serialize_message(&msg, buffer);

    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("Error enviando DHCP Discover");
    } else {
        print_message_sent (&msg);
    }
}

// Función para recibir DHCP Offer
void receive_offer(int sockfd, struct Client *client) {
    
    
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    socklen_t addr_len;
    struct sockaddr_in server_addr;

    if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &addr_len) == -1) {
        perror("Error recibiendo DHCP Offer");
    } else {
        struct Message msg;
        deserialize_message(buffer, &msg);
        print_message_received(&msg);
        printf("DNS: %s", msg.dns);
        // Asignar los valores al cliente
        strncpy(client->assigned_ip, msg.ip_address, sizeof(client->assigned_ip));
        strncpy(client->assigned_dns, msg.dns, sizeof(client->assigned_dns));
        strncpy(client->assigned_gateway, msg.gateway, sizeof(client->assigned_gateway));
        strncpy(client->assigned_subnet_mask, msg.subnet_mask, sizeof(client->assigned_subnet_mask));

        printf("Valores asignados en receive offer");
        
    }
}

// Función para enviar DHCP Request
void send_request(int sockfd, struct sockaddr_in *server_addr, struct Client *client) {
    struct Message msg;
    char buffer[1024];

    msg.message_type = DHCP_REQUEST;
    strncpy(msg.client_mac, client->mac_address, sizeof(msg.client_mac));
    strncpy(msg.sender, "Client", sizeof(msg.sender));
    strncpy(msg.ip_address, client->assigned_ip, sizeof(msg.ip_address));
    strncpy(msg.dns, client->assigned_dns, sizeof(msg.dns));
    strncpy(msg.gateway, client->assigned_gateway, sizeof(msg.gateway));
    strncpy(msg.subnet_mask, client->assigned_subnet_mask, sizeof(msg.subnet_mask));
    serialize_message(&msg, buffer);
    print_message_sent(&msg);

    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("Error enviando DHCP Request");
    } else {
        printf("DHCP Request enviado para la IP: %s\n", client->assigned_ip);
    }
}

// Función para recibir DHCP Acknowledgement
void receive_ack(int sockfd, struct Client *client) {
    char buffer[1024];
    socklen_t addr_len;
    struct sockaddr_in server_addr;

    if (recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&server_addr, &addr_len) == -1) {
        perror("Error recibiendo DHCP Acknowledgement");
    } else {
        struct Message msg;
        deserialize_message(buffer, &msg);

        print_message_received(&msg);
        strncpy(msg.dns, client->assigned_dns, sizeof(msg.dns));
        strncpy(msg.gateway, client->assigned_gateway, sizeof(msg.gateway));
        strncpy(msg.subnet_mask, client->assigned_subnet_mask, sizeof(msg.subnet_mask));
        client->lease_time = msg.lease_time;  // Actualizar el lease time si viene en el ACK
    }
}

// Función para enviar DHCP Renew Request
void send_renew_request(int sockfd, struct sockaddr_in *server_addr, struct Client *client) {
    struct Message msg;
    char buffer[1024];

    // Configurar el mensaje DHCP Request para renovar el lease
    msg.message_type = DHCP_REQUEST;
    strncpy(msg.client_mac, client->mac_address, sizeof(msg.client_mac));
    strncpy(msg.sender, "Client", sizeof(msg.sender));
    strncpy(msg.ip_address, client->assigned_ip, sizeof(msg.ip_address));

    // Serializar el mensaje para enviarlo por la red
    serialize_message(&msg, buffer);

    // Enviar el mensaje de renovación al servidor
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("Error enviando DHCP Renew Request");
    } else {
        print_message_sent(&msg);
        // Esperar la respuesta del servidor (ACK)
        receive_ack(sockfd, client);

        // Una vez recibido el ACK, continuar
        printf("Renovación de IP completada con éxito: %s\n", client->assigned_ip);
    }
}


// Función para enviar DHCP Release
void send_release(int sockfd, struct sockaddr_in *server_addr, struct Client *client) {
    struct Message msg;
    char buffer[1024];

    msg.message_type = DHCP_RELEASE;
    strncpy(msg.client_mac, client->mac_address, sizeof(msg.client_mac));
    strncpy(msg.sender, "Client", sizeof(msg.sender));
    strncpy(msg.ip_address, client->assigned_ip, sizeof(msg.ip_address));
    strncpy(msg.dns, client->assigned_dns, sizeof(msg.dns));
    strncpy(msg.subnet_mask, client->assigned_subnet_mask, sizeof(msg.subnet_mask));

    serialize_message(&msg, buffer);

    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)server_addr, sizeof(*server_addr)) == -1) {
        perror("Error enviando DHCP Release");
    } else {
        print_message_sent(&msg);
    }
}

// Función para verificar el tiempo de lease
void check_lease_time(int sockfd, struct sockaddr_in *server_addr, struct Client *client) {
    static time_t last_check_time = 0;
    time_t current_time = time(NULL);

    // Inicializar last_check_time en la primera ejecución
    if (last_check_time == 0) {
        last_check_time = current_time;
    }

    // Solo intentar renovar si ha pasado más de la mitad del tiempo del lease
    if (current_time - last_check_time >= client->lease_time / 2) {
        printf("El tiempo de lease está por expirar. Intentando renovar IP: %s\n", client->assigned_ip);
        send_renew_request(sockfd, server_addr, client);
        last_check_time = current_time;
    }
}


// Mostrar la información del cliente
void print_client_info(struct Client *client) {
    printf("Información del cliente DHCP:\n");
    printf("MAC Address: %s\n", client->mac_address);
    printf("IP Asignada: %s\n", client->assigned_ip);
    printf("DNS Asignado: %s\n", client->assigned_dns);
    printf("Gateway Asignado: %s\n", client->assigned_gateway);
    printf("Máscara de Subred: %s\n", client->assigned_subnet_mask);
    printf("Tiempo de Lease: %d segundos\n", client->lease_time);
    printf("IP del Servidor DHCP: %s\n", client->server_ip);
    printf("IP del Relay (si aplica): %s\n", client->relay_ip);
}

// Manejador de señales para enviar DHCP Release antes de salir
void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGHUP) {
        printf("Interrupción recibida, enviando DHCP Release...\n");
        send_release(sockfd, &server_addr, &client);
        close(sockfd);
        printf("Socket cerrado. Saliendo del programa.\n");
        exit(0);  // Terminar el programa
    }
}


int main() {
    // Configurar el manejador de señales
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);  // Capturar Ctrl+C
    sigaction(SIGHUP, &sa, NULL);  // Capturar cierre de terminal

    // Inicializar el cliente DHCP
    init_client(&client);

    // Crear un socket para la comunicación DHCP
    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sockfd == -1) {
        perror("No se pudo crear el socket");
        return 1;
    }

    // Configurar la dirección del servidor DHCP
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);  // Puerto 67 para DHCP server
    inet_pton(AF_INET, "255.255.255.255", &server_addr.sin_addr);  // La IP del servidor

    int broadcastEnable = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
    // Enviar DHCP Discover
    send_discover(sockfd, &server_addr, &client);

    // Recibir DHCP Offer
    receive_offer(sockfd, &client);

    // Enviar DHCP Request
    send_request(sockfd, &server_addr, &client);

    // Recibir DHCP Acknowledgement
    receive_ack(sockfd, &client);

    // Monitorizar el tiempo de lease
    while (1) {
        check_lease_time(sockfd, &server_addr, &client);
    }

    return 0;
}
