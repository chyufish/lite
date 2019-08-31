#include "request.h"

#include <string>
#include <unordered_map>
#include <algorithm>
#include <iostream>

namespace lite{

std::string StatusLine(Response::status_type status){

    switch(status){
        case Response::ok:
            return "HTTP/1.0 200 OK\r\n";
        case Response::created:
            return "HTTP/1.0 201 Created\r\n";
        case Response::accepted:
            return "HTTP/1.0 202 Accepted\r\n";
        case Response::no_content:
            return "HTTP/1.0 204 No Content\r\n";
        case Response::multiple_choices:
            return "HTTP/1.0 300 Multiple Choices\r\n";
        case Response::moved_permanently:
            return "HTTP/1.0 301 Moved Permanently\r\n";
        case Response::moved_temporarily:
            return "HTTP/1.0 302 Moved Temporarily\r\n";
        case Response::not_modified:
            return "HTTP/1.0 304 Not Modified\r\n";
        case Response::bad_request:
            return "HTTP/1.0 400 Bad Request\r\n";
        case Response::unauthorized:
            return "HTTP/1.0 401 Unauthorized\r\n";
        case Response::forbidden:
            return "HTTP/1.0 403 Forbidden\r\n";
        case Response::not_found:
            return "HTTP/1.0 404 Not Found\r\n";
        case Response::internal_server_error:
            return "HTTP/1.0 500 Internal Server Error\r\n";
        case Response::not_implemented:
            return "HTTP/1.0 501 Not Implemented\r\n";
        case Response::bad_gateway:
            return "HTTP/1.0 502 Bad Gateway\r\n";
        case Response::service_unavailable:
            return "HTTP/1.0 503 Service Unavailable\r\n";
	default:
	    return "";
    }
}

namespace status_html{

const char ok[] = "";
const char created[] =
    "<html>"
    "<head><title>Created</title></head>"
    "<body><h1>201 Created</h1></body>"
    "</html>";
const char accepted[] =
    "<html>"
    "<head><title>Accepted</title></head>"
    "<body><h1>202 Accepted</h1></body>"
    "</html>";
const char no_content[] =
    "<html>"
    "<head><title>No Content</title></head>"
    "<body><h1>204 Content</h1></body>"
    "</html>";
const char multiple_choices[] =
    "<html>"
    "<head><title>Multiple Choices</title></head>"
    "<body><h1>300 Multiple Choices</h1></body>"
    "</html>";
const char moved_permanently[] =
    "<html>"
    "<head><title>Moved Permanently</title></head>"
    "<body><h1>301 Moved Permanently</h1></body>"
    "</html>";
const char moved_temporarily[] =
    "<html>"
    "<head><title>Moved Temporarily</title></head>"
    "<body><h1>302 Moved Temporarily</h1></body>"
    "</html>";
const char not_modified[] =
    "<html>"
    "<head><title>Not Modified</title></head>"
    "<body><h1>304 Not Modified</h1></body>"
    "</html>";
const char bad_request[] =
    "<html>"
    "<head><title>Bad Request</title></head>"
    "<body><h1>400 Bad Request</h1></body>"
    "</html>";
const char unauthorized[] =
    "<html>"
    "<head><title>Unauthorized</title></head>"
    "<body><h1>401 Unauthorized</h1></body>"
    "</html>";
const char forbidden[] =
    "<html>"
    "<head><title>Forbidden</title></head>"
    "<body><h1>403 Forbidden</h1></body>"
    "</html>";
const char not_found[] =
    "<html>"
    "<head><title>Not Found</title></head>"
    "<body><h1>404 Not Found</h1></body>"
    "</html>";
const char internal_server_error[] =
    "<html>"
    "<head><title>Internal Server Error</title></head>"
    "<body><h1>500 Internal Server Error</h1></body>"
    "</html>";
const char not_implemented[] =
    "<html>"
    "<head><title>Not Implemented</title></head>"
    "<body><h1>501 Not Implemented</h1></body>"
    "</html>";
const char bad_gateway[] =
    "<html>"
    "<head><title>Bad Gateway</title></head>"
    "<body><h1>502 Bad Gateway</h1></body>"
    "</html>";
const char service_unavailable[] =
    "<html>"
    "<head><title>Service Unavailable</title></head>"
    "<body><h1>503 Service Unavailable</h1></body>"
    "</html>";

std::string to_string(Response::status_type status){

    switch (status){
    case Response::ok:
        return ok;
    case Response::created:
        return created;
    case Response::accepted:
        return accepted;
    case Response::no_content:
        return no_content;
    case Response::multiple_choices:
        return multiple_choices;
    case Response::moved_permanently:
        return moved_permanently;
    case Response::moved_temporarily:
        return moved_temporarily;
    case Response::not_modified:
        return not_modified;
    case Response::bad_request:
        return bad_request;
    case Response::unauthorized:
        return unauthorized;
    case Response::forbidden:
        return forbidden;
    case Response::not_found:
        return not_found;
    case Response::internal_server_error:
        return internal_server_error;
    case Response::not_implemented:
        return not_implemented;
    case Response::bad_gateway:
        return bad_gateway;
    case Response::service_unavailable:
        return service_unavailable;
    default:
        return internal_server_error;
    }
}

}

std::unordered_map<std::string,std::string> mapping{
    {"gif","image/gif"},
    {"jpg","image/jpg"},
    {"png","image/png"},
    {"htm","text/htm"},
    {"html","text/html"}
};

std::string Response::ExtensionToType(std::string extension){
    if(mapping.find(extension)!=mapping.end()){
        return mapping[extension];
    }
    return "text/plain";
}

Response Response::BuildResponse(Response::status_type status){
    Response response;
    response.status=status;
    response.contents=status_html::to_string(status);
    response.headers.resize(2);
    response.headers[0].name="Content-Length";
    response.headers[0].value=std::to_string(response.contents.size());
    response.headers[1].name="Content-Type";
    response.headers[1].value="text/html";
    return response;
}

int Response::WriteToBuffer(Buffer& buffer){
    const std::string sp=": ";
    const std::string CRLF="\r\n";
    std::string status_line=StatusLine(status);
    auto it=std::copy(status_line.begin(),status_line.end(),buffer.buffer_.begin());
    for(std::size_t i=0;i<headers.size();++i){
        it=std::copy(headers[i].name.begin(),headers[i].name.end(),it);
        it=std::copy(sp.begin(),sp.end(),it);
        it=std::copy(headers[i].value.begin(),headers[i].value.end(),it);
        it=std::copy(CRLF.begin(),CRLF.end(),it);
    }
    it=std::copy(CRLF.begin(),CRLF.end(),it);
    it=std::copy(contents.begin(),contents.end(),it);
    buffer.HasWritten(it-buffer.buffer_.begin());
}

}
