#include "server.hpp"
#include "http_req.hpp"
#include <iostream>
#include <string>
#include <sys/socket.h>
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
    std::cout << GRN "SERVER CREATED" WHT<< std::endl;
}

int Server::getsocketfd()const{
    return (1);
}

int Server::run(){
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, SIG_IGN);
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
                std::cout << GRN "Client connected fd number : " WHT << client_socketfd << '\n'; 
                // std::cout << "--------------------- Client socket fd: " << client_socketfd << " -------------------" <<'\n';
                EV_SET(&events[i], client_socketfd, EVFILT_READ, EV_ADD, 0, 0, NULL); //need to set the read event
                kevent(kernel_queue, &events[i], 1, NULL, 0, NULL);
                _Clients[client_socketfd] = client(client_socketfd);
            }
            else if (_Clients.find(events[i].ident) != _Clients.end()){
                 if (events[i].filter == EVFILT_READ){
                    std::cout << GRN "Reading request from client fd number : " << events[i].ident << WHT << '\n';
                    int r = getting_req(kernel_queue, events[i].ident); // parse request send to majid;
					(void)r;
                }
                else if (events[i].filter == EVFILT_WRITE)
                {
                    std::cout << "IT ENTERS WRITE FILTEEEEER \n";
                    Server::handle_write_request(events[i], kernel_queue);
                }
                if (events[i].flags & EV_EOF){
                    std::cout << RED "Client disconnected\n" WHT;
                    close_remove_event(events[i].ident, kernel_queue);
                }
            }
        }
    }
}

void Server::close_remove_event(int socket_fd, int &kqueue){
    struct kevent change;
    EV_SET(&change, socket_fd, EVFILT_READ , EV_DELETE,0, 0 , NULL);
    kevent(kqueue, &change, 1, NULL, 0, NULL);
    _Clients.erase(socket_fd);
    close(socket_fd);

}

int Server::handle_write_request(struct kevent &events, int kq) {
    int fd = events.ident;
    if (_Clients[fd].get_response_header() == ""){
        if ((_Clients[fd].get_filefd()) < 0){
            Server::register_read(fd, kq);
            return 0;
        }
        int buffer = 5000;
        char res[buffer + 1];
        int byes = read(_Clients[fd].get_filefd(), res, buffer);
        std::cout << "bytes read : " << byes << std::endl;
        if (byes < 0){
            perror("read");
            return (0);
        }
        if (byes == 0){
            _Clients[fd].set_ifstreamempty(1);
        }
        _Clients[fd].set_response(_Clients[fd].get_response() + std::string(res, byes));
    }
    
    int length = _Clients[fd].get_response().size();
    // std::cout << "-----------------RESPONSE TYPE LENGTH-------------------\n";
    // std::cout << _Clients[fd].get_content_type() << std::endl;
    // std::cout << _Clients[fd].get_content_length() << std::endl;
    // std::cout << _Clients[fd].get_response() << std::endl;
    // std::cout << "-----------------END-------------------\n";

    int size = send(fd, _Clients[fd].get_response().c_str(), length, 0);
    std::cout << "data sent : " << size << std::endl;
    if (size < 0){
        Server::register_read(fd, kq);
        Server::close_remove_event(fd, kq);
        return (1);
    }
    if (size >= 0) {
        if (_Clients[fd].get_response_header() != ""){
            if (_Clients[fd].get_response_header().size() == (unsigned long)size){
                _Clients[fd].set_response_header("");
                _Clients[fd].set_response(_Clients[fd].get_response().substr(size));
            }
            else {
                if (_Clients[fd].get_response_header().size() < (unsigned long)size){
                    _Clients[fd].set_response_header("");
                    size = size - _Clients[fd].get_response_header().size();
                    if ((unsigned long)size >= _Clients[fd].get_response().size())
                        _Clients[fd].set_response("");
                    else
                        _Clients[fd].set_response(_Clients[fd].get_response().substr(size));
                    return 0;
                }
                else if(_Clients[fd].get_response_header().size() > (unsigned long)size){
                    _Clients[fd].set_response(_Clients[fd].get_response().substr(size));
                    _Clients[fd].set_response_header(_Clients[fd].get_response_header().substr(size));
                    return 0;
                }
            }
        }
        else if (_Clients[fd].get_response_header() == "" && _Clients[fd].get_ifstreamempty()){
            std::cout << "checking if the file is empty && kevent is registred\n";
            int s = _Clients[fd].get_connection_close();
            _Clients[fd].clear_all();
            Server::register_read(fd, kq);
            if (s){
                std::cout << RED "Closed fd number : " << fd <<  WHT << '\n';
                close_remove_event(fd, kq);
            }
        }
        else{
            _Clients[fd].set_response(_Clients[fd].get_response().substr(size));
        }
    }
    return (0);
}

