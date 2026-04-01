#define _GNU_SOURCE // Enable GNU extensions for better error handling and socket options
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
    if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    // Set socket options to reuse address and port
    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        return -1;
    }

    server->address.sin_family = AF_INET;
    // INADDR_ANY allows the server to accept connections on any of the host's IP addresses
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
    char buffer[1024] = {0};
    http_request_t request;
    read(client_fd, buffer, 1024); // read from client (web browser)
    printf("Received request:\n%s\n", buffer);

    // Parse the HTTP request
    if (parse_http_request(buffer, &request) == -1) {
        printf("Failed to parse HTTP request\n");
        send_response(client_fd, 400, "Bad Request", "Invalid HTTP request format");
    } else if (strcmp(request.path, "/") == 0) {
        // TODO: I think we should have a more robust routing mechanism, but for now we will just check if the path is "/" and return a simple response.
        send_response(client_fd, 200, "OK", "Welcome to the C HTTP Server!");
    } else {
        send_response(client_fd, 404, "Not Found", "The requested resource was not found on this server.");
    }

    printf("Parsed Request - Method: %s, Path: %s, Version: %s\n", request.method, request.path, request.version);
    close(client_fd);
}

int parse_http_request(char *raw_data, http_request_t *request) {
    // 1. parse the request line (e.g., "GET /index.html HTTP/1.1")
    char *line_end = strstr(raw_data, "\r\n");  
    if (!line_end) {
        return -1;
    }
    *line_end = '\0';
    /* FIXME: Using strtok is mutable, so we need to be careful */
    char *method = strtok(raw_data, " ");
    char *path = strtok(NULL, " ");
    char *version = strtok(NULL, " ");

    if (!method || !path || !version) {
        return -1;
    }

    strncpy(request->method, method, sizeof(request->method) - 1);
    request->method[sizeof(request->method) - 1] = '\0';
    strncpy(request->path, path, sizeof(request->path) - 1);
    request->path[sizeof(request->path) - 1] = '\0';
    strncpy(request->version, version, sizeof(request->version) - 1);
    request->version[sizeof(request->version) - 1] = '\0';


    // 2. parse the body (for simplicity, we assume the body starts after the first blank line)

    // 3. In a real implementation, we would also need to parse headers to determine the content length and properly read the body, but for now we will just copy the remaining data as the body.

    return 0;
}

void send_response(int client_fd, const int status_code, const char *status_msg, const char *body) {
    char header[2048];
    int body_length = strlen(body);
    snprintf(header, sizeof(header), "HTTP/1.1 %d %s\r\nContent-Type: text/plain\r\nContent-Length: %d\r\n\r\n", 
             status_code, status_msg, body_length);
    send(client_fd, header, strlen(header), 0);

    if (body_length > 0) {
        send(client_fd, body, body_length, 0);
    }
}
