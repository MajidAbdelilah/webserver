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
#include <poll.h>
#include <algorithm>
#include <sys/event.h>

#define PORT 8080
#define BUFFER_SIZE 30000
#define MAX_EVENTS 10

class Server {
    private:
        Server();
        int _Tcpsocketfd;
        int _domain;
        int _type;
        int _protocol;
        int _port;
        int _interface;
        int _backlog;
        struct sockaddr_in hostaddr;
        std::map<std::string , std::string > req_head;

    public:
        Server(int , int , int , int , u_int32_t , int);
        ~Server();
        int getsocketfd() const;
        int Filldata();
        int run();
        int getting_req(struct kevent* , int kq, int cli_sock);
        int ParseRequest(std::vector<char> &);
};

#endif