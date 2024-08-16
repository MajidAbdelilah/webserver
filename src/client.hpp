#include "server.hpp"

#ifndef CLIENT_HPP
#define CLIENT_HPP

class client {
    private:
        int _socketfd;
        std::string body;
        std::string header;
        std::string _request;
        std::string _response;
        int status_code;
        bool header_done;
        bool body_done;
        bool request_done;
        bool connection_close; // 1 if close 0 if keep alive

    public:
        client();
        client(int fd);
        ~client();
        std::string &get_request();
        std::string &get_response();
        int get_status_code();
        void set_request(std::string );
        void set_response(std::string &);
        void set_status_code(int);
        void set_socketfd(int);
        int get_socketfd();
        void clear();
        void clear_request();
        void clear_response();
        void clear_status_code();
        void clear_socketfd();
        void clear_all();
        bool is_header_done();
        bool is_body_done();
        bool is_request_done();
        void set_header_done(bool);
        void set_body_done(bool);
        void set_request_done(bool);
        void set_body(std::string);
        void set_header(std::string);
        std::string get_body();
        std::string get_header();
        void clear_body();
        void clear_header();
        


};


#endif