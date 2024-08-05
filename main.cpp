#include <iostream>
#include "Server.hpp"

int main (int ac, char **av){
    (void)av;
    if (ac != 2){
        std::cerr << "Usage : ./webserv [configuration file]" << std::endl;
        return (1);
    }
    try{
        //get the info from config file
        // Server *webserv = new Server(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY, SOMAXCONN); /* for now i'll use custom ones */
        std::vector < std::pair < std::string , std::string  > >st;
        st.push_back(std::make_pair("127.0.0.1", "8080"));
        st.push_back(std::make_pair("10.13.8.14", "8080"));
        Server webserv(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY, SOMAXCONN, st);
        webserv.Filldata();
        webserv.run();
    }
    catch(const char * e){
        std::cout << e << std::endl;
    }
}