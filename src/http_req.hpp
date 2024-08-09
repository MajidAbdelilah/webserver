#include <sys/_types/_size_t.h>
#define http_host_macro "127.0.0.1:8080"
#include <string>
#include <map>
class Parsed_request_and_body
{
	public:
	std::map<std::string, std::string> req_map;
	std::string body;
	size_t content_len;
	std::string type;
	Parsed_request_and_body(std::map<std::string, std::string> req_map, std::string body, size_t content_len, std::string type)
	{
		this->req_map = req_map;
		this->body = body;
		this->content_len = content_len;
		this->type = type;
	}
	Parsed_request_and_body()
	{
	}
	~Parsed_request_and_body()
	{
	}	
};



// returns the exit status of the operation
int GET(Parsed_request_and_body &result);