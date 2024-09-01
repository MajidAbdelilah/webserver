#include <cstdlib>
#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <iostream>

#define NUM_CLIENTS 10
#define MAX_EVENTS 32
#define MAX_MSG_SIZE 256

int sk;

class client_data
{
	public:
	client_data();
	~client_data();
	int fd;
} clients[NUM_CLIENTS];



client_data::client_data()
{
	std::cout << "client_data()\n";
}
client_data::~client_data()
{
	std::cout << "~client_data()\n";
}

int get_conn(int fd) {
    for (int i = 0; i < NUM_CLIENTS; i++)
        if (clients[i].fd == fd)
            return i;
    return -1;
}

int conn_add(int fd) {
    if (fd < 1) return -1;
    int i = get_conn(0);
    if (i == -1) return -1;
    clients[i].fd = fd;
    return 0;
}

int conn_del(int fd) {
    if (fd < 1) return -1;
    int i = get_conn(fd);
    if (i == -1) return -1;
    clients[i].fd = 0;
    return close(fd);
}

int create_socket_and_listen() {
    struct addrinfo *addr;
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo("127.0.0.1", "8080", &hints, &addr);
	if(ret != 0)
	{
		std::cout << "getaddrinfo() failed!\n";
		std::cout << errno << "\n";
		return -1;
	}
    int local_s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if(local_s == -1)
	{
		std::cout << "socket() failed\n";
		std::cout << errno << "\n";
		return -1;
	}
    ret = bind(local_s, addr->ai_addr, addr->ai_addrlen);
	if(ret == -1)
	{
		std::cout << "bind() failed\n";
		std::cout << errno << "\n";
		return -1;
	}
    ret = listen(local_s, 5);
    if(ret == -1)
	{
		std::cout << "listen() failed\n";
		std::cout << errno << "\n";
		return -1;
	}
	return local_s;
}

void send_welcome_msg(int s) {
    std::string msg;
    msg = "welcome! you are client #" + std::to_string(get_conn(s)) + "!\n";
    send(s, msg.c_str(), msg.size(), 0);
}

int recv_msg(int s) {
    char buf[MAX_MSG_SIZE];
    int bytes_read = recv(s, buf, sizeof(buf) - 1, 0);
    buf[bytes_read] = 0;
	if(std::string(buf) == "kill\n")
	{
		return -1;
	}
		std::cout << "|" << buf << "|" << "\n";
		// return -1;
    std::cout << "client #" << get_conn(s) << " " << buf;
	return 0;
}

void run_event_loop(int kq, int local_s)
{

   	struct kevent evSet;
    struct kevent evList[MAX_EVENTS];
    struct sockaddr_storage addr;
    socklen_t socklen = sizeof(addr);

	while (1) {
		int num_events = kevent(kq, NULL, 0, evList, MAX_EVENTS, NULL);
		for (int i = 0; i < num_events; i++) {
			// receive new connection
			if (evList[i].ident == (unsigned long)local_s) {
				int fd = accept(evList[i].ident, (struct sockaddr *) &addr, &socklen);
				if (conn_add(fd) == 0) {
					EV_SET(&evSet, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
					kevent(kq, &evSet, 1, NULL, 0, NULL);
					send_welcome_msg(fd);
				} else {
					std::cout << "connection refused.\n";
					close(fd);
				}
			} // client disconnected
			else if (evList[i].flags & EV_EOF) {
				int fd = evList[i].ident;
				std::cout << "client #" <<  get_conn(fd) <<  " disconnected.\n";
				EV_SET(&evSet, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
				kevent(kq, &evSet, 1, NULL, 0, NULL);
				conn_del(fd);
			} // read message from client
			else if (evList[i].filter == EVFILT_READ) {
				if(recv_msg(evList[i].ident) == -1)
					return ;
			}
		}
	}
}

void d()
{
	std::cout << "here\n";
	// system("leaks a.out");
	close(sk);
	int i = 0;
	while(i < NUM_CLIENTS)
	{
		if(clients[i].fd)
			close(clients[i].fd);
		i++;
	}
}

int main(int argc, char *argv[]) {
	(void)argc;
	(void)argv;
	// int ret = std::atexit(d);
    // std::cout << ret << "\n";
	int local_s = create_socket_and_listen();
	if(local_s == -1)
	{
		std::cout << "create_socket_and_listen() failed!\n";
		return -1;
	}
	sk = local_s;
    int kq = kqueue();
    struct kevent evSet;
    EV_SET(&evSet, local_s, EVFILT_READ, EV_ADD, 0, 0, NULL);
    kevent(kq, &evSet, 1, NULL, 0, NULL);
    run_event_loop(kq, local_s);
    std::cout << "success\n";
    return EXIT_SUCCESS;
}
