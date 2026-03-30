#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server.h"

int server_init(http_server_t *server, int port) {
    server->port = port;
    int opt = 1;

    // Create socket
    if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return -1;
    }

    // Set socket options to reuse address and port
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        return -1;
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(server->port);

    // Bind socket to the address and port
    if (bind(server->server_fd, (struct sockaddr *)&server->address, sizeof(server->address)) < 0) {
        perror("bind failed");
        return -1;
    }

    // Start listening
    if (listen(server->server_fd, BACKLOG) < 0) {
        perror("listen failed");
        return -1;
    }

    printf("Server initialized on port %d\n", server->port);
    return 0;
}

void server_start(http_server_t *server) {
    int client_fd;
    struct sockaddr_in client_address;
    int addrlen = sizeof(client_address);

    printf("Waiting for connections...\n");

    while (1) {
        if ((client_fd = accept(server->server_fd, (struct sockaddr *)&client_address, (socklen_t*)&addrlen)) < 0) {
            perror("accept failed");
            continue;
        }

        printf("New connection accepted from %s:%d\n", 
               inet_ntoa(client_address.sin_addr), 
               ntohs(client_address.sin_port));

        handle_client(client_fd);
    }
}

void handle_client(int client_fd) {
    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, World!";
    
    // Basic "Hello, World!" response
    send(client_fd, response, strlen(response), 0);
    close(client_fd);
}
