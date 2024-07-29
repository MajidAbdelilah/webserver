#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

class Server {
    private:
        int _Tcpsocketfd;
    public:
        Server();
        ~Server();
        int getsocketfd() const;
        int run();
};


#endif