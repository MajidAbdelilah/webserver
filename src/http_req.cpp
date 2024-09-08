#include "http_req.hpp"
#include "server.hpp"
#include <cctype>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <sys/fcntl.h>
#include <unistd.h>


void fill_client_data(client &client_class, std::map<std::string, std::string> &req_map){
	// client_class.set_method(req_map["URI"]);
	client_class.set_version(req_map["Version"].substr(0, req_map["Version"].size() - 2));
	client_class.set_connection_close(req_map["Connection"].find("keep-alive") != std::string::npos ? 0 : 1);
	client_class.set_content_type(req_map["Content-Type"].substr(0, req_map["Content-Type"].find(";")));

}
std::string get_line(std::string &req)
{
	std::string line;
	size_t pos = req.find("\r\n");
	if (pos != std::string::npos)
	{
		line = req.substr(0, pos + 2);
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
	int i = 1;
	while(std::isspace(line[pos+i]))i++;
	req[line.substr(0, pos)] = line.substr(pos + i);
	return 0;
}

int DELETE(client &client_class, std::map<std::string, std::string> &req_map)
{
	std::string req = client_class.get_request();
	DEBUG && std::cout << "\n\n\nDELETE REQUEST START \n\n";
	// DEBUG && std::cout << req << std::endl;
	DEBUG && std::cout << "\nDELETE REQUEST END\n";

	std::string line = get_line(req);
	DEBUG && std::cout << line << std::endl;
	if (parse_initial_line(line, req_map) == -1)
	{
		DEBUG && std::cout << ("Error parsing initial line\n");
		return 400;
	}	
	if(req_map["Method"] != "DELETE" || req_map.find("Method") == req_map.end())
	{
		DEBUG && std::cout << ("Method is not DELETE\n");
		return 400;
	}	
	if(req_map.find("URI") == req_map.end())
	{
		DEBUG && std::cout << ("URI not found\n");
		return 400;
	}	
	if(req_map.find("Version") == req_map.end() || req_map["Version"] != "HTTP/1.1\r\n")
	{
		DEBUG && std::cout << ("Version is not HTTP/1.1\n");
		return 400;
	}	

	while (line != "\r\n")
	{
		line = get_line(req);
		DEBUG && std::cout << line << std::endl;
		parse_headers(line, req_map);
	}

	if(req_map.find("Host") == req_map.end())
	{
		DEBUG && std::cout << ("Host header not found\n");
		return 400;
	}	
	if(req_map["Host"] != http_hostname_macro && req_map["Host"] != http_localhost_macro)
	{
		DEBUG && std::cout << ("Host header is not correct\n");
		return 400;
	}	
	
	if(req_map["URI"] == "/")
		req_map["URI"] = "/index.html";

	std::string uri = req_map["URI"][0] == '/' ? req_map["URI"].substr(1) : req_map["URI"];
	while(uri.find("%20") != std::string::npos)
	{
		uri.replace(uri.find("%20"), 3, " ");
	}
	DEBUG && std::cout << "URI: " << uri << std::endl;

	fill_client_data(client_class, req_map);

    
	int status = std::remove(uri.c_str());

    // Check if the file has been successfully removed
    if (status != 0) {
        DEBUG && std::cout << "Error deleting file\n";
		return 404;
    }
    else {
        DEBUG && std::cout << "File successfully deleted\n";
    }

	// client_class.set_body("<html>"
	// 				"<body>"
	// 					"<h1>File deleted.</h1>"
	// 				"</body>"
	// 			  "</html>");
	// client_class.set_content_length(client_class.get_body().size());
	DEBUG && std::cout << "content_len: " << client_class.get_body().size() << std::endl;
	client_class.set_content_type("text/html");

	return 200;
}


int POST_body(client &client_class)
{
	DEBUG && std::cout << "POST_BODY---------------------------------\n";
	std::string &req = client_class.get_request();
	std::string bound = ("\r\n--"+client_class.get_post_boundary());
	// DEBUG && std::cout << bound << '\n';
	DEBUG && std::cout <<'|'<<req<<'|';
	unsigned long index = req.find(bound);
	if(index != std::string::npos)
	{
			long long size = write(client_class.get_post_fd(), req.c_str(), index);
			if(size == -1)
				return -1;
			req.erase(0, size);
			client_class.add_post_written_len(size);
			client_class.decrement_request_size(size);
			DEBUG && std::cout << "cline_written_len = " << client_class.get_post_written_len() << "\n";

			DEBUG && std::cout << "im here here\n";
			std::string line  = get_line(req);
			client_class.add_post_written_len(line.size());
			client_class.decrement_request_size(line.size());
			// DEBUG && std::cout << line;
			line  = get_line(req);
			// DEBUG && std::cout << line;
			client_class.add_post_written_len(line.size());
			client_class.decrement_request_size(line.size());
			while(line != "\r\n")
			{
				// parse_headers(line, req_map);
				if(line.find("Content-Disposition:") != std::string::npos)
				{
					long long i = line.find("filename=\"") + 10;
					std::string filename = "";
					while(line[i] != '\"')
						filename += line[i++];
					DEBUG && std::cout << "filename = = " << filename << "\n";
					close(client_class.get_post_fd());
					client_class.set_post_filename(filename);
					client_class.set_post_fd(open(filename.c_str(), O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666));
				}
				line = get_line(req);
				client_class.add_post_written_len(line.size());
				client_class.decrement_request_size(line.size());
				// DEBUG && std::cout << line;
			}
			DEBUG && std::cout << "kdgfiehqfiouhweifuhewf--------------------\n";
	}
	// DEBUG && std::cout << "fd: " << client_class.get_post_fd() << std::endl;
	// DEBUG && std::cout << req << "\n";
	// DEBUG && std::cout << "efwefewfewf------\n";
	std::string boundary = client_class.get_post_boundary();
	long long write_size = 0;
	int status = 0;
	DEBUG && std::cout << "req.size() + client_class.get_post_written_len() = " << req.size() + client_class.get_post_written_len() << "\n";
	DEBUG && std::cout << "(client_class.get_post_filelength() - (boundary.size() + 6)) = " << (client_class.get_post_filelength() - (boundary.size() + 6)) << "\n";
	if((req.size() + client_class.get_post_written_len()) 
			>= (client_class.get_post_filelength() - (boundary.size() + 6)))
	{
		write_size = client_class.get_post_filelength() - (boundary.size() + 6) - client_class.get_post_written_len();
		long long size = 0;
		
			DEBUG && std::cout << "first while write\n";
			size += write(client_class.get_post_fd(), req.c_str(), write_size - size);
		
		req.erase(0, size);
		client_class.add_post_written_len(size);
		client_class.decrement_request_size(size);
		DEBUG && std::cout << "cline_written_len = " << client_class.get_post_written_len() << "\n";
		if(size == write_size)
		{
			close(client_class.get_post_fd());
			return 200;
		}
		else
		 	return -100;
	}else {
		write_size = req.size();
		long long size = 0;
		
			DEBUG && std::cout << "second while write\n";
			size += write(client_class.get_post_fd(), req.c_str(), write_size - size);
		
		req.erase(0, size);
		client_class.add_post_written_len(size);
		client_class.decrement_request_size(size);
		DEBUG && std::cout << "cline_written_len = " << client_class.get_post_written_len() << "\n";
		return -100;
	}
	return status;
}
// ----------------------------418835119977164444166495--
// --------------------------418835119977164444166495
int POST_CHUNKED_BODY(client &client_class)
{
	std::string &req = client_class.get_request();
	DEBUG && std::cout << "POST_CHUNKED_BODY--------------start-------------------\n";
	DEBUG && std::cout << req << '\n';
	DEBUG && std::cout << "POST_CHUNKED_BODY---------------end------------------\n";
	DEBUG && std::cout << "chunk_size = " << client_class.get_POST_chunk_size()<< "\n";
	if(client_class.get_POST_chunk_size() == 0 && req.size() >= 3)
	{
		std::string line = "\r\n";
		DEBUG && std::cout << line << "\n";
		std::cout << line.size() << "\n";
		while(line == "\r\n" && line.size() == 2)
		{
			line = get_line(req);
			DEBUG && std::cout << "--------------------------\n";
			DEBUG && std::cout << line << std::endl;
			DEBUG && std::cout << "--------------------------\n";
		}
		DEBUG && std::cout << "line = " << line << std::endl;
		client_class.set_POST_chunk_size(std::stoll(line, 0, 16));
		DEBUG && std::cout << "chunk_size = " << client_class.get_POST_chunk_size()<< "\n";
		if(client_class.get_POST_chunk_size() == 0 && line == "0\r\n")
		{
			DEBUG && std::cout << "chunk_size =-=-=-=-=-=-=-= = 0\n";
			client_class.set_status_code(200);
			return 200;
		}
	}
	long long chunk_size = (long long)req.size() < client_class.get_POST_chunk_size() ? req.size() : client_class.get_POST_chunk_size();
	std::string chunk = req.substr(0, chunk_size);
	req.erase(0, chunk_size);

	DEBUG && std::cout << "chunk = " << chunk << "\n";

	unsigned long separet_boundry = chunk.find("--" + client_class.get_post_boundary());
	unsigned long end_of_file_boundry = chunk.find("--" + client_class.get_post_boundary().substr(0, client_class.get_post_boundary().find("\r\n")) + "--\r\n");
	DEBUG && std::cout << "end_of_file_boundry = " << end_of_file_boundry << "\n";
	if(separet_boundry != std::string::npos && end_of_file_boundry == std::string::npos)
	{
		std::cout << "separet_boundry = " << separet_boundry << "\n";
		long long size = write(client_class.get_post_fd(), chunk.c_str(), separet_boundry);
		std::cout << "size = " << size << "\n";
		if(size == -1)
			return -1;
		chunk.erase(0, size);
		client_class.set_POST_chunk_size(client_class.get_POST_chunk_size() - size);
		DEBUG && std::cout << "get_line loop start\n";
		std::string line = "";
		while(line != "\r\n")
		{
			line = get_line(chunk);
			client_class.set_POST_chunk_size(client_class.get_POST_chunk_size() - line.size());
			DEBUG && std::cout << client_class.get_POST_chunk_size() << "\n";
			client_class.decrement_request_size(line.size());
			DEBUG && std::cout << line;
			if(line.find("Content-Disposition:") != std::string::npos)
			{
				long long i = line.find("filename=\"") + 10;
				std::string filename = "";
				while(line[i] != '\"')
					filename += line[i++];
				DEBUG && std::cout << "filename = = " << filename << "\n";
				client_class.set_post_filename(filename);
				client_class.set_post_fd(open(filename.c_str(), O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666));
			}
		}
		 std::cout << "chunk_size = " <<client_class.get_POST_chunk_size() << "\n";
		DEBUG && std::cout << "get_line loop end\n";
		if(client_class.get_POST_chunk_size() <= 0)
		{
			// req = chunk + req;
			return POST_CHUNKED_BODY(client_class);
		}else {
			DEBUG && std::cout << "error 400\n";
			return 400;
		}
	}
	if(end_of_file_boundry != std::string::npos)
	{
		DEBUG && std::cout << "end of file boundry\n";
		
		long long size = write(client_class.get_post_fd(), chunk.c_str(), end_of_file_boundry - 2 * (end_of_file_boundry >= 2));
		if(size == -1)
			return -1;
		chunk.erase(0, size);
		client_class.set_POST_chunk_size(client_class.get_POST_chunk_size() - size);
		DEBUG && std::cout << "chunk_size = " <<client_class.get_POST_chunk_size()<< "\n";
		// req.erase(0, end_of_file_boundry + client_class.get_post_boundary().size() + 4);
		chunk.erase(chunk.begin(), chunk.end());
		std::cout << "req = " << req << "\n";
		client_class.set_POST_chunk_size(0);
		std::cout << "chunk = " << chunk << "\n";
		// client_class.set_POST_chunk_size(0);
		// return 200;
	}
	if(chunk.size() == 2 && chunk == "\r\n")
	{
		chunk.erase(0, 2);
		client_class.set_POST_chunk_size(client_class.get_POST_chunk_size() - 2);		
	}

	long long size = write(client_class.get_post_fd(), chunk.c_str(), chunk.size());
	if(size == -1)
		return -1;
	chunk.erase(0, size);
	client_class.set_POST_chunk_size(client_class.get_POST_chunk_size() - size);
	DEBUG && std::cout << "chunk_size11111 = " <<client_class.get_POST_chunk_size()<< "\n";
	req = chunk + req;
	if(req.size() > 2)
	{
		return POST_CHUNKED_BODY(client_class);
	}
	return -100;
}


int POST_RAW(client &client_class)
{
	std::string &req = client_class.get_request();
	DEBUG && std::cout << "req.size() = " << req.size() << "\n";
	DEBUG && std::cout << "\n\n\nPOST RAW REQUEST START \n\n";
	DEBUG && std::cout << req << std::endl;
	DEBUG && std::cout << "\nPOST RAW REQUEST END\n";

	return 200;	
}

int POST_header(client &client_class, std::map<std::string, std::string> &req_map)
{
	std::string &req = client_class.get_request();
	DEBUG && std::cout << "req.size() = " << req.size() << "\n";
	DEBUG && std::cout << "\n\n\nPOST REQUEST START \n\n";
	DEBUG && std::cout << req << std::endl;
	DEBUG && std::cout << "\nPOST REQUEST END\n";

	std::string line = get_line(req);
	DEBUG && std::cout << line << std::endl;
	if (parse_initial_line(line, req_map) == -1)
	{
		DEBUG && std::cout << ("Error parsing initial line\n");
		return 400;
	}
	if(req_map["Method"] != "POST" || req_map.find("Method") == req_map.end())
	{
		DEBUG && std::cout << ("Method is not POST\n");
		return 400;
	}
	if(req_map.find("URI") == req_map.end())
	{
		DEBUG && std::cout << ("URI not found\n");
		return 400;
	}
	if(req_map["Version"] != "HTTP/1.1\r\n" || req_map.find("Version") == req_map.end())
	{
		DEBUG && std::cout << ("Version is not HTTP/1.1\n");
		return 400;
	}
	DEBUG && std::cout << "get_line loop start\n";
	while(line != "\r\n")
	{
		line = get_line(req);
		DEBUG && std::cout << line << std::endl;
		parse_headers(line, req_map);
	}
	DEBUG && std::cout << "get_line loop end\n";

	DEBUG && std::cout << "req.size() = " << req.size() << "\n";
	
	if(req_map["Content-Type"].find("boundary=") == std::string::npos && req_map["Content-Type"].find("multipart/form-data") != std::string::npos)
	{
		DEBUG && std::cout << "Boundary not found\n";
		return 400;
	}
	if(req_map["Content-Type"].find("multipart/form-data") == std::string::npos && req_map["Content-Type"].find("text/plain") == std::string::npos)
	{
		DEBUG && std::cout << "Content-Type is not multipart/form-data\n";
		return 400;
	}
	if(req_map.find("Content-Length") == req_map.end() && req_map["Transfer-Encoding"] != "chunked\r\n")
	{
		std::cout << "Content-Length not found\n";
		return 400;
	}

	fill_client_data(client_class, req_map);
	if(req_map["Content-Type"].find("multipart/form-data") != std::string::npos)
		client_class.set_post_boundary(req_map["Content-Type"].substr(req_map["Content-Type"].find("boundary=") + 9));
	DEBUG && std::cout << "Boundary: " << client_class.get_post_boundary() << std::endl;
	std::string uri = req_map["URI"][0] == '/' ? req_map["URI"].substr(1) : req_map["URI"];
	if(uri == "")
		uri = "index.html";
	uri = uri.substr(0, uri.find("?"));
	DEBUG && std::cout << "URI: " << uri << std::endl;
	if(req_map.find("Content-Length") != req_map.end())
		client_class.set_post_filelength(std::stoll(req_map["Content-Length"]));
	DEBUG && std::cout << "Content-Length: " << client_class.get_post_filelength() << std::endl;
	client_class.set_post_request_parsed(true);
	if(req.find("\r\n\r\n") == std::string::npos)
	{
		DEBUG && std::cout << "request doesnt have a body\n";
		return -100;
	}else {
		DEBUG && std::cout << "request has a body\n";
		// DEBUG && std::cout << req << "\n";
	}
	if(req_map.find("Transfer-Encoding") != req_map.end() && req_map["Transfer-Encoding"] == "chunked\r\n")
	{
		if(req_map.find("Content-Length") != req_map.end())
		{
			 std::cout << "Content-Length exist in chunked transfer NOT allowed\n";
			client_class.set_request_done(true);
			client_class.set_status_code(400);
			return 400;
		}
		if(req_map["Content-Type"].find("multipart/form-data") == std::string::npos)
		{
			const std::chrono::time_point<std::chrono::system_clock> p1 = std::chrono::system_clock::now();
			const std::string name = "client_" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count());
			client_class.set_post_fd(open(name.c_str(), O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666));
			client_class.set_POST_Chuncked(true);
			DEBUG && std::cout << "chunked RAW body\n";
			return POST_CHUNKED_BODY(client_class);
		}
		DEBUG && std::cout << "chunked body\n";
		client_class.set_POST_Chuncked(true);
		DEBUG && std::cout << "get_line loop start\n";
		line = get_line(req);
		DEBUG && std::cout << line << std::endl;
		client_class.set_POST_chunk_size(std::stoll(line, 0, 16));
		DEBUG && std::cout << "chunk_size = " << client_class.get_POST_chunk_size()<< "\n";
		if(client_class.get_POST_chunk_size() == 0 && line == "0\r\n")
		{
			DEBUG && std::cout << "chunk_size = 0\n";
			return 200;
		}
		line = get_line(req);
		client_class.set_POST_chunk_size(client_class.get_POST_chunk_size() - line.size());
		DEBUG && std::cout << line << std::endl;
		while(line != "\r\n")
		{
			// DEBUG && std::cout << line << std::endl;
			if(line.find(client_class.get_post_boundary()) != std::string::npos)
			{
				while(line != "\r\n")
				{
					line = get_line(req);
					parse_headers(line, req_map);
					client_class.set_POST_chunk_size(client_class.get_POST_chunk_size() - line.size());
					DEBUG && std::cout << client_class.get_POST_chunk_size() << "\n";
					client_class.decrement_request_size(line.size());
					DEBUG && std::cout << line;
					if(line.find("Content-Disposition:") != std::string::npos)
					{
						long long i = line.find("filename=\"") + 10;
						std::string filename = "";
						while(line[i] != '\"')
							filename += line[i++];
						DEBUG && std::cout << "filename = = " << filename << "\n";
						client_class.set_post_filename(filename);
						client_class.set_post_fd(open(filename.c_str(), O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666));
					}
				}

			}else {
				return 400;
			}
		}
		DEBUG && std::cout << "get_line loop end\n";
		
		if(client_class.get_POST_chunk_size() <= 0)
		{
			return POST_CHUNKED_BODY(client_class);
		}else {
			return 400;
		}

	}
	if(req_map.find("Content-Length") == req_map.end())
	{
		 std::cout << "Content-Length doesnt exist in normal transfer NOT allowed\n";
		client_class.set_request_done(true);
		client_class.set_status_code(400);
		return 400;
	}
	DEBUG && std::cout << "get_line loop start\n";
	line = get_line(req);
	parse_headers(line, req_map);
	client_class.add_post_written_len(line.size());
	client_class.decrement_request_size(line.size());
	DEBUG && std::cout << line;
	while(line != "\r\n")
	{
		// DEBUG && std::cout << line << std::endl;
		if(line.find(client_class.get_post_boundary()) != std::string::npos)
		{
			while(line != "\r\n")
			{
				line = get_line(req);
				parse_headers(line, req_map);
				client_class.add_post_written_len(line.size());
				client_class.decrement_request_size(line.size());
				DEBUG && std::cout << line;
				if(line.find("Content-Disposition:") != std::string::npos)
				{
					long long i = line.find("filename=\"") + 10;
					std::string filename = "";
					while(line[i] != '\"')
						filename += line[i++];
					DEBUG && std::cout << "filename = = " << filename << "\n";
					client_class.set_post_filename(filename);
					client_class.set_post_fd(open(filename.c_str(), O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0666));
				}
			}

		}else {
			return 400;
		}
	}
	DEBUG && std::cout << "get_line loop end\n";
	DEBUG && std::cout << "POST HEADER PARSED, req = \n";
	DEBUG && std::cout << req << "\n";
	DEBUG && std::cout << "req.size() = " << req.size() << "\n";
	if(req.size() > 0)
	{
		return POST_body(client_class);
	}
	// client_class.set_request("");
	return -100;
}
int GET(client &client_class, std::map<std::string, std::string> &req_map)
{
	std::string req = client_class.get_request();
	DEBUG && std::cout << "\n\n\nGET REQUEST START \n\n";
	// DEBUG && std::cout << req << std::endl;
	DEBUG && std::cout << "\nGET REQUEST END\n";

	std::string line = get_line(req);
	DEBUG && std::cout << line << std::endl;
	if (parse_initial_line(line, req_map) == -1)
	{
		DEBUG && std::cout << ("Error parsing initial line\n");
		return 400;
	}	
	if(req_map["Method"] != "GET" || req_map.find("Method") == req_map.end())
	{
		DEBUG && std::cout << ("Method is not GET\n");
		return 400;
	}	
	if(req_map.find("URI") == req_map.end())
	{
		DEBUG && std::cout << ("URI not found\n");
		return 400;
	}	
	if(req_map.find("Version") == req_map.end() || req_map["Version"] != "HTTP/1.1\r\n")
	{
		DEBUG && std::cout << ("Version is not HTTP/1.1\n");
		return 400;
	}	

	while (line != "\r\n")
	{
		line = get_line(req);
		DEBUG && std::cout << line << std::endl;
		parse_headers(line, req_map);
	}

	if(req_map.find("Host") == req_map.end())
	{
		DEBUG && std::cout << ("Host header not found\n");
		return 400;
	}	
	if(req_map["Host"] != http_hostname_macro && req_map["Host"] != http_localhost_macro)
	{
		DEBUG && std::cout << ("Host header is not correct\n");
		return 400;
	}	
	
	if(req_map["URI"] == "/")
		req_map["URI"] = "/index.html";

	std::string uri = req_map["URI"][0] == '/' ? req_map["URI"].substr(1) : req_map["URI"];
	while(uri.find("%20") != std::string::npos)
	{
		uri.replace(uri.find("%20"), 3, " ");
	}
	DEBUG && std::cout << "URI: " << uri << std::endl;
	uri = uri.substr(0, uri.find("?"));
	client_class.set_filename(uri);
	fill_client_data(client_class, req_map);

	// DEBUG && std::cout << content << std::endl;

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
	// DEBUG && std::cout << "entering post request handling ==================== \n " << client_class.get_request() << '\n';
	if(client_class.get_request().size() == 0) 
	{
		DEBUG && std::cout << ("No request to handle\n");
		return 500;
	}

	if(client_class.get_post_request_parsed())
		{
			DEBUG && std::cout << ("POST body request found\n");
			if(client_class.get_POST_Chuncked())
			{
				int status = POST_CHUNKED_BODY(client_class);
				client_class.set_status_code(status);
				return status;
			}
			int status = POST_body(client_class);
			client_class.set_status_code(status);
			return status;
		}

	std::string req = client_class.get_request();
	// DEBUG && std::cout << "\n\n\nREQUEST START \n\n";
	// DEBUG && std::cout << req << std::endl;
	// DEBUG && std::cout << "\nREQUEST END\n";
	if(req.find("\r\n\r\n") == std::string::npos)
	{
		DEBUG && std::cout << ("Request not complete\n");
		return 400;
	}
	std::string line;
	size_t pos = req.find("\r\n");
	if (pos != std::string::npos)
	{
		line = req.substr(0, pos);
	}
	// DEBUG && std::cout << line << std::endl;
	std::map<std::string, std::string> req_map;
	if (parse_initial_line(line, req_map) == -1)
	{
		DEBUG && std::cout << ("Error parsing initial line\n");
		return 400;
	}	
	if(req_map.find("Method") == req_map.end())
	{
		DEBUG && std::cout << ("Method not found\n");
		return 400;
	}	
	if(req_map.find("URI") == req_map.end())
	{
		DEBUG && std::cout << ("URI not found\n");
		return 400;
	}	
	if(req_map.find("Version") == req_map.end())
	{
		DEBUG && std::cout << ("Version not found\n");
		return 400;
	}	
	if(req_map["Version"] != "HTTP/1.1")
	{
		DEBUG && std::cout << ("Version not supported\n");
		return 505;
	}
	if(req_map["Method"] == "GET")
	{
		DEBUG && std::cout << ("GET request found\n");
		int status =  GET(client_class, req_map);
		
			client_class.set_status_code(int(status));
			client_class.set_connection_close(req_map["Connection"].find("keep-alive") != std::string::npos  ? 0 : 1);
			client_class.set_method(req_map["Method"]);
			client_class.set_uri(req_map["URI"]);
			client_class.set_version(req_map["Version"].substr(0, req_map["Version"].size() - 2));
			client_class.set_host(req_map["Host"].substr(0, req_map["Host"].find(":")));
			client_class.set_port(req_map["Host"].substr(req_map["Host"].find(":") + 1));
			client_class.set_path(req_map["URI"].substr(0, req_map["URI"].find("?")));
		
		client_class.set_requestvalid(bool(status == 200 || status == 404));
		return status;

	}
	else if(req_map["Method"] == "DELETE")
	{
		DEBUG && std::cout << ("DELETE request found\n");
		int status = DELETE(client_class, req_map);

			client_class.set_status_code(int(status));
			client_class.set_connection_close(req_map["Connection"].find("keep-alive") != std::string::npos ? 0 : 1);
			client_class.set_method(req_map["Method"]);
			client_class.set_uri(req_map["URI"]);
			client_class.set_version(req_map["Version"].substr(0, req_map["Version"].size() - 2));
			client_class.set_host(req_map["Host"].substr(0, req_map["Host"].find(":")));
			client_class.set_port(req_map["Host"].substr(req_map["Host"].find(":") + 1));
			client_class.set_path(req_map["URI"].substr(0, req_map["URI"].find("?")));
		
		client_class.set_requestvalid(bool(status == 200 || status == 404));
		return status;
	}
	else if (req_map["Method"] == "POST")
	{
		DEBUG && std::cout << ("POST request found\n");
		int status = POST_header(client_class, req_map);
		client_class.set_status_code(status);

		return status;
	}
	else
	{
		
		DEBUG && std::cout << ("Method not supported\n");
		return 405;
	}
	return 500;
}