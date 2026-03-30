# GEMINI.md - Project Context & Development Plan

## Project Overview
`chttpd` is a lightweight, concurrent HTTP server implemented in C. It aims to provide a robust foundation for learning socket programming, HTTP protocol handling, and concurrent systems.

- **Main Technologies**: C (C99/C11), POSIX Sockets, Pthreads (for concurrency), and optionally `epoll` for event-driven IO.
- **Architecture**: Modular design with separate components for socket management, request parsing, response generation, and file serving.

## Building and Running
- **Build**: `make` (uses `gcc` and `pthread`).
- **Clean**: `make clean`.
- **Run**: `./chttpd` (TODO: Add command-line arguments for port and root directory).
- **Test**: `make test` (TODO: Implement test suite).

## Development Conventions
- **Code Style**: K&R style or Linux Kernel style.
- **Naming**: `snake_case` for functions and variables.
- **Error Handling**: Functions should return integers (0 for success, -1 or error code for failure) and use `errno`.
- **Memory Management**: Explicit allocation (`malloc`/`free`) with careful tracking. Prefer stack allocation where possible.
- **Documentation**: Use Doxygen-style comments for headers.

## 專案開發計畫 (Project Development Plan)

### Phase 1: Foundation (Networking & Sockets)
1.  Implement a basic TCP server that listens on a port.
2.  Handle incoming connections and print "Hello, World!" to the client.
3.  Support non-blocking I/O or basic socket timeouts.

### Phase 2: HTTP Protocol Handling
1.  **Request Parsing**: Implement a parser for HTTP Request-Lines (`GET /index.html HTTP/1.1`) and headers.
2.  **Response Generation**: Build a utility to generate standard HTTP responses (Headers, Status Codes, Body).
3.  **Error Responses**: Handle 400 (Bad Request), 404 (Not Found), and 500 (Internal Server Error).

### Phase 3: File Serving & MIME Types
1.  Serve static files from the `/public` directory.
2.  Implement a simple MIME type mapping (e.g., `.html` -> `text/html`, `.css` -> `text/css`, `.jpg` -> `image/jpeg`).
3.  Support `Connection: keep-alive`.

### Phase 4: Concurrency & Performance
1.  **Multi-threading**: Implement a thread pool (Pthreads) to handle multiple concurrent clients.
2.  **Alternative**: Explore `epoll` (Linux specific) for a high-performance event-driven model.
3.  Implement basic logging (access logs, error logs).

### Phase 5: Robustness & Security
1.  **Path Sanitization**: Prevent directory traversal attacks (e.g., `GET /../../etc/passwd`).
2.  **Graceful Shutdown**: Handle `SIGINT` and `SIGTERM` to close sockets and free resources.
3.  **Configuration**: Load configuration from a file or environment variables.

---
*Note: This file is used as instructional context for Gemini CLI. Update this file as the project evolves.*
