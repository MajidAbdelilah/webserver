#include "http_req.hpp"
#include "server.hpp"
#include <fstream>

std::string get_line(std::string &req)
{
	std::string line;
	size_t pos = req.find("\r\n");
	if (pos != std::string::npos)
	{
		line = req.substr(0, pos);
		req.erase(0, pos + 2);
	}
	return line;
}

int parse_initial_line(std::string line, std::map<std::string, std::string> &req)
{
	size_t pos = line.find(" ");
	if (pos == std::string::npos)
		return -1;
	req["Method"] = line.substr(0, pos);
	line.erase(0, pos + 1);
	pos = line.find(" ");
	if (pos == std::string::npos)
		return -1;
	req["URI"] = line.substr(0, pos);
	line.erase(0, pos + 1);
	req["Version"] = line;
	return 0;
}

int parse_headers(std::string line, std::map<std::string, std::string> &req)
{
	size_t pos = line.find(":");
	if (pos == std::string::npos)
		return -1;
	req[line.substr(0, pos)] = line.substr(pos + 2);
	return 0;
}

int GET(std::string &file_content)
{
	std::string req = Server::GetRequestToParse();
	std::cout << "\n\n\nGET REQUEST START \n\n";
	std::cout << req << std::endl;
	std::cout << "\nGET REQUEST END\n";

	std::string line = get_line(req);
	std::cout << line << std::endl;
	std::map<std::string, std::string> req_map;
	if (parse_initial_line(line, req_map) == -1)
	{
		std::cout << ("Error parsing initial line\n");
		return -1;
	}	
	if(req_map["Method"] != "GET" || req_map.find("Method") == req_map.end())
	{
		std::cout << ("Method is not GET\n");
		return -1;
	}	
	if(req_map.find("URI") == req_map.end())
	{
		std::cout << ("URI not found\n");
		return -1;
	}	
	if(req_map["Version"] != "HTTP/1.1" || req_map.find("Version") == req_map.end())
	{
		std::cout << ("Version is not HTTP/1.1\n");
		return -1;
	}	

	while (line != "")
	{
		line = get_line(req);
		std::cout << line << std::endl;
		parse_headers(line, req_map);
	}

	if(req_map.find("Host") == req_map.end())
	{
		std::cout << ("Host header not found\n");
		return -1;
	}	
	if(req_map["Host"] != http_host_macro)
	{
		std::cout << ("Host header is not correct\n");
		return -1;
	}	
	
	if(req_map["URI"] == "/")
		req_map["URI"] = "/index.html";

	std::string uri = req_map["URI"][0] == '/' ? req_map["URI"].substr(1) : req_map["URI"];
	std::cout << "URI: " << uri << std::endl;

	std::ifstream file(uri);
	if (!file.is_open())
	{
		std::cout << ("File not found\n");
		return -1;
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::cout << content << std::endl;
	file_content = content;
	std::cout << "content_len: " << content.length() << std::endl;
	return 0;
}