#include "http_req.hpp"
#include "server.hpp"
#include <fstream>
#include <string>



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

int DELETE(client &client_class, std::map<std::string, std::string> &req_map)
{
	std::string req = client_class.get_request();
	std::cout << "\n\n\nDELETE REQUEST START \n\n";
	std::cout << req << std::endl;
	std::cout << "\nDELETE REQUEST END\n";

	std::string line = get_line(req);
	std::cout << line << std::endl;
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
	while(uri.find("%20") != std::string::npos)
	{
		uri.replace(uri.find("%20"), 3, " ");
	}
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

	client_class.set_body("<html>"
					"<body>"
						"<h1>File deleted.</h1>"
					"</body>"
				  "</html>");
	client_class.set_content_length(client_class.get_body().size());
	std::cout << "content_len: " << client_class.get_body().size() << std::endl;
	client_class.set_content_type("text/html");

	return 200;
}


int POST_body();

int POST_header(client &client_class, std::map<std::string, std::string> &req_map)
{
	std::string req = client_class.get_request();
	std::cout << "\n\n\nPOST REQUEST START \n\n";
	std::cout << req << std::endl;
	std::cout << "\nPOST REQUEST END\n";

	std::string line = get_line(req);
	std::cout << line << std::endl;
	if (parse_initial_line(line, req_map) == -1)
	{
		std::cout << ("Error parsing initial line\n");
		return 400;
	}
	if(req_map["Method"] != "POST" || req_map.find("Method") == req_map.end())
	{
		std::cout << ("Method is not POST\n");
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

	while(line != "")
	{
		line = get_line(req);
		std::cout << line << std::endl;
		parse_headers(line, req_map);
	}

	if(req_map["Content-Type"].find("boundary=") == std::string::npos)
	{
		std::cout << "Boundary not found\n";
		return 400;
	}
	if(req_map["Content-Type"].find("multipart/form-data") == std::string::npos)
	{
		std::cout << "Content-Type is not multipart/form-data\n";
		return 400;
	}
	if(req_map.find("Content-Length") == req_map.end())
	{
		std::cout << "Content-Length not found\n";
		return 400;
	}
	client_class.set_post_boundary(req_map["Content-Type"].substr(req_map["Content-Type"].find("boundary=") + 9));
	std::cout << "Boundary: " << client_class.get_post_boundary() << std::endl;
	std::string uri = req_map["URI"][0] == '/' ? req_map["URI"].substr(1) : req_map["URI"];
	if(uri == "")
		uri = "index.html";
	uri = uri.substr(0, uri.find("?"));
	std::cout << "URI: " << uri << std::endl;
	client_class.set_post_filename(req_map[""]);
	client_class.set_post_filelength(std::stoll(req_map["Content-Length"]));
	std::cout << "Content-Length: " << client_class.get_post_filelength() << std::endl;
	client_class.set_post_fd(open(uri.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666));
	client_class.set_post_request_parsed(true);
	if(req.find("\r\n\r\n") == std::string::npos)
	{
		std::cout << "request doesnt have a body\n";
		return 400;
	}
	return 200;
}
int GET(client &client_class, std::map<std::string, std::string> &req_map)
{
	std::string req = client_class.get_request();
	std::cout << "\n\n\nGET REQUEST START \n\n";
	std::cout << req << std::endl;
	std::cout << "\nGET REQUEST END\n";

	std::string line = get_line(req);
	std::cout << line << std::endl;
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
	while(uri.find("%20") != std::string::npos)
	{
		uri.replace(uri.find("%20"), 3, " ");
	}
	std::cout << "URI: " << uri << std::endl;
	uri = uri.substr(0, uri.find("?"));
	client_class.set_filename(uri);

	// std::cout << content << std::endl;

	if (uri.find(".html") == uri.size() - 5)
		client_class.set_content_type("text/html");
	else if (uri.find(".css") == uri.size() - 4)
		client_class.set_content_type("text/css");
	else if (uri.find(".js") == uri.size() - 3)
		client_class.set_content_type("text/javascript");
	else if (uri.find(".jpg") == uri.size() - 4)
		client_class.set_content_type("image/jpeg");
	else if (uri.find(".png") == uri.size() - 4)
		client_class.set_content_type("image/png");
	else if (uri.find(".gif") == uri.size() - 4)
		client_class.set_content_type("image/gif");
	else if (uri.find(".ico") == uri.size() - 4)
		client_class.set_content_type("image/x-icon");
	else if(uri.find(".mp4") == uri.size() - 4)
		client_class.set_content_type("video/mp4");
	else
	{
		client_class.set_content_type("text/plain");
		// call smoumni cgi here
	}

	return 200;
}


int handle_request(client &client_class)
{
	if(client_class.get_request().size() == 0)
	{
		std::cout << ("No request to handle\n");
		return 500;
	}

	std::string req = client_class.get_request();
	std::cout << "\n\n\nREQUEST START \n\n";
	std::cout << req << std::endl;
	std::cout << "\nREQUEST END\n";
	if(req.find("\r\n\r\n") == std::string::npos)
	{
		std::cout << ("Request not complete\n");
		return 400;
	}
	std::string line;
	size_t pos = req.find("\r\n");
	if (pos != std::string::npos)
	{
		line = req.substr(0, pos);
	}
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
	if(req_map["Version"] != "HTTP/1.1")
	{
		std::cout << ("Version not supported\n");
		return 505;
	}
	if(req_map["Method"] == "GET")
	{
		std::cout << ("GET request found\n");
		int status =  GET(client_class, req_map);
		if(status == 200)
		{
			client_class.set_status_code(int(status));
			client_class.set_connection_close(req_map["Connection"] == "keep-alive" ? 0 : 1);
			client_class.set_method(req_map["Method"]);
			client_class.set_uri(req_map["URI"]);
			client_class.set_version(req_map["Version"]);
			client_class.set_host(req_map["Host"].substr(0, req_map["Host"].find(":")));
			client_class.set_port(req_map["Host"].substr(req_map["Host"].find(":") + 1));
			client_class.set_path(req_map["URI"].substr(0, req_map["URI"].find("?")));
		}
		client_class.set_requestvalid(bool(status == 200 || status == 404));
		return status;

	}
	else if(req_map["Method"] == "DELETE")
	{
		std::cout << ("DELETE request found\n");
		int status = DELETE(client_class, req_map);
		if(status == 200)
		{
			client_class.set_status_code(int(status));
			client_class.set_connection_close(req_map["Connection"] == "keep-alive" ? 0 : 1);
			client_class.set_method(req_map["Method"]);
			client_class.set_uri(req_map["URI"]);
			client_class.set_version(req_map["Version"]);
			client_class.set_host(req_map["Host"].substr(0, req_map["Host"].find(":")));
			client_class.set_port(req_map["Host"].substr(req_map["Host"].find(":") + 1));
			client_class.set_path(req_map["URI"].substr(0, req_map["URI"].find("?")));
		}
		client_class.set_requestvalid(bool(status == 200 || status == 404));
		return status;
	}
	else if (req_map["Method"] == "POST")
	{
		std::cout << ("POST request found\n");
		int status = POST_header(client_class, req_map);
		return status;
	}
	else
	{
		std::cout << ("Method not supported\n");
		return 405;
	}
	return 500;
}