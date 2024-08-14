#include "server.hpp"
#include "http_req.hpp"
#include <string>

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
                _Clien_sock.push_back(client_socketfd);
                EV_SET(&events[i], client_socketfd, EVFILT_READ, EV_ADD, 0, 0, NULL); //need to set the read event
                kevent(kernel_queue, &events[i], 1, NULL, 0, NULL);
            }
            else if (std::find(_Clien_sock.begin(), _Clien_sock.end(), events[i].ident) != _Clien_sock.end()){
                if (events[i].filter & EVFILT_READ){
                    int r = getting_req(kernel_queue, events[i].ident); // parse request send to majid;
					(void)r;
                    // TODO ! method to check the request part in each socket fd 
                }
                else if (events[i].filter & EVFILT_WRITE)
                {
                    std::cout << "Test\n";
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
    close(socket_fd);
}

int Server::getting_req(int kernel_q, int client_soc){

    (void)kernel_q;
    char s[600]={0};

    int _bytesread = -1;

    if ((_bytesread = recv(client_soc, s, 599, 0)) > 0){
        _Sockets_req[client_soc].append(s, _bytesread);
        // std::cout << _Sockets_req[client_soc] << '\n';
        // check if the map has the client socket fd with string
        if(_Sockets_req[client_soc].rfind("\r\n\r\n") != std::string::npos){
          Parsed_request_and_body result;
		   int status  = handle_request(result, _Sockets_req);
		   std::string resp = "HTTP/1.1 "+std::to_string(status)+"\r\n"
                "Content-Type: "+result.type+"\r\n"
                "Content-Length: "+std::to_string(result.content_len)+"\r\n"
                "Connection: keep-alive\r\n\r\n"
                +result.body;
				std::cout << "status: " << status << std::endl;
			send(client_soc, resp.c_str(), resp.size(), 0);
 	      	if(status != -100)
		   		Server::close_remove_event(client_soc, kernel_q);
		}
    }
    std::cout << "number of read bytes : " << _bytesread << '\n';
    if (_bytesread == 0){ // connection closed
        std::cout << "---------------------closing "<< client_soc<< " ---------------------\n";
        Server::close_remove_event(client_soc, kernel_q);
        return (0);
    }

    if (_bytesread < 0){ // failed recv could mean no more data no read;
        std::cout << "bytesread < 0 either no more data , or err in recv\n";
        // Server::close_remove_event(client_soc, kernel_q);
        return (_bytesread);
    }

    return (_bytesread);
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