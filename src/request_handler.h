#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include <string>

#include "request.h"

namespace lite {

class RequestHandler{
public:

    RequestHandler(const RequestHandler&)=delete;
    RequestHandler& operator=(const RequestHandler&)=delete;

    explicit RequestHandler(const std::string& dir_root);

    void HandleRequest(Request& request,Response& response);

private:
    std::string dir_root_;

    void DoGet(Request& request,Response& response);
    void DoPost(Request& request,Response& response);
    bool ParseUri(const std::string& uri,std::string& path,std::string& cgiargs);
};

}
#endif