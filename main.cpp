#include <iostream>
#include "Server.hpp"

int main (int ac, char **av){
    (void)av;
    if (ac != 2){
        std::cerr << "Usage : ./webserv [configuration file]" << std::endl;
        return (1);
    }
    try{
        Server *webserv = new Server();
        webserv->run();
        delete webserv;
    }
    catch(const char * e){
        std::cout << e << std::endl;
    }
}