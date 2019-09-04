#include "request_handler.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <string>
#include <fstream>
#include <sstream>

#include "request.h"
#include "log.h"

namespace lite {

RequestHandler::RequestHandler(const std::string& dir_root):dir_root_(dir_root){}

void RequestHandler::HandleRequest(Request& request,Response& response){
    if(request.method=="GET"){
        DoGet(request,response);
    }else if(request.method=="POST"){
        DoPost(request,response);
    }else{
        response=Response::BuildResponse(Response::not_implemented);
    }
}

void RequestHandler::DoGet(Request& request,Response& response){
    std::string path;
    std::string cgiargs;
    bool is_static=ParseUri(request.uri,path,cgiargs);
    std::string full_path = dir_root_ + path;
    
    std::size_t last_slash_pos = path.find_last_of("/");
    std::size_t last_dot_pos = path.find_last_of(".");
    std::string extension;
    if(last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos){
        extension = path.substr(last_dot_pos + 1);
    }
    
    response.file_fd=open(full_path.c_str(),O_RDONLY);
    if(response.file_fd<0){
        response=Response::BuildResponse(Response::not_found);
        return;
    }

    if(!is_static){
        //todo
        return;
    }
    struct stat statbuf;
    if(stat(full_path.c_str(),&statbuf)==-1){
        LogError("get file state information failed\n");
    }
    response.file_size=statbuf.st_size;
    
    response.status=Response::ok;
    char buffer[512];
    response.contents.clear();
    
    response.headers.resize(2);
    response.headers[0].name="Content-Length";
    response.headers[0].value=std::to_string(statbuf.st_size);
    response.headers[1].name="Content-Type";
    response.headers[1].value=Response::ExtensionToType(extension);
}

void RequestHandler::DoPost(Request& request,Response& response){
    //todo
    response=Response::BuildResponse(Response::not_implemented);
}

bool RequestHandler::ParseUri(const std::string &uri, std::string &path, std::string& cgiargs){
    path.clear();
    path.reserve(uri.size());
    bool is_static=true;
    if(uri.find("cgi-bin")!=std::string::npos){
        is_static=false;
    }
    for (std::size_t i = 0; i < uri.size(); ++i){
        if (uri[i] == '%'){
            if (i + 3 <= uri.size()){
                int value = 0;
                std::istringstream is(uri.substr(i + 1, 2));
                if (is >> std::hex >> value){
                    path += static_cast<char>(value);
                    i += 2;
                }else{
                    return false;
                }
            }else{
                return false;
            }
        }else if (uri[i] == '+'){
            path += ' ';
        }else if (uri[i] == '?'){
            cgiargs=uri.substr(i+1);
            break;
        }else{
            path += uri[i];
        }
    }
    if(path[path.length()-1]=='/'){
        path+="index.html";
    }
    return is_static;
}

}
