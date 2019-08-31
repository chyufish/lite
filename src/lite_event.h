#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>

namespace lite{

class LiteEvent{
public:
    static const int kMaxEvents=1024;
    LiteEvent(const LiteEvent&)=delete;
    LiteEvent& operator=(const LiteEvent&)=delete;

    LiteEvent()=default;

    int Init();
    int Wait(int timeout);
    bool AddEvent(int fd,int events);
    bool ModEvent(int fd,int events);
    bool RemoveFd(int fd);

    epoll_event* events(){return ev_;}

private:
    int epfd_;
    epoll_event ev_[kMaxEvents];
};

}
#endif