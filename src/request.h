#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <vector>

#include "buffer.h"

namespace lite{

struct Header{
    std::string name;
    std::string value;
};

struct Request{
    std::string method;
    std::string uri;
    int http_version_major;
    int http_version_minor;
    std::vector<Header> headers;
    std::string body;
    void clear(){
        method.clear();
        uri.clear();
        headers.clear();
        body.clear();
    }
};

struct Response{

    enum status_type{
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        multiple_choices = 300,
        moved_permanently = 301,
        moved_temporarily = 302,
        not_modified = 304,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
    } status;

    std::vector<Header> headers;

    std::string contents;

    int file_fd=-1; //需要发送的文件fd，-1代表没有文件发送
    long file_size;

    static std::string ExtensionToType(std::string);
    static Response BuildResponse(Response::status_type status);
    int WriteToBuffer(Buffer& buffer); //将response写入缓冲区
};

}
#endif