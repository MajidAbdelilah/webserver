#include "Server.hpp"

Server::Server(int domain, int type, int protocol, int port, u_int32_t interface, int backlog, std::vector < std::pair < std::string , std::string > > &addresses){
    this->_domain = domain;
    this->_type = type;
    this->_interfaces = addresses;
    this->_protocol = protocol;
    this->_port = port; 
    this->_interface = interface;
    this->_backlog = backlog;
    std::cout << "Server constructor called " << std::endl;
}

int Server::getsocketfd()const{
    return (1);
}

// handle signal for interupting server 

int Server::run(){
    std::vector < char > buffer(BUFFER_SIZE);

    char resp[] = "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: 39\r\n"
                    "Connection: keep-alive\r\n\r\n"
                    "<html><body><h1>wa ras l9lwa</h1></body></html>";

    int kernel_queue = kqueue();
    if (kernel_queue < 0)
        throw("Kq failure");

    struct kevent Server_k;
    for(size_t i = 0 ; i < _Socketsfd.size(); i++){
        EV_SET(&Server_k, _Socketsfd[i], EVFILT_READ, EV_ADD ,0 ,0 ,NULL);
        if(kevent(kernel_queue, &Server_k, 1, NULL, 0 , NULL) < 0)
            throw("kevent error");
    }

    while (1){
        struct kevent events[MAX_EVENTS];
        int count = kevent(kernel_queue, NULL, 0, events, MAX_EVENTS, NULL);
        for (int i = 0 ; i < count; i++){
            if (std::find(_Socketsfd.begin(), _Socketsfd.end(), events[i].ident) != _Socketsfd.end()){
                int sizeHost = sizeof(struct sockaddr_in);
                int client_socketfd = accept(events[i].ident, (struct sockaddr*)&_Addresses[i], (socklen_t*)&sizeHost);
                if (client_socketfd < 0)
                    throw("Client fd accept error"); // accepting client connection
                struct sockaddr_in clin;
                int sizecli = sizeof(clin);
                getsockname(client_socketfd, (struct sockaddr *)&clin, (socklen_t *)&sizecli);
                std::cout << "Client address and port : " << inet_ntoa(clin.sin_addr) 
                    << " " << ntohs(clin.sin_port) << std::endl;
                EV_SET(&events[i], client_socketfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kernel_queue, &events[i], 1, NULL, 0, NULL);
                // close (client_socketfd);
            }
            else{
                getting_req(events, kernel_queue, events[i].ident); // parse request send to majid;
                send(events[i].ident, resp ,strlen(resp),0);
                // close(events[i].ident);
            }
        }
    }
}

int Server::getting_req(struct kevent events[MAX_EVENTS], int kernel_q, int client_soc){
    (void)events;
    (void)kernel_q;
    std::vector < char > buffer(BUFFER_SIZE);
    ssize_t _bytesread = recv(client_soc, &buffer[0], BUFFER_SIZE, 0);
    if (_bytesread < 0)
        throw ("recv error");
    if (_bytesread == 0){
        close (client_soc);
        return (1);
    }
    for (size_t i = 0 ; i < buffer.size(); i++)
        std::cout << buffer[i];
    std::cout << std::endl;
    std::string _Recv_request(buffer.begin(), buffer.end());
    // call majid's request implementation 
    return (0);
}

std::string Server::GetRequestToParse() {
    return (_Recv_request);
}

int Server::Filldata(){
    for (size_t i = 0 ; i <_interfaces.size(); i++){
        struct addrinfo hints;
        struct addrinfo *res;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = _domain;
        hints.ai_socktype = _type;
        // getting the ip in res struct;
        int stats = getaddrinfo(_interfaces[i].first.c_str(), NULL, &hints, &res);
        if (stats != 0){
            throw(gai_strerror(stats));
        }

        int _Tcpsocketfd;
        _Tcpsocketfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (_Tcpsocketfd == -1){
            throw("Socket creation failed");
        }

        struct sockaddr_in *addr = (sockaddr_in *)res->ai_addr;
        addr->sin_port = htons(_port);

        int opt = 1;
        if (setsockopt(_Tcpsocketfd, SOL_SOCKET, SO_REUSEADDR, &opt, res->ai_addrlen) < 0){
            freeaddrinfo(res);
            throw("Setsockopt error");
        }

        if (bind(_Tcpsocketfd, (struct sockaddr *) addr, sizeof(*addr)) < 0){
            freeaddrinfo(res);
            throw("Bind failure");
        }
        if (listen(_Tcpsocketfd, this->_backlog) <  0){
            freeaddrinfo(res);
            throw("listen failure");
        }
        _Socketsfd.push_back(_Tcpsocketfd);
        _Addresses.push_back(*addr);
        freeaddrinfo(res);
    }
    return (0);
}

Server::~Server(){
    std::cout << "Destructor called " << std::endl;
}