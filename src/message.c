#include "message.h"
#include <string.h>
#include <stdio.h>

// Serializar el mensaje DHCP en un buffer para enviarlo por la red
void serialize_message(struct Message *msg, char *buffer) {
    sprintf(buffer, "%d|%s|%s|%s|%s|%s|%s|%s", 
        msg->message_type, 
        msg->ip_address, 
        msg->client_mac, 
        msg->dns, 
        msg->gateway, 
        msg->subnet_mask, 
        msg->giaddr,
        msg->sender);
}

// Deserializar un mensaje DHCP a partir de un buffer recibido
void deserialize_message(char *buffer, struct Message *msg) {
    sscanf(buffer, "%d|%15[^|]|%17[^|]|%15[^|]|%15[^|]|%15[^|]|%15[^|]|%7[^|]", 
        &msg->message_type, 
        msg->ip_address, 
        msg->client_mac, 
        msg->dns, 
        msg->gateway, 
        msg->subnet_mask, 
        msg->giaddr,
        msg->sender);
}

// Función para imprimir el contenido de un mensaje (para depuración)
void print_message(struct Message *msg) {
    printf("Message Type: %d\n", msg->message_type);
    printf("IP Address: %s\n", msg->ip_address);
    printf("Client MAC: %s\n", msg->client_mac);
    printf("DNS: %s\n", msg->dns);
    printf("Gateway: %s\n", msg->gateway);
    printf("Subnet Mask: %s\n", msg->subnet_mask);
    printf("Relay IP (giaddr): %s\n", msg->giaddr);
    printf("Sender: %s\n", msg->sender);
}
