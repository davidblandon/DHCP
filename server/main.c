#include "dhcp_server.h"
#include <stdio.h>

int main() {
    if (start_dhcp_server() != 0) {
        fprintf(stderr, "Failed to start DHCP server\n");
        return 1;
    }

    return 0;
}