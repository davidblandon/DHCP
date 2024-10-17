#include "message.h"
#include <stdio.h>
#include <string.h>

int main() {
    struct Message msg;
    char buffer[256];

    // Inicializar el mensaje
    msg.message_type = DHCP_OFFER;
    strncpy(msg.ip_address, "192.168.1.10", sizeof(msg.ip_address));
    strncpy(msg.client_mac, "00:11:22:33:44:55", sizeof(msg.client_mac));
    strncpy(msg.dns, "8.8.8.8", sizeof(msg.dns));
    strncpy(msg.gateway, "192.168.1.1", sizeof(msg.gateway));
    strncpy(msg.subnet_mask, "255.255.255.0", sizeof(msg.subnet_mask));
    msg.lease_time = 3600;  // 1 hora de arrendamiento
    strncpy(msg.sender, "Server", sizeof(msg.sender));

    // Imprimir el mensaje original
    printf("Mensaje Original:\n");
    print_message(&msg);

    // Serializar el mensaje
    serialize_message(&msg, buffer);
    printf("\nMensaje Serializado: %s\n", buffer);

    // Crear un nuevo mensaje y deserializar los datos
    struct Message new_msg;
    deserialize_message(buffer, &new_msg);

    // Imprimir el nuevo mensaje deserializado
    printf("\nMensaje Deserializado:\n");
    print_message(&new_msg);

    return 0;
}
