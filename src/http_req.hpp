#include <fstream>
#include <sys/_types/_size_t.h>
#define http_hostname_macro "127.0.0.1:8080"
#define http_localhost_macro "localhost:8080"
#include <string>
#include <map>
class Parsed_request_and_body
{
	public:
	std::map<std::string, std::string> req_map;
	std::string body;
	size_t content_len;
	std::string type;
	// 1 if keep alive, 0 if not, -1 if header doesnt exist
	char keep_alive;
	Parsed_request_and_body(std::map<std::string, std::string> req_map, std::string body, size_t content_len, std::string type)
	{
		this->req_map = req_map;
		this->body = body;
		this->content_len = content_len;
		this->type = type;
	}
	Parsed_request_and_body(): req_map(), body(), content_len(0), type()
	{
	}
	~Parsed_request_and_body()
	{
	}	
};

class request_queue_element
{
	public:
	std::string request;
	long long written_len;
	std::map<std::string, std::string> req_header_map;
	std::ofstream file;

	request_queue_element(std::string request, long long written_len)
	{
		this->request = request;
		this->written_len = written_len;
	}
	
	request_queue_element(): request(), written_len(0), req_header_map()
	{
	}
	~request_queue_element()
	{
	}
	void add_to_request(std::string to_add)
	{
		this->request += to_add;
	}
	void write_to_file(std::string to_write)
	{
		this->file << to_write;
	}

};


// returns the exit status of the operation
int GET(Parsed_request_and_body &result);
int DELETE(Parsed_request_and_body &result);
int handle_request(Parsed_request_and_body &result);

