#include "client.hpp"
#include <string>
#include <sstream>


client::client(int fd) : _socketfd(fd){
	_filename = "";
    this->header_done = false;
    this->body_done = false;
    this->request_done = false;
    this->status_code = 0;
    this->body = "";
    this->header = "";
    this->_request = "";
    this->_response = "";
    this->content_length = 0;
    this->chunked = false;
    this->requestvalid = false;
    this->connection_close = false;
    this->filefd = 0;
    this->ifstream_empty = false;
    this->postfd = 0;
    this->postfilelength = 0;
    this->content_length_valid = false;
    this->encoding_length_conflict = false;
    this->postfilename = "";
    this->postfiletype = "";
    this->postfiledata = "";
    this->postfileboundary = "";
    this->postfileboundaryend = "";
    this->post_boundary = "";
    this->post_request_parsed = false;
    this->post_body_header_parsed = false;
    post_written_len = 0;
    _bytesread = 0;
    request_size = 0;
}

client::client(){
	_filename = "";
    this->header_done = false;
    this->body_done = false;
    this->request_done = false;
    this->status_code = 0;
    this->body = "";
    this->header = "";
    this->_request = "";
    this->_response = "";
    this->content_length = 0;
    this->chunked = false;
    this->requestvalid = false;
    this->connection_close = false;
    this->filefd = 0;
    this->ifstream_empty = false;
    this->postfd = 0;
    this->postfilelength = 0;
    this->content_length_valid = false;
    this->encoding_length_conflict = false;
    this->postfilename = "";
    this->postfiletype = "";
    this->postfiledata = "";
    this->postfileboundary = "";
    this->postfileboundaryend = "";
    this->post_boundary = "";
	this->post_body_header_parsed = false;
    this->post_request_parsed = false;
    post_written_len = 0;
    _bytesread = 0;
    request_size = 0;
}

client::~client(){
    this->clear_all();
}


int client::get_bytesread(){
    return (_bytesread);
}

void client::set_bytesread(int bytes){
    this->_bytesread  = bytes;
}


// IMPORTANT NOTE: this DOES NOT copy the ifstream file
client &client::operator=(const client &rhs){
	this->_socketfd = rhs._socketfd;
	this->header_done = rhs.header_done;
	this->body_done = rhs.body_done;
	this->request_done = rhs.request_done;
	this->status_code = rhs.status_code;
	this->body = rhs.body;
	this->header = rhs.header;
	this->_request = rhs._request;
	this->_response = rhs._response;
	this->method = rhs.method;
	this->uri = rhs.uri;
	this->version = rhs.version;
	this->host = rhs.host;
	this->port = rhs.port;
	this->path = rhs.path;
	this->query = rhs.query;
	this->fragment = rhs.fragment;
	this->connection_close = rhs.connection_close;
	this->content_length = rhs.content_length;
	this->content_type = rhs.content_type;
	this->status_message = rhs.status_message;
	this->chunked = rhs.chunked;
	return (*this);
}

void client::set_requestvalid(bool valid){
	this->requestvalid = valid;
}

void client::set_response_header(std::string header){
    this->response_header = header;
}


std::string &client::get_request(){
    return (this->_request);
}

std::string &client::get_response(){
    return (this->_response);
}

int client::get_status_code(){
    return (this->status_code);
}
void client::set_filename(std::string filename){
	_filename = filename;
}

void client::set_append_with_bytes(char *req, int bytes_read){
    this->_request.append(req,  bytes_read);
    request_size += bytes_read;
}


void client::set_request(std::string req){
    this->_request.append(req);
    request_size += req.size();
}

void extern_set_append_with_bytes(std::string &a, const char *req, int bytes_read){
    a.append(req, bytes_read);
}


void client::set_response(std::string res){
    this->_response = res;
}

void client::set_status_code(int code){
    this->status_code = code;
}

