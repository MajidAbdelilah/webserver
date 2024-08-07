#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/event.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <regex>
class Server
{
public:
	Server(int port);
	~Server();
public:
	int port;
	int socket_listen_fd;
	int kq;
	struct kevent change_event[4];
	struct kevent event[4];
	struct sockaddr_in serv_addr;
	int new_events;
};

Server::Server(int port)
{
	this->port = port;
}

Server::~Server()
{
}


class Client
{
public:
	Client();
	~Client();
public:
	sockaddr_in client_addr;
	int socket_connection_fd;
};

Client::Client()
{
}
Client::~Client()
{
}
int main()
{
	Server server(8080);
	
    // Create socket.
    if (((server.socket_listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0))
    {
        std::cout << ("ERROR opening socket");
        exit(1);
    }

    // Create socket structure and bind to ip address.
    bzero((char *)&server.serv_addr, sizeof(server.serv_addr));
    server.serv_addr.sin_family = AF_INET;
    server.serv_addr.sin_addr.s_addr = INADDR_ANY;
    server.serv_addr.sin_port = htons(server.port);

    if (bind(server.socket_listen_fd, (struct sockaddr *)&server.serv_addr, sizeof(server.serv_addr)) < 0)
    {
        std::cout << ("Error binding socket");
        exit(1);
    }
	Client client;
    // Start listening.
    listen(server.socket_listen_fd, 3);
    int client_len = sizeof(client.client_addr);

    // Prepare the kqueue.
    server.kq = kqueue();

    // Create server.event 'filter', these are the events we want to monitor.
    // Here we want to monitor: server.socket_listen_fd, for the events: EVFILT_READ 
    // (when there is data to be read on the socket), and perform the following
    // actions on this kevent: EV_ADD and EV_ENABLE (add the server.event to the kqueue 
    // and enable it).
    EV_SET(server.change_event, server.socket_listen_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

    // Register kevent with the kqueue.
    if (kevent(server.kq, server.change_event, 1, NULL, 0, NULL) == -1)
    {
        std::cout << ("kevent");
        exit(1);
    }

    // Actual server.event loop.
    for (;;)
    {
        // Check for new events, but do not register new events with
        // the kqueue. Hence the 2nd and 3rd arguments are NULL, 0.
        // Only handle 1 new server.event per iteration in the loop; 5th
        // argument is 1.
        server.new_events = kevent(server.kq, NULL, 0, server.event, 1, NULL);
        if (server.new_events == -1)
        {
            std::cout << ("kevent");
            exit(1);
        }

        for (int i = 0; server.new_events > i; i++)
        {
            int event_fd = server.event[i].ident;

            // When the client disconnects an EOF is sent. By closing the file
            // descriptor the server.event is automatically removed from the kqueue.
            if (server.event[i].flags & EV_EOF)
            {
                std::cout << ("Client has disconnected");
                close(event_fd);
            }
            // If the new server.event's file descriptor is the same as the listening
            // socket's file descriptor, we are sure that a new client wants 
            // to connect to our socket.
            else if (event_fd == server.socket_listen_fd)
            {
                // Incoming socket connection on the listening socket.
                // Create a new socket for the actual connection to client.
                client.socket_connection_fd = accept(event_fd, (struct sockaddr *)&client.client_addr, (socklen_t *)&client_len);
                if (client.socket_connection_fd == -1)
                {
                    std::cout << ("Accept socket error");
                }

                // Put this new socket connection also as a 'filter' server.event
                // to watch in kqueue, so we can now watch for events on this
                // new socket.
                EV_SET(server.change_event, client.socket_connection_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                if (kevent(server.kq, server.change_event, 1, NULL, 0, NULL) < 0)
                {
                    std::cout << ("kevent error");
                }
            }

            else if (server.event[i].filter & EVFILT_READ)
            {
                // Read bytes from socket
                char buf[1024];
                size_t bytes_read = recv(event_fd, buf, sizeof(buf), 0);
				buf[bytes_read] = '\0';
                std::cout << "read " << bytes_read << " bytes\n";
				std::cout << buf << "\n";
				if(std::regex_match(buf, std::regex("GET / HTTP/1.1*"))){
					std::cout << "HTTP/1.1 200 OK\n";
					std::cout << "Content-Type: text/html\n\n";
					std::cout << "<html><body><h1>Hello, World!</h1></body></html>\n";
				}
				std::cout << std::regex_match(buf, std::regex("GET / HTTP/1.1*")) << "\n";
				if (bytes_read == 0)
				{
					close(event_fd);
				}
            }
        }
    }

    return 0;
}
