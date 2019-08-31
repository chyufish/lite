#ifndef CONNECTION_H
#define CONNECTION_H

#include <array>
#include <memory>

#include "lite_event.h"
#include "request_handler.h"
#include "http_parser.h"
#include "buffer.h"

namespace lite{
class ConnectionManager;

class Connection{
public:
    enum ConnectionState{READING,WRITING,SUCCESS,ERROR,CLOSED};

    Connection(const Connection&)=delete;
    Connection& operator=(const Connection&)=delete;

    Connection(int fd,RequestHandler& request_handler,LiteEvent& lite_event,
        ConnectionManager& connection_manager);
    ~Connection();
   
    void Handle();
    void stop();

    int Fd(){return fd_;}
    ConnectionState State(){return state_;}

private:
    ConnectionState state_;
    int fd_;
    bool keep_alive_;

    Buffer read_buffer_;
    Buffer write_buffer_;

    RequestHandler& request_handler_;
    HttpParser http_parser_;

    LiteEvent& lite_event_;
    ConnectionManager& connection_manager_;

    Request request_;
    Response response_;

    bool Read();
    void HandleWrite();
    void HandleRead();

};

typedef std::shared_ptr<Connection> ConnectionPtr;

}
#endif