void client::set_socketfd(int fd){
    this->_socketfd = fd;
}


void client::set_content_length(long long len){
	this->content_length = len;
}

void client::set_content_type(std::string type){
	this->content_type = type;
}

int client::get_socketfd(){
    return (this->_socketfd);
}

void client::clear_request(){
    this->_request.clear();
    request_size = 0;
}

void client::clear_response(){
    this->_response.clear();
}

void client::clear_status_code(){
    this->status_code = 0;
}

void client::clear_socketfd(){
    this->_socketfd = 0;
}


bool client::is_header_done(){
    return (this->header_done);
}

bool client::is_body_done(){
    return (this->body_done);
}

bool client::is_request_done(){
    return (this->request_done);
}

void client::set_header_done(bool done){
    this->header_done = done;
}

void client::set_body_done(bool done){
    this->body_done = done;
}

void client::set_request_done(bool done){
    this->request_done = done;
}

void client::set_body(std::string body){
    this->body=(body);
}

void client::set_header(std::string header){
    this->header.append(header);
}

std::string client::get_body(){
    return (this->body);
}

std::string client::get_header(){
    return (this->header);
}

void client::clear_body(){
    this->body.clear();
}

void client::clear_header(){
    this->header.clear();
}

void client::set_connection_close(bool close){
    this->connection_close = close;
}

bool client::get_connection_close(){
    return (this->connection_close);
}

void client::set_method(std::string method){
    this->method = method;
}

void client::set_uri(std::string uri){
    this->uri = uri;
}

void client::set_version(std::string version){
    this->version = version;
}

void client::set_host(std::string host){
    this->host = host;
}

void client::set_port(std::string port){
    this->port = port;
}

void client::set_path(std::string path){
    this->path = path;
}

void client::set_query(std::string query){
    this->query = query;
}

void client::set_fragment(std::string fragment){
    this->fragment = fragment;
}

std::string client::get_method(){
    return (this->method);
}

std::string client::get_uri(){
    return (this->uri);
}

std::string client::get_version(){
    return (this->version);
}

std::string client::get_post_boundary(){
    return (this->post_boundary);
}

void client::set_post_boundary(std::string boundary){
    this->post_boundary = boundary;
}

std::string client::get_host(){
    return (this->host);
}

std::string client::get_port(){
    return (this->port);
}

std::string client::get_path(){
    return (this->path);
}

std::string client::get_query(){
    return (this->query);
}

std::string client::get_fragment(){
    return (this->fragment);
}


std::string client::get_status_message(){
    return (this->status_message);
}

void client::set_status_message(int status_code){
    switch (status_code) {
        case 200:
            this->status_message = "OK";
            break;
        case 201:
            this->status_message = "Created";
            break;
        case 202:
            this->status_message = "Accepted";
            break;
        case 204:
            this->status_message = "No Content";
            break;
        case 301:
            this->status_message = "Moved Permanently";
            break;
        case 302:
            this->status_message = "Found";
            break;
        case 303:
            this->status_message = "See Other";
            break;
        case 304:
            this->status_message = "Not Modified";
            break;
        case 400:
            this->status_message = "Bad Request";
            break;
        case 401:
            this->status_message = "Unauthorized";
            break;
        case 403:
            this->status_message = "Forbidden";
            break;
        case 404:
            this->status_message = "Not Found";
            break;
        case 405:
            this->status_message = "Method Not Allowed";
            break;
        case 406:
            this->status_message = "Not Acceptable";
            break;
        case 408:
            this->status_message = "Request Timeout";
            break;
        case 409:
            this->status_message = "Conflict";
            break;
        case 411:
            this->status_message = "Length Required";
            break;
        case 413:
            this->status_message = "Payload Too Large";
            break;
        case 414:
            this->status_message = "URI Too Long";
            break;
        case 415:
            this->status_message = "Unsupported Media Type";
            break;
        case 500:
            this->status_message = "Internal Server Error";
            break;
        case 501:
            this->status_message = "Not Implemented";
            break;
        case 502:
            this->status_message = "Bad Gateway";
            break;
        case 503:
            this->status_message = "Service Unavailable";
            break;
        case 505:
            this->status_message = "HTTP Version Not Supported";
            break;
        default:
            this->status_message = "Internal Server Error";
            break;
    }
}

