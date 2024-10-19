# Variables
CC = gcc
CFLAGS = -Wall -g -I../include
LDFLAGS = -pthread
SRC_DIR = ../src
OBJ_DIR = obj
OBJ_SERVER = $(OBJ_DIR)/server.o $(OBJ_DIR)/message.o
OBJ_CLIENT = $(OBJ_DIR)/client.o $(OBJ_DIR)/message.o
TARGET_SERVER = dhcp_server
TARGET_CLIENT = dhcp_client
TARGET_RELAY = dhcp_relay

# Reglas

all: $(TARGET_SERVER) $(TARGET_CLIENT) $(TARGET_RELAY)

# Regla para compilar el servidor DHCP
$(TARGET_SERVER): $(OBJ_SERVER)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET_SERVER) $(OBJ_SERVER)

# Regla para compilar el cliente DHCP
$(TARGET_CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -o $(TARGET_CLIENT) $(OBJ_CLIENT)
#Regla para compilar el relay
$(TARGET_RELAY): 
	$(CC) $(CFLAGS) $(SRC_DIR)/relay_handler.c  -o $(TARGET_RELAY) 

# Regla para compilar el archivo server.c
$(OBJ_DIR)/server.o: $(SRC_DIR)/server.c ../include/server.h ../include/message.h ../include/relay_handler.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/server.c -o $(OBJ_DIR)/server.o

# Regla para compilar el archivo client.c
$(OBJ_DIR)/client.o: $(SRC_DIR)/client.c ../include/client.h ../include/message.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/client.c -o $(OBJ_DIR)/client.o

# Regla para compilar el archivo message.c (compartido entre el servidor y el cliente)
$(OBJ_DIR)/message.o: $(SRC_DIR)/message.c ../include/message.h
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/message.c -o $(OBJ_DIR)/message.o

# Limpieza de los archivos compilados
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET_SERVER) $(TARGET_CLIENT)

# Ejecución del servidor
run_server: $(TARGET_SERVER)
	./$(TARGET_SERVER)

# Ejecución del cliente
run_client: $(TARGET_CLIENT)
	./$(TARGET_CLIENT
