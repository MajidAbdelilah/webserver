#include "server.hpp"

#include <fstream>
#include <string>
#include <sys/stat.h>

#ifndef CLIENT_HPP
#define CLIENT_HPP


#define CRLF "\r\n"



class client {
    private:
        // std::ifstream file;
		std::string _filename;
        int filefd;
        int _socketfd; // socket file descriptor
        std::string body; // body of the request
        std::string header; // header of the request
        std::string _request; // request to be parsed
        std::string _response; // response to be sent
        struct stat filestat; // file status
        

        std::string response_header; // header of the response
        std::string response_body; // body of the response

        int status_code; // 200, 404, 500 etc
        bool header_done; // 1 if header is done 0 if not
        bool body_done; // 1 if body is done 0 if not
        bool request_done; // 1 if request is done 0 if not
        bool connection_close; // 1 if close 0 if keep alive
        std::string method; // example GET http://example.com/page -> GET
        std::string uri; // example http://example.com/page -> http://example.com/page
        std::string version; // example GET http://example.com/page HTTP/1.1 -> HTTP/1.1
        std::string host; // example http://example.com/page -> example.com
        std::string port; // example http://example.com:8080/page -> 8080
        std::string path; // example http://example.com/page -> /page
        std::string query; // example http://example.com/page?param1=value1&param2=value2 -> param1=value1&param2=value2
        std::string fragment; // example http://example.com/page#fragment -> fragment
        std::string status_message; // example 200 OK -> OK
        long long content_length; // example Content-Length: 12 -> 12
        std::string content_type; // example Content-Type: text/html -> text/html
        bool chunked; // 1 if chunked 0 if not
        bool requestvalid; // 1 if request is valid 0 if not
        bool ifstream_empty; // 1 if ifstream is empty 0 if not
        std::string post_boundary;
        // bool whole_response_sent; // 1 if whole response is sent 0 if not
        int postfd;
        std::string postfilename;
        std::string postfiletype;
        std::string postfiledata;
        std::string postfileboundary;
        std::string postfileboundaryend;
        bool content_length_valid;
        bool encoding_length_conflict;
        long long postfilelength;
        bool post_request_parsed;
		bool post_body_header_parsed;
        unsigned long post_written_len;
        unsigned long request_size;
        int _bytesread;
    public:
        client();
        client(int fd);
        ~client();
        std::string &get_request();
        std::string &get_response();
        int get_status_code();
        int get_post_fd();
        std::string get_post_filename();
        std::string get_post_filetype();
        std::string get_post_filedata();
        std::string get_post_fileboundary();
        std::string get_post_fileboundaryend();
        unsigned long get_request_size() const
        {
            return request_size;
        }
        void decrement_request_size(const unsigned long &size)
        {
            request_size -= size;
        }
        void add_post_written_len(const unsigned long &size)
        {
            post_written_len += size;
        }

        unsigned long get_post_written_len() const
        {
            return post_written_len;
        }
        

        long long get_post_filelength();
        bool get_content_length_valid();
        bool get_encoding_length_conflict();
        void set_post_filelength(long long);
        void set_post_fd(int);
        void set_post_filename(std::string);
        void set_post_filetype(std::string);
        void set_post_filedata(std::string);
        void set_post_fileboundary(std::string);
        void set_post_fileboundaryend(std::string);
        void set_content_length_valid(bool);
        void set_encoding_length_conflict(bool);
        void set_post_request_parsed(bool);
        bool get_post_request_parsed();
        void set_request(std::string );
        void set_response(std::string );
        client &operator=(const client &);
		void set_filename(std::string);
        std::string get_post_boundary();
        void set_post_boundary(std::string);
        std::string get_filename();
        void set_filefd(int);
        int get_filefd();
        void set_status_code(int);
        void set_socketfd(int);
        int get_socketfd();
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
        void set_connection_close(bool);
        bool get_connection_close();
        void set_method(std::string);
        void set_uri(std::string);
        void set_version(std::string);
        void set_host(std::string);
        void set_port(std::string);
        void set_path(std::string);
        void set_query(std::string);
        void set_fragment(std::string);
        std::string get_method();
        std::string get_uri();
        std::string get_version();
        std::string get_host();
        int get_bytesread();
        void set_bytesread(int);
        std::string get_port();
        std::string get_path();
        std::string get_query();
        std::string get_fragment();
        std::string get_response_header();
        bool is_chunked();
        void set_chunked(bool);
        void set_append_with_bytes(char *s, int );
        void set_requestvalid(bool);
        bool is_requestvalid();
        void set_content_length(long long);
        long long get_content_length();
        void set_content_type(std::string);
        std::string get_content_type();
        void set_status_message(int);
        std::string get_status_message();
        void build_response();
        std::string tostring(long long);
        void set_response_header(std::string);
        bool get_ifstreamempty();
        void set_ifstreamempty(bool);
        void clear_method();
        void clear_uri();
        void clear_version();
        void clear_host();
        void clear_port();
        void clear_path();
        void clear_query();
        void clear_fragment();
        void clear_body();
        void clear_header();
        void clear_request();
        void clear_response();
        void clear_status_code();
        void clear_socketfd();
        void clear_request_response_elements();
        void clear_post_elements();
        void clear_all();

};

void extern_set_append_with_bytes(std::string &, const char *, int );


#endif