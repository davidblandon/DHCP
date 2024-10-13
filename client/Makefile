CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99

all: dhcp_client

dhcp_client: main.o dhcp_client.o
	$(CC) $(CFLAGS) -o dhcp_client main.o dhcp_client.o

main.o: main.c dhcp_client.h
	$(CC) $(CFLAGS) -c main.c

dhcp_client.o: dhcp_client.c dhcp_client.h
	$(CC) $(CFLAGS) -c dhcp_client.c

clean:
	rm -f *.o dhcp_client