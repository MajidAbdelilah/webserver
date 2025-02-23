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
#include <netdb.h>
#include <fcntl.h>

#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

#define PORT 8080
#define BUFFER_SIZE 30000
#define MAX_EVENTS 1000
#define DEBUG 1


class Parsed_request_and_body;
class client;
class Server {
    private:
        Server();
        // int _Tcpsocketfd;
        int _domain;
        int _type;
        int _protocol;
        int _port;
        int _interface;
        std::vector < std::pair < std::string , std::string > > _interfaces;
        int _backlog;
        std::string _Recv_request; // request send lmajid
        std::vector < int > _Socketsfd; // save the fd's
        std::vector < sockaddr_in > _Addresses;
        //redone the request part
        // will create a class for each client to handle the request and response
        std::map < int , client > _Clients; // major client classes

    public:
        Server(int , int , int , int , u_int32_t , int, std::vector < std::pair < std::string , std::string  > > &);
        ~Server();
        int getsocketfd() const;
        int Filldata();
        int run();
        int getting_req( int kq, int cli_sock);
        std::string GetRequestToParse();
        void close_remove_event(int , int &);
        int handle_write_request(struct kevent&, int);
        void check_header_body(int, int);
        void register_read(int fd, int kq);
};

#endif