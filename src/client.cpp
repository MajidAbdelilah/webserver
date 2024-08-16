#include "client.hpp"

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

void client::clear(){
    this->clear_all();
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

void client::clear_all(){
    this->clear_request();
    this->clear_response();
    this->clear_status_code();
    this->clear_socketfd();
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

