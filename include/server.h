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

// HTTP Parser result structure (to be expanded in later phases)
typedef struct {
    char method[8]; // e.g., "GET", "POST"
    char path[256]; // e.g., "/index.html"
    char version[16]; // e.g., "HTTP/1.1"
    
    // body
    char body[1024];
} http_request_t;

/**
 * @brief Initialize the HTTP server with a specific port.
 * 
 * @param server Pointer to the http_server_t structure.
 * @param port The port number to listen on.
 * @return int 0 on success, -1 on failure.
 */
int server_init(http_server_t *server, int port);

/**
 * @brief Start the server's main loop to accept connections.
 * 
 * @param server Pointer to the initialized http_server_t structure.
 */
void server_start(http_server_t *server);

/**
 * @brief Handle a single client connection.
 * 
 * @param client_fd The file descriptor for the client socket.
 */
void handle_client(int client_fd);

/**
 * @brief Parse the raw HTTP request string into a structured format.
 * 
 * Extracts the HTTP method, request path, and HTTP version from the request line.
 * 
 * @param raw_data The raw string containing the HTTP request received from the client.
 * @param request Pointer to the http_request_t structure where the results will be stored.
 * @return int Returns 0 on success, or -1 if the request format is invalid.
 */
int parse_http_request(char *raw_data, http_request_t *request);

/**
 * @brief Send an HTTP response to the client.
 * 
 * @param client_fd The file descriptor for the client socket.
 * @param status_code The HTTP status code (e.g., 200).
 * @param status_msg The HTTP status message (e.g., "OK").
 * @param body The response body.
 * @param content_type The MIME type of the response body.
 */
void send_response(int client_fd, const int status_code, const char *status_msg, const char *body, const char *content_type);

#endif // SERVER_H
