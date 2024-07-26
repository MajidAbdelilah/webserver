# Webserver

Introduction to HTTP
====================

What is a Socket?
-----------------

A socket is an endpoint for sending or receiving data across a network. Sockets are used to establish communication between a client and a server.

Types of Sockets
----------------

### Stream Sockets (SOCK_STREAM)

Stream sockets use TCP (Transmission Control Protocol) to provide a reliable, two-way connection-based byte stream.

### Datagram Sockets (SOCK_DGRAM)

Datagram sockets use UDP (User Datagram Protocol) for connectionless, unreliable messages.

System Calls for Socket Creation
--------------------------------

### socket()

Creates a new socket. Takes three integer parameters:

```cpp
int socket(int domain, int type, int protocol);

```

-   **domain**: Specifies the protocol family (e.g., AF_INET for IPv4).
-   **type**: The communication type (e.g., SOCK_STREAM for TCP or SOCK_DGRAM for UDP).
-   **protocol**: The particular protocol to be used with the socket. The value 0 is given for the default protocol.

The return value of `socket()` is a positive integer or -1 in case of an error.

### setsockopt()

Sets options on a socket.



```cpp
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
```

-   **sockfd**: The socket file descriptor.
-   **level**: The level where the option is defined.
-   **optname**: The name of the option.
-   **optval**: A pointer to the option value.
-   **optlen**: The size of the option value.

### bind()

Binds a socket to an address and port number.


```cpp
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

-   **sockfd**: The socket file descriptor.
-   **addr**: A pointer to a sockaddr struct representing the address to bind to.
-   **addrlen**: The length of the address structure.

### listen()

Marks the socket as a passive socket, meaning it will be used to accept incoming connection requests.



```cpp
int listen(int sockfd, int backlog);
```

-   **sockfd**: The socket file descriptor.
-   **backlog**: The maximum length of the queue of pending connections.

### accept()

Accepts a connection on a socket.


`int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);`

-   **sockfd**: The socket file descriptor.
-   **addr**: A pointer to a sockaddr struct that will hold the address of the connecting client.
-   **addrlen**: A pointer to the length of the address struct.

### connect()

Initiates a connection on a socket (used on the client side).




```cpp
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

-   **sockfd**: The socket file descriptor.
-   **addr**: A pointer to a sockaddr struct representing the address to connect to.
-   **addrlen**: The length of the address structure.

### recv()

Receives data from a connected socket.


```cpp
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

-   **sockfd**: The socket file descriptor.
-   **buf**: The buffer to store the received data.
-   **len**: The length of the buffer.
-   **flags**: Flags for receiving data.

### send()

Sends data to a connected socket.


```cpp
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

-   **sockfd**: The socket file descriptor.
-   **buf**: The buffer containing data to send.
-   **len**: The length of the data.
-   **flags**: Flags for sending data.

### close()

Closes a socket.


```cpp
int close(int sockfd);
```

-   **sockfd**: The socket file descriptor to close.

Poll And Select
===============

to check if input and output is possible , we use these function to monitor multiple file descriptors.

poll() : waits for an event on an fd.

```cpp
int poll(struct pollfd *fds, nfds_t nfds, int timeout)
```

fds : array of pollfd structures

nfds : number of fds

timeout : time to wait in miliseconds

select() : monitors multiple file descriptors to see if they are ready for any Input / output.

```cpp
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
```

nfds : highest number of fd + 1

readfds : set of file descriptors to check for readability

writefds : set of file descriptors to check for writability

exceptfds : set of file descriptors to check for exceptions

timeout : maximum time to wait
Sets options on a socket.

<!-- # Webserver Project - Task Repartition

## Team Members
- **Yassine** - Server and Response Part
- **Majid** - Request Part
- **Salah** - Configuration File and CGI

---

## Detailed Task Repartition
    
### Yassine - Server and Response Part

**Tasks:**

1. **Create the Server:**
   - Set up the server to listen on multiple ports as specified in the configuration file.
   - Implement non-blocking I/O operations using `poll()`, `select()`, or `epoll()` to manage multiple connections efficiently.
   - Ensure the server can handle multiple clients simultaneously without blocking.

2. **HTTP Response Handling:**
   - Implement the logic to construct and send HTTP responses.
   - Ensure accurate HTTP response status codes (e.g., 200, 404, 500).
   - Develop default error pages for common HTTP error statuses.
   - Handle GET, POST, and DELETE requests as per the requirements.
   - Manage file uploads from clients and save them to the specified directory.
   - Implement functionality to serve static files (HTML, CSS, JS, images, etc.).

3. **Testing and Debugging:**
   - Conduct stress tests to ensure the server remains available under high load.
   - Use tools like NGINX and `telnet` to compare behaviors and validate HTTP responses.

---

### Majid - Request Part

**Tasks:**

1. **HTTP Request Parsing:**
   - Implement parsing of incoming HTTP requests, including the request line, headers, and body.
   - Handle different HTTP methods (GET, POST, DELETE) and ensure they are processed correctly.
   - Implement request validation to ensure proper formatting and completeness.

2. **Client Management:**
   - Manage client connections, including accepting new connections and reading data from clients.
   - Ensure non-blocking reads using `poll()`, `select()`, or `epoll()` before processing requests.

3. **Error Handling:**
   - Implement proper error handling for invalid requests or unsupported methods.
   - Return appropriate error responses to clients for various error conditions.

4. **Testing and Debugging:**
   - Develop tests to ensure correct parsing and handling of different types of requests.
   - Use tools like `curl` and `telnet` to test different scenarios and edge cases.

---

### Salah - Configuration File and CGI

**Tasks:**

1. **Configuration File Parsing:**
   - Design and implement the parsing logic for the configuration file.
   - Allow configuration of server settings such as port, host, server names, default error pages, and client body size limits.
   - Configure routes with rules such as accepted HTTP methods, redirections, directory listings, default files, and CGI execution.

2. **CGI Implementation:**
   - Implement the execution of CGI scripts based on file extensions (e.g., .php).
   - Handle CGI script execution with the proper environment and PATH_INFO.
   - Ensure the server unchunks chunked requests before passing them to the CGI.
   - Implement correct handling of CGI output, marking the end of data with EOF if no content length is specified.

3. **Configuration Management:**
   - Enable the server to dynamically reload or update configurations without downtime.
   - Ensure the server behaves correctly based on the configuration settings provided.

4. **Testing and Debugging:**
   - Develop example configuration files to demonstrate different features and settings.
   - Test CGI execution with scripts written in languages like PHP and Python.
   - Ensure the configuration parser handles invalid configurations gracefully and provides useful error messages.

---

## Collaboration and Integration

### Tasks for Each Member

1. **Regular Meetings:**
   - We will conduct regular team meetings to discuss our progress, address any challenges, and plan our integration steps.

2. **Code Integration:**
   - Once each of you has completed your respective tasks, we will merge our individual components (server, request handling, configuration, and CGI) into a cohesive system. 
   - We will then conduct integration tests to ensure that all parts work together seamlessly.

3. **Documentation:**
   - Ensure your code is thoroughly documented, including comments and README files for each component. 
   - Provide usage instructions and examples for setting up and running the server.

4. **Final Testing and Evaluation:**
   - After integrating all parts, we will perform comprehensive testing of the entire system to ensure it meets all requirements. 
   - Finally, we will prepare the project for submission and peer evaluation, ensuring all files are correctly named and organized in the repository. -->