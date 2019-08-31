#ifndef LITE_TCP_H
#define LITE_TCP_H

#include <cstdint>
#include <string>
#include <map>

#include "lite_event.h"
#include "request_handler.h"
#include "thread_pool.h"
#include "connection_manager.h"

namespace lite{

class TcpServer{
public:
    static const int BACKLOG=10;
    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    TcpServer(const std::string& address,const std::string& port,
        const std::string& dir_root);

    void Run();

private:
    int fd_;
    std::string address_;
    std::string port_;

    ThreadPool thread_pool_;
    LiteEvent lite_event_;
    ConnectionManager connection_manager_;
    RequestHandler request_handler_;

    bool SetNonBlock(int fd);
    void Init();
    void Accept();
};

}
#endif