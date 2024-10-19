#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define MAX_IPS 256
#define MAX_RELAY_AGENTS 10

typedef struct {
    char ip[16];
    char mac[18];
    int lease_time;
} LeasedIP;

typedef struct {
    char ip[16];
    int lease_time_default;
    char dns[256];
    char gateway[16];
    char subnet_mask[16];
    LeasedIP leased_ips[MAX_IPS];
    LeasedIP clients[MAX_IPS];
    char ip_pool[MAX_IPS][16];
    char relay_agents[MAX_RELAY_AGENTS][16];
    int leased_ip_count;
    int ip_pool_count;
    int relay_agent_count;
    int client_count;
} Server;

void send_offer(Server* server, struct sockaddr_in* client_addr, int sockfd);
void send_ack(Server* server, struct sockaddr_in* client_addr, int sockfd, const char* ip_address, int lease_time);
void process_renew_request(Server* server, const char* client_mac);
void reclaim_ip(Server* server, const char* client_mac);

#endif // SERVER_H