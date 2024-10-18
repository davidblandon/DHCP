#include "utils.h"
#include <stdio.h>

// Definiciones de colores ANSI
#define RESET_COLOR "\033[0m"
#define RED_COLOR "\033[31m"
#define GREEN_COLOR "\033[32m"
#define YELLOW_COLOR "\033[33m"
#define BLUE_COLOR "\033[34m"
#define MAGENTA_COLOR "\033[35m"
#define CYAN_COLOR "\033[36m"
#define WHITE_COLOR "\033[37m"

void print_message_sent(const struct Message* msg) {
    printf(CYAN_COLOR "========================================\n" RESET_COLOR);
    printf(MAGENTA_COLOR "DHCP Message %s\n" RESET_COLOR, "Sent");
    printf(CYAN_COLOR "========================================\n" RESET_COLOR);
    printf(GREEN_COLOR "Message Type: " RESET_COLOR YELLOW_COLOR "%d\n" RESET_COLOR, msg->message_type);
    printf(GREEN_COLOR "Client MAC: " RESET_COLOR YELLOW_COLOR "%s\n" RESET_COLOR, msg->client_mac);
    printf(GREEN_COLOR "IP Address: " RESET_COLOR YELLOW_COLOR "%s\n" RESET_COLOR, msg->ip_address);
    printf(GREEN_COLOR "DNS: " RESET_COLOR YELLOW_COLOR "%s\n" RESET_COLOR, msg->dns);
    printf(GREEN_COLOR "Gateway: " RESET_COLOR YELLOW_COLOR "%s\n" RESET_COLOR, msg->gateway);
    printf(GREEN_COLOR "Subnet Mask: " RESET_COLOR YELLOW_COLOR "%s\n" RESET_COLOR, msg->subnet_mask);
    printf(GREEN_COLOR "Lease Time: " RESET_COLOR YELLOW_COLOR "%d\n" RESET_COLOR, msg->lease_time);
    printf(CYAN_COLOR "========================================\n" RESET_COLOR);
}


void print_message_received(const struct Message* msg) {
    printf(MAGENTA_COLOR "========================================\n" RESET_COLOR);
    printf(CYAN_COLOR "DHCP Message %s\n" RESET_COLOR, "Received");
    printf(MAGENTA_COLOR "========================================\n" RESET_COLOR);
    printf(YELLOW_COLOR "Message Type: " RESET_COLOR GREEN_COLOR "%d\n" RESET_COLOR, msg->message_type);
    printf(YELLOW_COLOR "Client MAC: " RESET_COLOR GREEN_COLOR "%s\n" RESET_COLOR, msg->client_mac);
    printf(YELLOW_COLOR "IP Address: " RESET_COLOR GREEN_COLOR "%s\n" RESET_COLOR, msg->ip_address);
    printf(YELLOW_COLOR "DNS: " RESET_COLOR GREEN_COLOR "%s\n" RESET_COLOR, msg->dns);
    printf(YELLOW_COLOR "Gateway: " RESET_COLOR GREEN_COLOR "%s\n" RESET_COLOR, msg->gateway);
    printf(YELLOW_COLOR "Subnet Mask: " RESET_COLOR GREEN_COLOR "%s\n" RESET_COLOR, msg->subnet_mask);
    printf(YELLOW_COLOR "Lease Time: " RESET_COLOR GREEN_COLOR "%d\n" RESET_COLOR, msg->lease_time);
    printf(MAGENTA_COLOR "========================================\n" RESET_COLOR);
} 