std::string client::get_content_type(){
    return (this->content_type);
}


long long client::get_content_length(){
    return (this->content_length);
}


std::string client::tostring(long long num){
    std::ostringstream convert ;
    convert << num;
    std::string result(convert.str());
    return (result);
}



void client::build_response(){
    std::cout << "here 1---------\n";
    if (method == "POST"){
        set_status_message(status_code); // setting the message
        _response  = version.erase(version.size() -2 * (version.size() >= 2)) + " " + tostring((long long)status_code) + " " + status_message + CRLF\
        + "Content-Type: " + content_type + CRLF\
        + "Content-Length: " + tostring(0)+ CRLF \
        + (!connection_close ? "Connection: keep-alive\r\nKeep-Alive: timeout=10, max=20" : "Connection: close" ) + CRLF + CRLF;
    // setting the header
    response_header = _response;
    return ;
    }
    std::cout << "here 2---------\n";
    if (_filename != ""){
        filefd = open(_filename.c_str(), O_RDONLY);
        if (filefd < 0){
            filefd = -2;
            set_status_code(404);
            set_content_length(0);
        }
        else if (filefd != -2 && -1 ==fstat(filefd, &filestat)){
            set_content_length(0);
            perror("fstat");
        }
        else
            set_content_length(filestat.st_size);
    }
    set_status_message(status_code); // setting the message
    long long length = get_content_length();
    std::cout << "here 3---------\n";
    std::cout << version.size() << "\n";
    response_header = version.erase(version.size() -2 * (version.size() >= 2)) + " " + tostring((long long)status_code) + " " + status_message + CRLF\
        + "Content-Type: " + content_type + CRLF\
        + "Content-Length: " + tostring(length)+ CRLF \
        + (!connection_close ? "Connection: keep-alive\r\nKeep-Alive: timeout=10, max=20" : "Connection: close" ) + CRLF + CRLF;
    std::cout << response_header << "\n";
    std::cout << "here 4---------\n";
    // setting the header
    _response = response_header;
    if (_filename != "" && filefd > 0){
        char buffer[1024];
        int bytes_read = read(filefd, buffer, 1024);
        if (bytes_read < 0){
            perror("read");
        }
        _response.append(buffer, bytes_read);
    }
    ifstream_empty = false;
    clear_request();
    clear_header();
    clear_body();
    request_done = false;
    header_done = false;
    body_done = false;
    requestvalid = false;
}

long long client::get_post_filelength(){
    return (postfilelength);
}

void client::set_post_filelength(long long length){
    postfilelength = length;
}

std::string client::get_post_filename(){
    return (postfilename);
}

void client::set_post_filename(std::string filename){
    postfilename = filename;
}

std::string client::get_post_filetype(){
    return (postfiletype);
}

void client::set_post_filetype(std::string filetype){
    postfiletype = filetype;
}

std::string client::get_post_filedata(){
    return (postfiledata);
}

void client::set_post_filedata(std::string data){
    postfiledata = data;
}

std::string client::get_post_fileboundary(){
    return (postfileboundary);
}

void client::set_post_fileboundary(std::string boundary){
    postfileboundary = boundary;
}

std::string client::get_post_fileboundaryend(){
    return (postfileboundaryend);
}

void client::set_post_fileboundaryend(std::string boundaryend){
    postfileboundaryend = boundaryend;
}

int client::get_post_fd(){
    return (postfd);
}

void client::set_post_fd(int fd){
    postfd = fd;
}

