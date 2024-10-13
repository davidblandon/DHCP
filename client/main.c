#include "dhcp_client.h"
#include <stdio.h>

int main() {
    if (dhcp_discover() != 0) {
        fprintf(stderr, "Failed to send DHCPDISCOVER\n");
        return 1;
    }

    if (dhcp_request() != 0) {
        fprintf(stderr, "Failed to send DHCPREQUEST\n");
        return 1;
    }

    return 0;
}