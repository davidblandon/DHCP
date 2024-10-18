#ifndef UTILS_H
#define UTILS_H

#include "message.h"

// Función para imprimir mensajes DHCP de manera formateada
void print_message_sent(const struct Message* msg);
void print_message_received(const struct Message* msg);

#endif // UTILS_H