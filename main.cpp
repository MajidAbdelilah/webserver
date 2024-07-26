#include <iostream>

int main (int ac, char **av){
    if (ac != 2){
        std::cerr << "Usage : ./webserv [configuration file]" << std::endl;
        return (1);
    }

}