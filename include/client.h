#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

// Estructura del cliente DHCP
struct Client {
    char mac_address[18];           // Dirección MAC real del cliente
    char assigned_ip[16];           // Dirección IP asignada por el servidor
    char assigned_dns[16];          // Dirección del servidor DNS asignado
    char assigned_gateway[16];      // Dirección del gateway asignado
    char assigned_subnet_mask[16];  // Máscara de subred asignada
    int lease_time;                 // Tiempo de lease asignado en segundos
    char server_ip[16];             // Dirección IP del servidor DHCP
    char relay_ip[16];              // Dirección IP del relay (si aplica)
};

// Funciones relacionadas con el cliente

// Función para obtener la dirección MAC real de la interfaz de red
int get_mac_address(char *mac_address);

// Inicializar los atributos del cliente DHCP
void init_client(struct Client *client);

// Funciones de ciclo DHCP
void send_discover(int sockfd, struct sockaddr_in *server_addr, struct Client *client);
void receive_offer(int sockfd, struct Client *client);
void send_request(int sockfd, struct sockaddr_in *server_addr, struct Client *client);
void send_renew_request(int sockfd, struct sockaddr_in *server_addr, struct Client *client);
void send_release(int sockfd, struct sockaddr_in *server_addr, struct Client *client);
void check_lease_time(int sockfd, struct sockaddr_in *server_addr, struct Client *client);
void receive_ack(int sockfd, struct Client *client);  // Recibir DHCP Acknowledgement

// Mostrar la información del cliente
void print_client_info(struct Client *client);

#endif