void Server::register_read(int fd, int kq){
    struct kevent change;
    EV_SET(&change, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    kevent(kq, &change, 1, NULL, 0 , NULL);
    EV_SET(&change, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    kevent(kq, &change, 1, NULL, 0 , NULL);
}

int Server::getting_req(int kernel_q, int client_soc){

    (void)kernel_q;
    char s[4096]={0};
    if (_Clients[client_soc].get_request().size() > 0){
        std::string tmp = _Clients[client_soc].get_request();
        std::string tmp2 = s;
        tmp2.append(tmp);
        _Clients[client_soc].set_request(tmp2);
    }

    int _bytesread = recv(client_soc, s, 4095, 0);

    if (_bytesread < 0){
        std::cout << "Recv returned -1 removing client fd number : " <<_Clients[client_soc].get_socketfd() << '\n';
        Server::close_remove_event(client_soc, kernel_q);
        return (-1);
    }
    else if (_bytesread == 0){
        std::cout << "received 0 bytes, closing connection\n";
        Server::close_remove_event(client_soc, kernel_q);
    }
    else {
        std::string a(s);
        std::cout << "Request received: " << a << std::endl;
        std::cout << _Clients[client_soc].get_socketfd() << std::endl;

        _Clients[client_soc].set_request(a);
        check_header_body(client_soc);
        if (_Clients[client_soc].is_request_done()){
            struct kevent changes;
            EV_SET(&changes, client_soc, EVFILT_READ, EV_DELETE, 0, 0, NULL);
            kevent(kernel_q, &changes, 1, NULL, 0 , NULL);
            EV_SET(&changes, client_soc, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
            kevent(kernel_q, &changes, 1, NULL, 0 , NULL);
			handle_request(_Clients[client_soc]);
            _Clients[client_soc].build_response();
        }
    }
    return (_bytesread);
}

void Server::check_header_body(int client_soc){
    if (!_Clients[client_soc].is_header_done()){
        std::string header = _Clients[client_soc].get_request();
        std::string tmp = header;
        unsigned long pos = tmp.find("\r\n\r\n");

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
            _Clients[client_soc].set_request_done(false);
            return ;
        }
    }
    if (_Clients[client_soc].is_header_done())
    {
		std::cout << "here\n";
        if (_Clients[client_soc].get_method() != ""){
            std::cout << "test\n";
            // reading body part of post request // majid post request/ body parsing and opening file and putting data in it 
        }
        std::string first_line = _Clients[client_soc].get_header();
        std::string method = first_line.substr(0, first_line.find(" "));
        if (method == "GET" || method == "DELETE"){
            _Clients[client_soc].set_request_done(true);
            if (_Clients[client_soc].get_body().size() > 0){
                char trash[10000] = {0};
                while (recv(client_soc, trash, 9999, 0) > 0);
            }
        }
        std::cout << "method is : " << method << std::endl;
        
        _Clients[client_soc].set_request(_Clients[client_soc].get_header());
        _Clients[client_soc].set_method(method);
        if (method != "POST" || method != "GET" || method != "DELETE"){
            _Clients[client_soc].set_status_code(405);
            _Clients[client_soc].set_request_done(true);
        }
        return ;
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