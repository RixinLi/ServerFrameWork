#include "iomanager.h"
#include "macro.h"
#include "log.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>

namespace sylar{

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

Iomanager::IOManager(size_t threads, bool use_caller, const std::string& name):
Scheduler(threads,use_caller,name)
{
    m_epfd = epoll_create(5000);
    SYLAR_ASSERT(m_epfd > 0);

    int rt = pipe(m_tickleFds);
    SYLAR_ASSERT(rt);

    epoll_event event;
    memset(&event, 0 sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    rt = fcntl(m_tickleFds[0],F_SETFL,O_NONBLOCK);
    SYLAR_ASSERT(rt);

    rt = epoll_ctl(m_epfd,EPOLL_CTL_ADD,m_tickleFds[0],&event);
    SYLAR_ASSERT(rt);

    contextResize(32);

    start();
}

IOManager::~IOManager(){
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);
    for (size_t i=0;i<m_fdCotexts.size();i++){
        if (m_fdCotexts[i]) delete m_fdCotexts[i];
    }
}

// 1 success, 0 retry, -1 error
int IOManager::addEvent(int fd, Event event,std::function<void()> cb){
    FdContext* fd_ctx = nullptr;
    RWMutexType::ReadLock lock(&m_mutex);
    if (m_fdCotexts.size() > fd){
        fd_ctx = m_fdCotexts[fd];
        lock.unlock();
    }
    else{
        lock.unlock();
        RWMutexType::WriteLock lock2(&m_mutex);
        contextResize(fd*1.5);
        fd_ctx = m_fdCotexts[fd];
    }
    FdContext::Mutex::Lock lock2(fd_ctx->mutex);
    if (fd_ctx->events & event){
        SYLAR_LOG_ERROR(g_logger)<<"";
    }
}

void IOManager::contextResize(size_t size){
    m_fdCotexts.resize(size);
    for (size_t i=0;i<size;i++){
        if (!m_fdCotexts[i]){
            m_fdCotexts[i] = new FdContext;
            m_fdCotexts[i]->fd = i;
        }
    }
}


bool IOManager::delEvent(int fd, Event event);

bool IOManager::cancelEvent(int fd, Event event);

bool IOManager::cancelAll(int fd);

IOManager* IOManager::GetThis();

}