bool client::get_content_length_valid(){
    return (content_length_valid);
}

void client::set_content_length_valid(bool valid){
    content_length_valid = valid;
}

bool client::get_encoding_length_conflict(){
    return (encoding_length_conflict);
}

void client::set_encoding_length_conflict(bool conflict){
    encoding_length_conflict = conflict;
}

bool client::get_post_request_parsed(){
    return (post_request_parsed);
}

void client::set_post_request_parsed(bool parsed){
    post_request_parsed = parsed;
}



std::string client::get_response_header(){
    return (response_header);
}

bool client::is_chunked(){
    return (this->chunked);
}

void client::set_chunked(bool is){
    this->chunked = is;
}

bool client::is_requestvalid(){
    return (this->requestvalid);
}


void client::clear_method(){
    this->method.clear();
}

void client::clear_uri(){
    this->uri.clear();
}

void client::clear_version(){
    this->version.clear();
}

void client::clear_host(){
    this->host.clear();
}

void client::clear_port(){
    this->port.clear();
}

void client::clear_path(){
    this->path.clear();
}

void client::clear_query(){
    this->query.clear();
}

void client::clear_fragment(){
    this->fragment.clear();
}

void client::clear_all(){
    this->clear_request();
    this->clear_response();
    this->clear_status_code();
    this->clear_socketfd();
    this->clear_method();
    this->clear_uri();
    this->clear_version();
    this->clear_host();
    this->clear_port();
    this->clear_path();
    this->clear_query();
    this->clear_fragment();
    this->clear_body();
    this->clear_header();
    this->set_requestvalid(false);
    this->set_content_length(0);
    this->set_content_type("");
    this->set_status_message(0);
    this->set_chunked(false);
    this->set_connection_close(false);
    this->set_response_header("");
    this->set_ifstreamempty(false);
    if (filefd > 0){
        close(filefd);
    }
    filefd = -2;
    if (postfd > 0){
        close(postfd);
    }
    postfd = -2;
    postfilelength = 0;
    postfilename.clear();
    postfiletype.clear();
    postfiledata.clear();
    postfileboundary.clear();
    postfileboundaryend.clear();
    post_boundary.clear();
    content_length_valid = 0;
    _bytesread = 0;
}


void client::set_filefd(int fd){
    this->filefd = fd;
}

int client::get_filefd(){
    return (this->filefd);
}


std::string client::get_filename(){
    return (_filename);
}

bool client::get_ifstreamempty(){
    return (ifstream_empty);
}

void client::set_ifstreamempty(bool empty){
    ifstream_empty = empty;
}

void client::clear_post_elements(){
    postfilelength = 0;
    postfilename.clear();
    postfiletype.clear();
    postfiledata.clear();
    postfileboundary.clear();
    postfileboundaryend.clear();
    postfd = 0;
    content_length_valid = false;
    encoding_length_conflict = false;
}


void client::clear_request_response_elements(){
    if (filefd > 0){
        close(filefd);
    }
    if (postfd > 0){
        close(postfd);
    }
    _filename = "";
    this->header_done = false;
    this->body_done = false;
    this->request_done = false;
    this->status_code = 0;
    this->body = "";
    this->header = "";
    this->_request = "";
    this->_response = "";
    this->content_length = 0;
    this->chunked = false;
    this->requestvalid = false;
    this->connection_close = false;
    this->filefd = 0;
    this->ifstream_empty = false;
    this->postfd = 0;
    this->postfilelength = 0;
    this->content_length_valid = false;
    this->encoding_length_conflict = false;
    this->postfilename = "";
    this->postfiletype = "";
    this->postfiledata = "";
    this->postfileboundary = "";
    this->postfileboundaryend = "";
    this->post_boundary = "";
    this->post_request_parsed = false;
    this->post_body_header_parsed = false;
    post_written_len = 0;
    _bytesread = 0;
    request_size = 0;
}