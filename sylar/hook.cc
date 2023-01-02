#include "hook.h"
#include "fiber.h"
#include "iomanager.h"
#include "fd_manager.h"
#include "log.h"

#include <dlfcn.h>

sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

namespace sylar{

static thread_local bool t_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep) \
    XX(usleep) \
    XX(nanosleep) \
    XX(socket) \
    XX(connect) \
    XX(accept) \
    XX(read) \
    XX(readv) \
    XX(recv) \
    XX(recvfrom) \
    XX(recvmsg) \
    XX(write) \
    XX(writev) \
    XX(send) \
    XX(sendto) \
    XX(sendmsg) \
    XX(close) \
    XX(fcntl) \
    XX(ioctl) \
    XX(getsockopt) \
    XX(setsockopt)

void hook_init() {
    static bool is_inited = false;
    if(is_inited){
        return;
    }

#define XX(name) name ## _f = (name ## _fun)dlsym(RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
}

struct _HookIniter{
    _HookIniter(){
        hook_init();
    }
};

static _HookIniter s_hook_initer;


bool is_hook_enable(){
    return t_hook_enable;
}

void set_hook_enable(bool flag){
    t_hook_enable = flag;
}

}

struct timer_info{
    int cancelled = 0;
};

template<typename OriginFun, typename ... Args >
static ssize_t do_io(int fd, OriginFun fun, const char* hook_fun_name,
uint32_t event, int timeout_so, Args&&... args){
    if (!sylar::t_hook_enable){
        return fun(fd,std::forward<Args>(args)...);
    }

    sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);
    if (!ctx){
        return fun(fd,std::forward<Args>(args)...);
    }

    if (ctx->isClose()){
        errno = EBADF;
        return -1;
    }

    if (!ctx->isSocket() || ctx->getUserNonblock()){
        return fun(fd,std::forward<Args>(args)...);
    }

    uint64_t to = ctx->getTimeout(timeout_so);
    std::shared_ptr<timer_info> tinfo(new timer_info);

retry:
    ssize_t n = fun(fd,std::forward<Args>(args)...);
    while(n==-1 && errno == EINTR){
        n = fun(fd,std::forward<Args>(args)...);
    }
    
    if (n==-1 && errno == EAGAIN){
        sylar::IOManager* iom = sylar::IOManager::GetThis();
        sylar::Timer::ptr timer;
        std::weak_ptr<timer_info> winfo(tinfo);

        if (to != (uint64_t)-1){
            timer = iom->addConditionTimer(to,[winfo,fd,iom,event](){
                 auto t = winfo.lock();
                 if (!t || t->cancelled){
                    return;
                 }
                 t->cancelled = ETIMEDOUT;
                 iom->cancelEvent(fd,(sylar::IOManager::Event)(event));
            },winfo);
        }
    

        int rt = iom->addEvent(fd,(sylar::IOManager::Event)(event));
        if (rt){
            SYLAR_LOG_ERROR(g_logger)<<hook_fun_name<<"addEvent("<<fd<<", "<<event<<")";
            if (timer){
                timer->cancel();
            }
            return -1;
        }
        else{
            sylar::Fiber::YieldToHold();
            if (timer){
                timer->cancel();
            }
            if (tinfo->cancelled){
                errno = tinfo->cancelled;
                return -1;
            }

            goto retry;
        }
    }
    return n;
}

