#ifndef MESSAGE_H
#define MESSAGE_H

// Definir los tipos de mensajes DHCP
#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_REQUEST 3
#define DHCP_ACK 4
#define DHCP_RELEASE 5

// Estructura del mensaje DHCP
struct Message {
    int message_type;       // Tipo de mensaje (Discover, Offer, etc.)
    char ip_address[16];    // Dirección IP en formato de cadena
    char client_mac[18];    // Dirección MAC del cliente
    char dns[16];           // Servidor DNS
    char gateway[16];       // Gateway predeterminado
    char subnet_mask[16];   // Máscara de subred
    int lease_time;         // Tiempo de arrendamiento en segundos
    char giaddr[16];        // IP del relay, si aplica
    char sender[8];         // "Client" o "Server" según el origen del mensaje
};

#endif
