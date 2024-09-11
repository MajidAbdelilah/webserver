#include "server.hpp"
#include <fstream>
#include <sys/_types/_size_t.h>
#define http_hostname_macro "127.0.0.1:8080\r\n"
#define http_localhost_macro "localhost:8080\r\n"
#include <string>
#include <map>
#include "client.hpp"




// returns the exit status of the operation
int handle_request(client &client);

std::string get_line(std::string &req);
int parse_initial_line(std::string line, std::map<std::string, std::string> &req);