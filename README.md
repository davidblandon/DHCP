## 1. Introducción

Este repositorio contiene la implementación de un servidor y un cliente DHCP.

### Integrantes:
- Juan Martin Espitia
- David Blandon
- Natalia Ceballos 

En este readme se puede encontrar un resumen de todo lo trabajado y aprendido en el desarrollo de esta aplicacion, todo lo que no sea explicado explicitamente en este readme tambien puede ser leido del codigo.

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
- Diagrama de Clases:
  
![image](https://github.com/user-attachments/assets/05e010c3-2709-4ecf-bba4-ef67ecac7d66)

- Diagrama de Componentes:
  
![image](https://github.com/user-attachments/assets/0104355d-359f-40eb-a6f8-ce78b2d958fa)

Usando los diagramas y despues de haber escogido el tipo de socket, empezamos a desarrollar el proyecto. A continuacion, en el video les explicaremos el codigo, las funcionalidades que tiene y como correrlo.

### Video
https://youtu.be/24uoMqJmL0Y?feature=shared

## 3. Aspectos Logrados y No logrados
En este proyecto, se alcanzaron varios objetivos. El servidor y los clientes pueden mandar y recibir mensajes estando en la misma red.
El cliente manda los mensajes broadcast DHCPDISCOVER y DHCPREQUEST de forma correcta. El servidor manda los mensajes DHCPOFFER y DHCPACK de froma correcta y también logra recibir solicitudes simultaneamente usando threading. 
Finalmente, se hace uso de la API Sockets Berkeley para usar UDP en nuestro proyecto.

Sin embargo, quedaron algunos aspectos sin implementar como la completitud del mensaje es decir, a veces el mensaje no se manda completo y el manage lease
ya que el servidor no comprueba si una IP cumplió su tiempo de alquiler, finalmente, tampoco pudimos lograr que el servidor y el cloente se comunicaran del todo estando en redes distintas.

## 4. Conclusiones
Este proyecto nos ayudo a entender mejor el funcionamiendo de un DHCP, de sockets (UDP y TCP) en C y más, 
fue un reto interesante para construir y entender sistemas a nivel de infrastuctura de red.
Para el desarrollo de este proyecto también fue fundamental una buena planeación antes de empezar a desarrollar, 
lo cual nos permitio hacernos una idea de la estructura y funcionamiento de la aplicacion. 

## 5. Referencias 
[UDP Client Server using connect](https://www.geeksforgeeks.org/udp-client-server-using-connect-c-implementation/)

[Beej's Guide to C Programming](https://beej.us/guide/bgc/html/split/)

[Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/split/)

[DHCP](https://datatracker.ietf.org/doc/html/rfc2131)

