#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <string>
#include <signal.h>
#define PORT 8080
#define BUFFER_SIZE 4096


class Server {
    private:
        int _Tcpsocketfd;
    public:
        Server();
        ~Server();
        int getsocketfd() const;
        int run();
};

void handle_inter(int fd){
    close(fd);
    return ;
}

#endif