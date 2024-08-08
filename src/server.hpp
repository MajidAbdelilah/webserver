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

#define PORT 8080
#define BUFFER_SIZE 30000
#define MAX_EVENTS 10000

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
        struct sockaddr_in hostaddr; //convert into an array of struct 
        static std::string _Recv_request; // request send lmajid
        std::vector < int > _Socketsfd; // save the fd's
        std::vector < sockaddr_in > _Addresses;

    public:
        Server(int , int , int , int , u_int32_t , int, std::vector < std::pair < std::string , std::string  > > &);
        ~Server();
        int getsocketfd() const;
        int Filldata();
        int run();
        int getting_req(struct kevent* , int kq, int cli_sock);
        static std::string GetRequestToParse();
		void close_remove_event(int &socket_fd, int &kqueue);

};

#endif