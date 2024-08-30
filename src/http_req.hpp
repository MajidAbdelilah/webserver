#include "server.hpp"
#include <fstream>
#include <sys/_types/_size_t.h>
#define http_hostname_macro "127.0.0.1:8080"
#define http_localhost_macro "localhost:8080"
#include <string>
#include <map>
#include "client.hpp"




// returns the exit status of the operation
int handle_request(client &client);

