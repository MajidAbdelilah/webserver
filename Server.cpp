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

// handle signal for interupting server 

int Server::run(){
    signal(SIGINT, handle_inter);
    struct sockaddr_in hostAddr;
    char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: webserver-c\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>hello, world</html>\r\n";

    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(PORT); // uses default port
    hostAddr.sin_addr.s_addr = htonl(INADDR_ANY); // takes any address

    struct sockaddr_in client_sock; // client sockaddr_in
    int client_addrlen = sizeof(client_sock); //client sockaddr_in length

    if (bind(_Tcpsocketfd, (struct sockaddr *) &hostAddr, sizeof(hostAddr)) == -1)
        throw("Bind failure");
    if (listen(_Tcpsocketfd, SOMAXCONN) == -1)
        throw("listen failure");

    int serversize = sizeof(hostAddr);
    // handle client connections 
    while (1) {
        /* accept client connection to socket and creates a socket for the client.*/
        int clientfd = accept(_Tcpsocketfd, (struct sockaddr *) &hostAddr, (socklen_t *)&serversize);
        if (clientfd < 0)
            throw ("Clientfd error");
        std::cout << "connection accepted" << std::endl;
        
        // Getting the addr of the client , and the port used in the sockaddr_in struct 
        int _socknameclient = getsockname(clientfd, (struct sockaddr *)&client_sock, (socklen_t *)&client_addrlen );
        if (_socknameclient == -1){
            throw ("Clientname error");
        }
        std::cout  << "Client ip && Port [ " << inet_ntoa(client_sock.sin_addr) << " , " << ntohs(client_sock.sin_port) << " ]" << std::endl;

        std::vector <char> buf(BUFFER_SIZE);
        std::string _Fullbuffer;
        int _Readbuffer = recv(clientfd, &buf[0], BUFFER_SIZE, 0);
        if (_Readbuffer == -1)
            throw ("Recv error");
        for (int i = 0 ; (size_t)i < buf.size() ; i++)
            std::cout << buf[i];
        std::cout << std::endl;
        while (_Readbuffer){
            _Fullbuffer.append(buf.begin(), buf.end());
            buf.clear();
            _Readbuffer = recv(clientfd, &buf[0], BUFFER_SIZE, 0);
            if (_Readbuffer == -1)
                throw ("Recv error");
        }
        std::cout << _Fullbuffer << std::endl;
        // write(clientfd, resp , strlen(resp)); /* write a response containing Hello world */
        send(clientfd, resp, strlen(resp), 0);
        close(clientfd);
    }
}

Server::~Server(){
    std::cout << "Destructor called " << std::endl;
}