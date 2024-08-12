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

int GET(Parsed_request_and_body &result)
{
	std::string req ;//= Server::GetRequestToParse();
	std::cout << "\n\n\nGET REQUEST START \n\n";
	std::cout << req << std::endl;
	std::cout << "\nGET REQUEST END\n";

	std::string line = get_line(req);
	std::cout << line << std::endl;
	std::map<std::string, std::string> req_map;
	if (parse_initial_line(line, req_map) == -1)
	{
		std::cout << ("Error parsing initial line\n");
		return 400;
	}	
	if(req_map["Method"] != "GET" || req_map.find("Method") == req_map.end())
	{
		std::cout << ("Method is not GET\n");
		return 400;
	}	
	if(req_map.find("URI") == req_map.end())
	{
		std::cout << ("URI not found\n");
		return 400;
	}	
	if(req_map["Version"] != "HTTP/1.1" || req_map.find("Version") == req_map.end())
	{
		std::cout << ("Version is not HTTP/1.1\n");
		return 400;
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
		return 400;
	}	
	if(req_map["Host"] != http_hostname_macro && req_map["Host"] != http_localhost_macro)
	{
		std::cout << ("Host header is not correct\n");
		return 400;
	}	
	
	if(req_map["URI"] == "/")
		req_map["URI"] = "/index.html";

	std::string uri = req_map["URI"][0] == '/' ? req_map["URI"].substr(1) : req_map["URI"];
	std::cout << "URI: " << uri << std::endl;

	std::ifstream file(uri);
	if (!file.is_open())
	{
		std::cout << ("File not found\n");
		return 404;
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	std::cout << content << std::endl;
	result.body = content;
	result.content_len = content.size();
	std::cout << "content_len: " << content.size() << std::endl;

	if (uri.find(".html") == uri.size() - 5)
		result.type = "text/html";
	else if (uri.find(".css") == uri.size() - 4)
		result.type = "text/css";
	else if (uri.find(".js") == uri.size() - 3)
		result.type = "text/javascript";
	else if (uri.find(".jpg") == uri.size() - 4)
		result.type = "image/jpeg";
	else if (uri.find(".png") == uri.size() - 4)
		result.type = "image/png";
	else if (uri.find(".gif") == uri.size() - 4)
		result.type = "image/gif";
	else if (uri.find(".ico") == uri.size() - 4)
		result.type = "image/x-icon";
	else
	{
		result.type = "text/plain";
		// call smoumni cgi here
	}

	return 200;
}

int DELETE(Parsed_request_and_body &result)
{
	std::string req ;//= Server::GetRequestToParse();
	std::cout << "\n\n\nDELETE REQUEST START \n\n";
	std::cout << req << std::endl;
	std::cout << "\nDELETE REQUEST END\n";

	std::string line = get_line(req);
	std::cout << line << std::endl;
	std::map<std::string, std::string> req_map;
	if (parse_initial_line(line, req_map) == -1)
	{
		std::cout << ("Error parsing initial line\n");
		return 400;
	}	
	if(req_map["Method"] != "DELETE" || req_map.find("Method") == req_map.end())
	{
		std::cout << ("Method is not DELETE\n");
		return 400;
	}	
	if(req_map.find("URI") == req_map.end())
	{
		std::cout << ("URI not found\n");
		return 400;
	}	
	if(req_map["Version"] != "HTTP/1.1" || req_map.find("Version") == req_map.end())
	{
		std::cout << ("Version is not HTTP/1.1\n");
		return 400;
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
		return 400;
	}	
	if(req_map["Host"] != http_hostname_macro && req_map["Host"] != http_localhost_macro)
	{
		std::cout << ("Host header is not correct\n");
		return 400;
	}	
	
	if(req_map["URI"] == "/")
		req_map["URI"] = "/index.html";

	std::string uri = req_map["URI"][0] == '/' ? req_map["URI"].substr(1) : req_map["URI"];
	std::cout << "URI: " << uri << std::endl;


    
	int status = std::remove(uri.c_str());

    // Check if the file has been successfully removed
    if (status != 0) {
        std::cout << "Error deleting file\n";
		return 404;
    }
    else {
        std::cout << "File successfully deleted\n";
    }

	result.body = "<html>"
					"<body>"
						"<h1>File deleted.</h1>"
					"</body>"
				  "</html>";
	result.content_len = result.body.size();
	std::cout << "content_len: " << result.body.size() << std::endl;
	result.type = "text/html";
	result.req_map = req_map;

	return 200;
}


int handle_request(Parsed_request_and_body &result, std::map <int, std::string> &Sockets_req)
{
	std::string req ;//= Server::GetRequestToParse();
	std::cout << "\n\n\nREQUEST START \n\n";
	std::cout << req << std::endl;
	std::cout << "\nREQUEST END\n";

	std::string line = get_line(req);
	std::cout << line << std::endl;
	std::map<std::string, std::string> req_map;
	if (parse_initial_line(line, req_map) == -1)
	{
		std::cout << ("Error parsing initial line\n");
		return 400;
	}	
	if(req_map.find("Method") == req_map.end())
	{
		std::cout << ("Method not found\n");
		return 400;
	}	
	if(req_map.find("URI") == req_map.end())
	{
		std::cout << ("URI not found\n");
		return 400;
	}	
	if(req_map.find("Version") == req_map.end())
	{
		std::cout << ("Version not found\n");
		return 400;
	}	

	if(req_map["Method"] == "GET")
		return GET(result);
	else if(req_map["Method"] == "DELETE")
		return DELETE(result);
	else
	{
		std::cout << ("Method not supported\n");
		return 405;
	}
	return 500;
}