#include <iostream>
#include "lite_tcp.h"

int main(int argc,char *argv[]){
    if(argc!=4){
        std::cerr<<"Usage: ./lite_server address port dir_root"<<std::endl;
        return 1;
    }
    lite::TcpServer server(argv[1],argv[2],argv[3]);
    server.Run();
    return 0;
}
