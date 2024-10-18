CC = gcc
CFLAGS = -Wall
SRC = ../src
INCLUDE = ../include
TESTS = ../tests

# Archivos fuente principales
SRC_FILES = $(SRC)/server.c $(SRC)/client.c $(SRC)/message.c
INCLUDE_FILES = -I$(INCLUDE)

# Compilación del servidor
dhcp_server: $(SRC_FILES)
	$(CC) $(CFLAGS) $(SRC_FILES) -o dhcp_server $(INCLUDE_FILES)

# Compilación del cliente
dhcp_client: $(SRC)/client.c $(SRC)/message.c
	$(CC) $(CFLAGS) $(SRC)/client.c $(SRC)/message.c -o dhcp_client $(INCLUDE_FILES)

# Compilación del archivo de prueba para la clase Message
test_message: $(TESTS)/test_message.c $(SRC)/message.c
	$(CC) $(CFLAGS) $(TESTS)/test_message.c $(SRC)/message.c -o test_message $(INCLUDE_FILES)

# Limpiar binarios
clean:
	rm -f dhcp_server dhcp_client test_message
