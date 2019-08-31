#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include <map>

#include "connection.h"

namespace lite{

class ConnectionManager{
public:
    ConnectionManager(const ConnectionManager&)=delete;
    ConnectionManager& operator=(const ConnectionManager&)=delete;

    ConnectionManager()=default;

    void AddConnection(int fd,ConnectionPtr conptr){
        connections_[fd]=conptr;
    }
    void DelConnection(int fd){
        connections_[fd]->stop();
        connections_.erase(fd);
    }
    ConnectionPtr GetConnection(int fd){
        return connections_[fd];
    }
private:
    std::map<int,ConnectionPtr> connections_;
};

}

#endif