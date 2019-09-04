#include "lite_tcp.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "log.h"

namespace lite {

TcpServer::TcpServer(const std::string& address,const std::string& port,
    const std::string& dir_root):
    fd_(-1),address_(address),port_(port),request_handler_(dir_root),
    thread_pool_(4,10000){}

void TcpServer::Run(){
    Init();
    for(;;){
        int nfds=lite_event_.Wait(-1);
        if(nfds==-1){
            exit(EXIT_FAILURE);
        }
        for(int i=0;i<nfds;++i){
            int sock_fd=lite_event_.events()[i].data.fd;
            if(sock_fd==fd_){
                Accept();
            }else{
                //LogInfo("request from %d\n",sock_fd);
                thread_pool_.Submit(std::bind(&Connection::Handle,connection_manager_.GetConnection(sock_fd)));
            }
        }
    }
}

bool TcpServer::SetNonBlock(int fd){
    int flags=fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        LogError("fcntl(F_GETFL): %s", strerror(errno));
        return false;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        LogError("fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return false;
    }
    return true;
}

void TcpServer::Init(){
    if(lite_event_.Init()<0){
        exit(EXIT_FAILURE);
    }
    
    int sockfd=-1;
    int yes=1; //reuse address
    int rv;
    struct addrinfo hints,*servinfo,*p;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, port_.c_str(), &hints, &servinfo)) != 0) {
        LogError("getaddrinfo: %s\n", gai_strerror(rv));
        exit(EXIT_FAILURE);
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            LogError("server: socket\n");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            close(sockfd);
            LogError("server: setsockopt\n");
            exit(EXIT_FAILURE);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            LogError("server: bind\n");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);
    if(p==NULL){
        LogError("server: failed to bind\n");
        exit(EXIT_FAILURE);
    }

    fd_=sockfd;
    if(!SetNonBlock(fd_)){
        exit(EXIT_FAILURE);
    }
    lite_event_.AddEvent(fd_,EPOLLIN | EPOLLET);

    if(listen(fd_,TcpServer::BACKLOG)==-1){
        LogError("server: failed to listen\n");
        exit(EXIT_FAILURE);
    }

    LogInfo("server start \n");
    std::cout<<"server: waiting for connections..."<<std::endl;
}

void TcpServer::Accept(){
    for(;;){
        struct sockaddr_storage their_addr;
        socklen_t sin_size=sizeof(their_addr);
        int client=accept(fd_, (struct sockaddr *)&their_addr, &sin_size);
        if(client==-1){
            if(errno==EAGAIN || errno==EWOULDBLOCK){
                //LogInfo("Accept over\n");
                break;
            }
            LogError("Server: accept %s\n",strerror(errno));       
        }

        std::string client_addr;
        int port;
        char ip[INET_ADDRSTRLEN]={0};
        if(their_addr.ss_family == AF_INET){
            struct sockaddr_in *s = (struct sockaddr_in *)&their_addr;
            inet_ntop(AF_INET,(void*)&(s->sin_addr),ip,sizeof(ip));
            port = ntohs(s->sin_port);
        }else{
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&their_addr;
            inet_ntop(AF_INET6,(void*)&(s->sin6_addr),ip,sizeof(ip));
            port = ntohs(s->sin6_port);
        }
        client_addr=ip;
    
        std::string log_msg="Accept a connection from "+client_addr+":"+std::to_string(port);
        log_msg=log_msg+", fileno is "+std::to_string(client);
        //LogInfo(log_msg.c_str());

        if(!SetNonBlock(client)){
            continue;
        }

        lite_event_.AddEvent(client,EPOLLIN | EPOLLET | EPOLLONESHOT);
        
        ConnectionPtr conptr=std::make_shared<Connection>(client,request_handler_,lite_event_,
            connection_manager_);        
        connection_manager_.AddConnection(client,conptr);
    }
}

}
