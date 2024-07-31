#include "Server.hpp"

Server::Server(int domain, int type, int protocol, int port, u_int32_t interface, int backlog){
    this->_domain = domain;
    this->_type = type;
    this->_protocol = protocol;
    this->_port = port; 
    this->_interface = interface;
    this->_backlog = backlog;
    std::cout << "Server constructor called " << std::endl;
}

int Server::getsocketfd()const{
    return (_Tcpsocketfd);
}

// handle signal for interupting server 

int Server::run(){
    std::vector < char > buffer(BUFFER_SIZE);
    int sizesocket = sizeof(hostaddr);
    char resp[] = "HTTP/1.0 200 OK\r\n"
                "Server: webserver-c\r\n"
                "Content-type: text/html\r\n\r\n"
                "<html>hello, world. This is webserver</html>\r\n";
    int client_socket;
    // while (1){
    //     std::cout << "** Waiting for connection **\n";
    //     client_socket = accept(_Tcpsocketfd, (struct sockaddr *)&hostaddr, (socklen_t *)&sizesocket);
    //     if (client_socket < 0)
    //         throw ("Accept failed ");
    //     std::cout << "** Connected **\n";
    //     read(client_socket , &buffer[0], BUFFER_SIZE);
    //     for (int i = 0 ; (size_t)i  < buffer.size(); i++)
    //         std::cout << buffer[i];
    //     std::cout << std::endl;
    //     write(client_socket , resp , strlen(resp));
    //     close(client_socket);
    // }
    int kernel_queue = kqueue();
    if (kernel_queue < 0)
        throw("Kq failure");

    struct kevent Server_k;
    EV_SET(&Server_k, _Tcpsocketfd, EVFILT_READ, EV_ADD ,0 ,0 ,NULL);
    if(kevent(kernel_queue, &Server_k, 1, NULL, 0 , NULL) < 0)
        throw("kevent error");

    while (1){
        struct kevent events[MAX_EVENTS];
        int count = kevent(kernel_queue, NULL, 0, events, MAX_EVENTS, NULL);
        for (int i = 0 ; i < count; i++){
            if (events[i].ident == _Tcpsocketfd){
                int sizeHost = sizeof(hostaddr);
                int client_socketfd = accept(_Tcpsocketfd, (struct sockaddr *)&hostaddr, (socklen_t*)&sizeHost); 
                if (client_socketfd < 0)
                    throw("Client fd accept error"); // accepting client connection
                struct sockaddr_in clin;
                int sizecli = sizeof(clin);
                getsockname(client_socketfd, (struct sockaddr *)&clin, (socklen_t *)&sizecli);
                std::cout << "Client address and port : " << inet_ntoa(clin.sin_addr) 
                    << " " << ntohs(clin.sin_port) << std::endl;
                getting_req(events, kernel_queue, client_socket);
            }
        }
    }
}

int Server::getting_req(struct kevent events[MAX_EVENTS], int kernal_q, int client_soc){
    std::vector < char > buffer(BUFFER_SIZE);
    ssize_t _bytesread = recv(client_soc, &buffer[0], BUFFER_SIZE, 0); // continue the req part
    if (_bytesread < 0)
        throw ("recv error");
        for (size_t i = 0 ; i < buffer.size(); i++)
            std::cout << buffer[i];
    std::cout << std::endl;
}

int Server::Filldata(){
    _Tcpsocketfd = socket(_domain, _type, _protocol);
    if (_Tcpsocketfd == -1){
        throw("Socket creation failed");
    }

    hostaddr.sin_family = _domain; // uses ipv4
    hostaddr.sin_port = htons(_port); // uses default port "netwrk is big endian"
    hostaddr.sin_addr.s_addr = htonl(_interface); // takes any address

    int opt = 1;
    setsockopt(_Tcpsocketfd, SOL_SOCKET, SO_REUSEADDR, &opt, (socklen_t )sizeof(hostaddr));

    if (bind(_Tcpsocketfd, (struct sockaddr *) &hostaddr, sizeof(hostaddr)) < 0)
        throw("Bind failure");
    if (listen(_Tcpsocketfd, this->_backlog) <  0)
        throw("listen failure");
    return (0);
}

Server::~Server(){
    std::cout << "Destructor called " << std::endl;
}