#define _GNU_SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include "server.h"
#include "utils/path_utils.h"
#include "utils/mime_utils.h"

// Helper function to serve a static file
void serve_file(int client_fd, const char *full_path) {
    FILE *file = fopen(full_path, "rb");
    
    // If file is not found, try to serve 404.html
    if (file == NULL) {
        perror("fopen failed");
        // Try serving our custom 404 page
        file = fopen("public/404.html", "rb");
        if (file == NULL) {
            // Ultimate fallback if even 404.html is missing
            const char *not_found_msg = "<h1>404 Not Found</h1>";
            send_response(client_fd, 404, "Not Found", not_found_msg, "text/html");
            return;
        }
        
        // If we found 404.html, we need to send the 404 status code header first
        struct stat st;
        stat("public/404.html", &st);
        char header[1024];
        snprintf(header, sizeof(header), 
                 "HTTP/1.1 404 Not Found\r\n"
                 "Content-Type: text/html\r\n"
                 "Content-Length: %ld\r\n"
                 "Connection: close\r\n\r\n", 
                 st.st_size);
        send(client_fd, header, strlen(header), 0);
    } else {
        // Normal 200 OK case
        struct stat st;
        stat(full_path, &st);
        long file_size = st.st_size;
        const char *content_type = get_mime_type(full_path);

        char header[1024];
        snprintf(header, sizeof(header), 
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: %s\r\n"
                 "Content-Length: %ld\r\n"
                 "Connection: close\r\n\r\n", 
                 content_type, file_size);
        send(client_fd, header, strlen(header), 0);
    }

    // Common file sending logic
    char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_fd, buffer, bytes_read, 0);
    }

    fclose(file);
}

int server_init(http_server_t *server, int port) {
    server->port = port;
    int opt = 1;

    if ((server->server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        return -1;
    }

    if (setsockopt(server->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt failed");
        return -1;
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY; 
    server->address.sin_port = htons(server->port);

    if (bind(server->server_fd, (struct sockaddr *)&server->address, sizeof(server->address)) < 0) {
        perror("bind failed");
        return -1;
    }

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
    char buffer[2048] = {0};
    char full_path[1024] = {0};
    http_request_t request;
    
    // Read the request
    ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_received < 0) {
        perror("read failed");
        close(client_fd);
        return;
    }
    buffer[bytes_received] = '\0';

    // Parse the HTTP request
    if (parse_http_request(buffer, &request) == -1) {
        printf("Failed to parse HTTP request\n");
        send_response(client_fd, 400, "Bad Request", "<h1>400 Bad Request</h1>", "text/html");
    } else {
        // Resolve URL to file path
        get_full_path(request.path, full_path);
        printf("Serving file: %s\n", full_path);
        
        // Serve the file
        serve_file(client_fd, full_path);
    }

    close(client_fd);
}

int parse_http_request(char *raw_data, http_request_t *request) {
    char *line_end = strstr(raw_data, "\r\n");  
    if (!line_end) return -1;
    *line_end = '\0';

    char *method = strtok(raw_data, " ");
    char *path = strtok(NULL, " ");
    char *version = strtok(NULL, " ");

    if (!method || !path || !version) return -1;

    strncpy(request->method, method, sizeof(request->method) - 1);
    request->method[sizeof(request->method) - 1] = '\0';
    strncpy(request->path, path, sizeof(request->path) - 1);
    request->path[sizeof(request->path) - 1] = '\0';
    strncpy(request->version, version, sizeof(request->version) - 1);
    request->version[sizeof(request->version) - 1] = '\0';

    return 0;
}

void send_response(int client_fd, const int status_code, const char *status_msg, const char *body, const char *content_type) {
    char header[1024];
    int body_length = strlen(body);
    snprintf(header, sizeof(header), 
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %d\r\n"
             "Connection: close\r\n\r\n", 
             status_code, status_msg, content_type, body_length);
    send(client_fd, header, strlen(header), 0);

    if (body_length > 0) {
        send(client_fd, body, body_length, 0);
    }
}
