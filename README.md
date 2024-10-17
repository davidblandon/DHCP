## 1. Introducción

Este repositorio contiene la implementación de un servidor y un cliente DHCP.

### Integrantes:
- Juan Martin Espitia
- David Blandon
- Natalia Ceballos 

## 2. Desarrollo

### Tipo de Socket (TCP o UDP)
Decidimos usar UDP en nuestra aplicación ya que el protocolo DHCP no necesita las características de TCP, como el reenvío de paquetes perdidos, el control de flujo, o el orden garantizado de los paquetes.

UDP simplemente envía los mensajes, confiando en que la red local es lo suficientemente fiable como para que los paquetes lleguen.
DHCP no necesita la retransmisión automática, ya que está diseñado para que los mensajes puedan ser reenviados de nuevo por el cliente en caso de no recibir una respuesta.

También, UDP no necesita una conexión previa, lo que lo hace mejor para manejar solicitudes simultaneas y además permite que el cliente envíe mensajes de broadcast en la red local, lo cual es esencial para el DHCPDISCOVER. 

Podemos ver en el codigo cómo se crea el socket UDP usando SOCK_DGRAM:

```
 int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
```

### Diagramas
- Diagrama de Clases

- Diagrama de Componentes:
  
![image](https://github.com/user-attachments/assets/950cc553-0c4c-4af8-9223-388e16e7b294)

### Video

## 3. Aspectos Logrados y No logrados

## 4. Conclusiones

## 5. Referencias 
[UDP Client Server using connect](https://www.geeksforgeeks.org/udp-client-server-using-connect-c-implementation/)
