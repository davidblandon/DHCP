#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <stdbool.h>

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
    char ip_pool[MAX_IPS][16];
    char relay_agents[MAX_RELAY_AGENTS][16];
    int leased_ip_count;
    int ip_pool_count;
    int relay_agent_count;
} Server;

// Constructor
void init(Server* server, const char* ip, int lease_time_default, const char* dns, const char* gateway, const char* subnet_mask);

// Methods
void listen_for_discover(Server* server);
void send_offer(Server* server);
void process_request(Server* server);
void send_ack(Server* server);
void process_renew_request(Server* server);
void renew_lease(Server* server);
void process_release(Server* server);
void reclaim_ip(Server* server);
void manage_leases(Server* server);

#endif // SERVER_H