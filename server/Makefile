CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -pthread

all: dhcp_server

dhcp_server: main.o dhcp_server.o
	$(CC) $(CFLAGS) -o dhcp_server main.o dhcp_server.o

main.o: main.c dhcp_server.h
	$(CC) $(CFLAGS) -c main.c

dhcp_server.o: dhcp_server.c dhcp_server.h
	$(CC) $(CFLAGS) -c dhcp_server.c

clean:
	rm -f *.o dhcp_server