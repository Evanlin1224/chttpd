#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#define DEFAULT_PORT 8080
#define BACKLOG 10

typedef struct {
    int port;
    int server_fd;
    struct sockaddr_in address;
} http_server_t;

int server_init(http_server_t *server, int port);
void server_start(http_server_t *server);
void handle_client(int client_fd);

#endif // SERVER_H