extern "C"{
#define XX(name) name ## _fun name ## _f = nullptr;
    HOOK_FUN(XX);
#undef XX


unsigned int sleep(unsigned int seconds){
    if(!sylar::t_hook_enable){
        return sleep_f(seconds);
    }
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    // iom->addTimer(seconds * 1000, std::bind(&sylar::IOManager::schedule,iom,fiber));
    iom->addTimer(seconds*1000, std::bind((void(sylar::Scheduler::*)(sylar::Fiber::ptr,int thread))&sylar::IOManager::schedule,iom,fiber,-1));
    sylar::Fiber::YieldToHold();
    return 0;
}

int usleep(useconds_t usec){
    if (!sylar::t_hook_enable){
        return usleep_f(usec);
    }
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    // iom->addTimer(usec / 1000, std::bind(&sylar::IOManager::schedule,iom,fiber));
    iom->addTimer(usec / 1000, std::bind((void(sylar::Scheduler::*)(sylar::Fiber::ptr,int thread))&sylar::IOManager::schedule,iom,fiber,-1));
    sylar::Fiber::YieldToHold();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem){
    if (!sylar::t_hook_enable){
        return nanosleep_f(req,rem);
    }
    
    int timeout_ms = req->tv_sec * 1000 + req->tv_nsec / 1000 / 1000;
    sylar::Fiber::ptr fiber = sylar::Fiber::GetThis();
    sylar::IOManager* iom = sylar::IOManager::GetThis();
    // iom->addTimer(seconds * 1000, std::bind(&sylar::IOManager::schedule,iom,fiber));
    iom->addTimer(timeout_ms, std::bind((void(sylar::Scheduler::*)(sylar::Fiber::ptr,int thread))&sylar::IOManager::schedule,iom,fiber,-1));
    sylar::Fiber::YieldToHold();
    return 0;
}

/*socket*/
int socket(int domain, int type, int protocol){
    if(!sylar::t_hook_enable){
        return socket_f(domain,type,protocol);
    }
    int fd = socket_f(domain,type,protocol);
    if (fd == -1) return fd;
    sylar::FdMgr::GetInstance()->get(fd,true);
    return fd;
}

/*connect*/
int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen){
    return connect_f(sockfd,addr,addrlen);
}

/*accept*/
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen){
    int fd = do_io(sockfd,accept_f,"accept",sylar::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if (fd>=0){
        sylar::FdMgr::GetInstance()->get(fd,true);
    }
    return fd;
}

ssize_t read(int fd, void *buf, size_t count){
    return do_io(fd,read_f,"read",sylar::IOManager::READ,SO_RCVTIMEO,buf,count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt){
    return do_io(fd,read_f,"read",sylar::IOManager::READ,SO_RCVTIMEO, iov,iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags){
    return do_io(sockfd, recv_f, "recv", sylar::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen){
    return do_io(sockfd,recvfrom_f,"recvfrom",sylar::IOManager::READ, SO_RCVTIMEO,buf,len,flags,src_addr,addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags){
    return do_io(sockfd,recvmsg_f,"recvmsg",sylar::IOManager::READ, SO_RCVTIMEO,msg,flags);
}

// write
ssize_t write(int fd, const void *buf, size_t count){
    return do_io(fd,write_f,"write",sylar::IOManager::WRITE, SO_SNDTIMEO,buf,count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt){
    return do_io(fd,writev_f,"write_f",sylar::IOManager::WRITE, SO_SNDTIMEO,iov,iovcnt);
}

/*send*/
ssize_t send(int sockfd, const void *buf, size_t len, int flags){
    return do_io(sockfd,send_f,"send",sylar::IOManager::WRITE, SO_SNDTIMEO,buf,len,flags);
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen){
    return do_io(sockfd,sendto_f,"sendto",sylar::IOManager::WRITE, SO_SNDTIMEO,buf,len,flags,dest_addr,addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags){
    return do_io(sockfd,sendmsg_f,"sendmsg",sylar::IOManager::WRITE, SO_SNDTIMEO,msg,flags);
}

// close
int close(int fd){
    if(!sylar::t_hook_enable){
        return close_f(fd);
    }
    sylar::FdCtx::ptr ctx = sylar::FdMgr::GetInstance()->get(fd);

    if (ctx){
        auto iom = sylar::IOManager::GetThis();
        if(iom){
            iom->cancelAll(fd);
        }
        sylar::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

// socket control
// fcntl
int fcntl(int fd, int cmd, ... /* arg */ ){
    if (!sylar::t_hook_enable){
        return fcntl_f(fd,int cmd,)
    }
}

// ioctl
int ioctl(int d, int request, ...){

}

// socket option
int getsockopt(int sockfd, int level, int optname,
                      void *optval, socklen_t *optlen)
{

}

int setsockopt(int sockfd, int level, int optname,
                    const void *optval, socklen_t optlen)
{
    
}

}

