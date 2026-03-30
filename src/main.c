#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "server.h"

http_server_t server;

// Basic signal handler for graceful shutdown (to be expanded in later phases)
void handle_sigint(int sig) {
    printf("\nServer stopping... (Signal %d)\n", sig);
    close(server.server_fd);
    exit(0);
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;

    if (argc > 1) {
        port = atoi(argv[1]);
    }

    signal(SIGINT, handle_sigint);

    if (server_init(&server, port) < 0) {
        fprintf(stderr, "Failed to initialize server\n");
        return EXIT_FAILURE;
    }

    server_start(&server);

    return EXIT_SUCCESS;
}
