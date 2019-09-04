#include "lite_event.h"

#include <sys/epoll.h>
#include <string.h>

#include "log.h"

namespace lite{

int LiteEvent::Init(){
    epfd_=epoll_create1(0);
    if(epfd_==-1){
        LogError("epoll_create1 error %s",strerror(errno));
    }
    return epfd_;
}

bool LiteEvent::AddEvent(int fd,int events){
    struct epoll_event ev;
    ev.data.fd=fd;
    ev.events=events;
    if(epoll_ctl(epfd_,EPOLL_CTL_ADD,fd,&ev)==-1){
        LogError("epoll_ctl error %s",strerror(errno));
        return false;
    }
    return true;
}

bool LiteEvent::ModEvent(int fd,int events){
    struct epoll_event ev;
    ev.data.fd=fd;
    ev.events=events;
    if(epoll_ctl(epfd_,EPOLL_CTL_MOD,fd,&ev)==-1){
        LogError("epoll_ctl error %s",strerror(errno));
        return false;
    }
    return true;
}

bool LiteEvent::RemoveFd(int fd){
    if(epoll_ctl(epfd_,EPOLL_CTL_DEL,fd,NULL)==-1){
        LogError("epoll_ctl error %s",strerror(errno));
        return false;
    }
    return true;
}

int LiteEvent::Wait(int timeout){
    int nfds=epoll_wait(epfd_,ev_,kMaxEvents,timeout);
    if(nfds==-1){
        LogError("epoll_wait error %s",strerror(errno));
        return -1;
    }
    return nfds;
}

}