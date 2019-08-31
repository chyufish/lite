#include "connection.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/sendfile.h>

#include <iostream>

#include "connection_manager.h"
#include "log.h"

namespace lite{

Connection::Connection(int fd,RequestHandler& request_handler,LiteEvent& lite_event,
    ConnectionManager& connection_manager):fd_(fd),request_handler_(request_handler),lite_event_(lite_event),
    connection_manager_(connection_manager),state_(READING){}

Connection::~Connection(){
    //LogInfo("release connection %d\n",fd_);
}

void Connection::Handle(){
    switch(state_){
        case READING:
            HandleRead();
            break;
        case WRITING:
            HandleWrite();
            break;
        default:
	        return;
    }
    LogInfo("Connection %d : state %d\n",fd_,state_);
    switch(state_){
        case READING:
            lite_event_.ModEvent(fd_,EPOLLIN | EPOLLET | EPOLLONESHOT);
	        break;
        case WRITING:
            lite_event_.ModEvent(fd_,EPOLLOUT | EPOLLET | EPOLLONESHOT);
	        break;
        default:
            lite_event_.RemoveFd(fd_);
	        connection_manager_.DelConnection(fd_);
	        break;
    }
}

void Connection::HandleRead(){
    if(!Read()){
        return;
    }
    HttpParser::result_type result;
    const char* parse_end;
    std::tie(result, parse_end)=http_parser_.Parse(request_,read_buffer_.ReadBegin(),
        read_buffer_.ReadBegin()+read_buffer_.ReadableBytes());
    //LogInfo("%s\n",read_buffer_.buffer_.data());

    if(result==HttpParser::indeterminate){
        return;
    }else if(result==HttpParser::bad){
        response_=Response::BuildResponse(Response::bad_request);
    }else{
        request_handler_.HandleRequest(request_,response_);

        for(int i=0;i<request_.headers.size();++i){
            if(request_.headers[i].name=="Connection"){
                if(request_.headers[i].value=="keep-alive"){
                    keep_alive_=true;
                    response_.headers.push_back({"Connection","keep-alive"});
                }else{
                    keep_alive_=false;
                }
            }
        }
    }
    http_parser_.Reset();
    read_buffer_.Reset();
    request_.clear();
    response_.WriteToBuffer(write_buffer_);
    state_=WRITING;
    HandleWrite();
}

bool Connection::Read(){
    ssize_t nbytes;
    for(;;){
        nbytes=recv(fd_,read_buffer_.WriteBegin(),read_buffer_.WriteableBytes(),0);

        if(nbytes==-1){
            if(errno==EAGAIN || errno==EWOULDBLOCK){
                break;
            }else if(errno==EINTR){
                continue;
            }else{
                LogError("Conncetion %d : recv error %s\n",fd_,strerror(errno));
                state_=ERROR;
                return false;
            }
        }else if(nbytes==0){
            state_=CLOSED;
            return false;
        }else{
            read_buffer_.HasWritten(nbytes);
        }
    }
    return true;
}

void Connection::HandleWrite(){
    ssize_t nsend=0;
    while(write_buffer_.ReadableBytes()>0){
        nsend=send(fd_,write_buffer_.ReadBegin(),write_buffer_.ReadableBytes(),0);

        if(nsend<0){
            if(errno==EINTR){
                continue;
            }else if(errno==EAGAIN || errno==EWOULDBLOCK){
                return;
            }else{
                LogError("Conncetion %d : send error %s\n",fd_,strerror(errno));
                state_=ERROR;
                return;
            }
        }else{
            write_buffer_.HasRead(nsend);
        }
    }
    write_buffer_.Reset();

    if(response_.file_fd!=-1){
        off_t offset=0;
        for(;;){
            ssize_t nsend=sendfile(fd_,response_.file_fd,&offset,response_.file_size-offset);
            if(nsend<0){
                if(errno==EINTR){
                    continue;
                }else if(errno==EAGAIN || errno==EWOULDBLOCK){
                    return;
                }else{
                    LogError("Conncetion %d : sendfile error %s\n",fd_,strerror(errno));
                    state_=ERROR;
                    return;
                }
            }
            if(response_.file_size-offset==0){
                close(response_.file_fd);
                response_.file_fd=-1;
                break;
            }
        }
    }

    if(keep_alive_){
        state_=READING;
    }else{
        state_=SUCCESS;
    }
}

void Connection::stop(){
    close(fd_);
}

}
