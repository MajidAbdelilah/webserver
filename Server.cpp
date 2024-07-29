#include "Server.hpp"

Server::Server(){
    _Tcpsocketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_Tcpsocketfd == -1){
        throw("Socket creation failed");
    }
    std::cout << "Server default constructor called With socket creation" << std::endl;
}

int Server::getsocketfd()const{
    return (_Tcpsocketfd);
}

int Server::run(){
    struct sockaddr_in hostAddr;
    char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>hello, world</html>\r\n";
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(PORT); // uses default port
    hostAddr.sin_addr.s_addr = htonl(INADDR_ANY); // takes any address

    if (bind(_Tcpsocketfd, (struct sockaddr *) &hostAddr, sizeof(hostAddr)) == -1)
        throw("Bind failure");
    if (listen(_Tcpsocketfd, 10) == -1)
        throw("listen failure");

    int serversize = sizeof(hostAddr);
    // handle client connections 
    while (1){
        int clientfd = accept(_Tcpsocketfd, (struct sockaddr *) &hostAddr, (socklen_t *)&serversize);
        if (clientfd < 0)
            throw ("Clientfd error");
        std::cout << "connection accepted" << std::endl;
        write(clientfd, resp , strlen(resp));
    }
}

Server::~Server(){
    std::cout << "Destructor called " << std::endl;
}