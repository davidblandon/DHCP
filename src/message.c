#include "message.h"
#include <string.h>
#include <stdio.h>

// No se incluyen funciones dentro de Message, ya que la manipulación la realizarán Client y Server.
// Sin embargo, agregamos funciones auxiliares para facilitar la serialización y deserialización del mensaje.

// Serializar el mensaje DHCP en un buffer para enviarlo por la red
void serialize_message(struct Message *msg, char *buffer) {
    sprintf(buffer, "%d|%s|%s|%s|%s|%s|%d|%s", 
        msg->message_type, 
        msg->ip_address, 
        msg->client_mac, 
        msg->dns, 
        msg->gateway, 
        msg->subnet_mask, 
        msg->lease_time,
        msg->sender);
}

// Deserializar un mensaje DHCP a partir de un buffer recibido
void deserialize_message(char *buffer, struct Message *msg) {
    sscanf(buffer, "%d|%15[^|]|%17[^|]|%15[^|]|%15[^|]|%15[^|]|%d|%7[^|]", 
        &msg->message_type, 
        msg->ip_address, 
        msg->client_mac, 
        msg->dns, 
        msg->gateway, 
        msg->subnet_mask, 
        &msg->lease_time,
        msg->sender);
}

// Función para imprimir el contenido de un mensaje (para fines de depuración)
void print_message(struct Message *msg) {
    printf("Message Type: %d\n", msg->message_type);
    printf("IP Address: %s\n", msg->ip_address);
    printf("Client MAC: %s\n", msg->client_mac);
    printf("DNS: %s\n", msg->dns);
    printf("Gateway: %s\n", msg->gateway);
    printf("Subnet Mask: %s\n", msg->subnet_mask);
    printf("Lease Time: %d seconds\n", msg->lease_time);
    printf("Sender: %s\n", msg->sender);
}
