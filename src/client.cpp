#include "client.hpp"
#include <string>
#include <sstream>

client::client(int fd) : _socketfd(fd){
    this->header_done = false;
    this->body_done = false;
    this->request_done = false;
    this->status_code = 0;
    this->body = "";
    this->header = "";
    this->_request = "";
    this->_response = "";
}

client::client(){
    this->header_done = false;
    this->body_done = false;
    this->request_done = false;
    this->status_code = 0;
    this->body = "";
    this->header = "";
    this->_request = "";
    this->_response = "";
}

client::~client(){
    this->clear_all();
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

void client::set_request(std::string req){
    this->_request.append(req);
}

void client::set_response(std::string &res){
    this->_response = res;
}

void client::set_status_code(int code){
    this->status_code = code;
}

void client::set_socketfd(int fd){
    this->_socketfd = fd;
}

int client::get_socketfd(){
    return (this->_socketfd);
}

void client::clear_request(){
    this->_request.clear();
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
    this->body.append(body);
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

void client::set_status_message(std::string message){
    this->status_message = message;
}

std::string client::get_content_type(){
    return (this->content_type);
}

void client::set_content_type(std::string type){
    this->content_type = type;
}

long long client::get_content_length(){
    return (this->content_length);
}

void client::set_content_length(long long length){
    this->content_length = length;
}

std::string client::tostring(long long num){
    std::ostringstream convert ;
    convert << num;
    std::string result(convert.str());
    return (result);
}

void client::build_response(){
    std::string response = version + " " + tostring((long long)status_code) + " " + status_message + CRLF\
        + "Content-Type: " + content_type + CRLF\
        + "Content-Length: " + tostring(content_length)+ CRLF+CRLF\
        + c;

}

bool client::is_chunked(){
    return (this->chunked);
}

void client::set_chunked(bool is){
    this->chunked = is;
}

void client::set_requestvalid(bool is){
    this->requestvalid = is;
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
}
