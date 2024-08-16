#include "server.hpp"
#include "http_req.hpp"
#include <iostream>
#include <string>
#include <utility>
#include "client.hpp"

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
    int kernel_queue = kqueue();
    if (kernel_queue < 0)
        throw("Kq failure");

    struct kevent Server_k;
    for(size_t i = 0 ; i < _Socketsfd.size(); i++){
        EV_SET(&Server_k, _Socketsfd[i], EVFILT_READ, EV_ADD ,0 ,0 ,NULL);
        if(kevent(kernel_queue, &Server_k, 1, NULL, 0 , NULL) < 0)
            throw("kevent creation error");
    }
    struct timespec timeout;
    timeout.tv_nsec = 400000000;
    int count =0;
    while (1){
        struct kevent events[MAX_EVENTS];
        count = kevent(kernel_queue, NULL, 0, events, MAX_EVENTS, NULL);
        if (count == -1)
            throw("kevent error");
        for (int i = 0 ; i < count; i++){
            if (std::find(_Socketsfd.begin(), _Socketsfd.end(), events[i].ident) != _Socketsfd.end()){
                int client_socketfd = accept(events[i].ident, NULL,0);
                if (client_socketfd < 0)
                    throw("Client fd accept error"); // accepting client connection
                if(-1 == fcntl(client_socketfd, F_SETFL, O_NONBLOCK))
                    throw("error fcntl client socket");
                std::cout << "--------------------- Client socket fd: " << client_socketfd << " -------------------" <<'\n';
                EV_SET(&events[i], client_socketfd, EVFILT_READ, EV_ADD, 0, 0, NULL); //need to set the read event
                kevent(kernel_queue, &events[i], 1, NULL, 0, NULL);
                _Clients[client_socketfd] = client(client_socketfd);
            }
            else if (std::find(_Clients.begin(), _Clients.end(), events[i].ident) != _Clients.end()){
                if (events[i].filter & EVFILT_READ){
                    int r = getting_req(kernel_queue, events[i].ident); // parse request send to majid;
					(void)r;
                    // TODO ! method to check the request part in each socket fd 
                }
                if (events[i].filter & EVFILT_WRITE)
                {
                    std::cout << "IT ENTERS WRITE FILTEEEEER \n";
                    Server::handle_write_request(events[i], kernel_queue);
                    // TODO ! the socket is ready to be written on
                }

            }
        }
    }
}

void Server::close_remove_event(int socket_fd, int &kqueue){
    struct kevent change;
    EV_SET(&change, socket_fd, EVFILT_WRITE | EVFILT_READ , EV_DELETE,0, 0 , NULL);
    kevent(kqueue, &change, 1, NULL, 0, NULL);
    _Sockets_req.erase(socket_fd); // removing the client socket
    _Clients.erase(socket_fd);
    close(socket_fd);
}

int Server::handle_write_request(struct kevent &events, int kq) {
    long long length = socket_response[events.ident].content_len;
    std::string resp = socket_response[events.ident].body;
    int size = send(events.ident, resp.c_str(), length, 0);

    if (size < 0)
        return (1);

    struct kevent change;
    if (size >= length){ //whole body has been sent
        EV_SET(&change, events.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        kevent(kq, &change, 1, NULL, 0 , NULL);
        EV_SET(&change, events.ident, EVFILT_READ, EV_ADD, 0, 0, NULL);
        kevent(kq, &change, 1, NULL, 0 , NULL);
        socket_response.erase(events.ident);
    }
    else{
        socket_response[events.ident].body = resp.substr(size);
    }
    return (0);
}

int Server::getting_req(int kernel_q, int client_soc){

    (void)kernel_q;
    char s[4096]={0};

    int _bytesread = recv(client_soc, s, 4095, 0);

    if (_bytesread < 0){
        std::cout << "bytesread < 0 either no more data , or err in recv\n";
    }
    else if (_bytesread == 0){
        std::cout << "received 0 bytes, closing connection\n";
        Server::close_remove_event(client_soc, kernel_q);
    }
    else{
        _Clients[client_soc].set_request(s);
        // _Client_header_body[client_soc].second.append(s, _bytesread);
        check_header_body(client_soc);
        if (request_done){
            std::string request = _Client_header_body[client_soc].first + _Client_header_body[client_soc].second;
            _Client_full_request[client_soc] = std::make_pair(request.size(), request);
            struct kevent change;
            EV_SET(&change, client_soc, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
            kevent(kernel_q, &change, 1, NULL, 0, NULL);
            request_done = false;
            header_done = false;
            body_done = false;
        }

    }
    return (_bytesread);
}

void Server::check_header_body(int client_soc){
    if (!_Clients[client_soc].is_header_done()){
        std::string header = _Clients[client_soc].get_request();
        std::string tmp = header;
        int pos = tmp.find("\r\n\r\n");
        if (pos != std::string::npos){
            _Clients[client_soc].set_header_done(true);
            std::string head = tmp.substr(0, pos + 4);
            _Clients[client_soc].set_header(head);
            _Clients[client_soc].set_body(tmp.substr(pos + 4));
            _Clients[client_soc].clear_request();
        }
        else {
            _Clients[client_soc].set_header_done(false);
            _Clients[client_soc].set_body_done(false);
            return ;
        }
    }
    if (_Clients[client_soc].is_header_done())
    {
        std::string method = _Client_header_body[client_soc].first.substr(0, _Client_header_body[client_soc].first.find(" "));
        if (method == "GET" || method == "DELETE"){
            request_done = true;
            return ;
        }
        if (method == "POST"){
            std::string &body = _Client_header_body[client_soc].second;
            std::string tmp = body;
            int pos = tmp.find("\r\n\r\n");
            if (pos != std::string::npos){
                body_done = true;
                std::string body = tmp.substr(0, pos + 4);
                _Client_header_body[client_soc].second = body;
                _Recv_request = _Client_header_body[client_soc].first + _Client_header_body[client_soc].second;
            }
            else{
                body_done = false;
            }
        }
        else{
            request_done = true;
        }
    }
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

        if (fcntl(_Tcpsocketfd, F_SETFL, O_NONBLOCK) == -1)
            throw("Fcntl error");

        if (bind(_Tcpsocketfd, (struct sockaddr *) addr, sizeof(*addr)) < 0){
            freeaddrinfo(res);
            throw("Bind failure");
        }
        if (listen(_Tcpsocketfd, SOMAXCONN) <  0){
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