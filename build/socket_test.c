#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 2000  // Cambia este puerto según lo que necesites

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    int opt = 1;

    // Crear el socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando el socket");
        exit(EXIT_FAILURE);
    }

    // Permitir reutilizar la dirección y el puerto
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt falló");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    
    // Usa INADDR_ANY para enlazar a todas las interfaces disponibles o especifica una IP
    server_addr.sin_addr.s_addr = INADDR_ANY;  // O usa inet_addr("127.0.0.1") o inet_addr("192.168.1.x")
    
    // Establecer el puerto
    server_addr.sin_port = htons(PORT);

    // Intentar enlazar (bind) el socket a la dirección IP y puerto especificado
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Socket enlazado exitosamente a la IP y puerto especificado.\n");
    printf("Esperando mensajes...\n");

    // Bucle infinito para esperar mensajes entrantes
    while (1) {
        char buffer[1024];
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_len);
        
        if (len > 0) {
            buffer[len] = '\0';
            printf("Mensaje recibido: %s\n", buffer);
        } else {
            perror("Error al recibir mensaje");
        }
    }

    // Cerrar el socket
    close(sockfd);

    return 0;
}
