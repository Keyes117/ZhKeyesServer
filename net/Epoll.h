/**
 * @brief Epoll 2025/4/6
 */

#ifndef NET_EPOLL_H_
#define NET_EPOLL_H_


#include  "IOMultiplex.h"


#ifndef _WIN32

#include <sys/epoll.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/unistd.h>

#include <map>



class Epoll : public IOMultiplex
{
public:
    Epoll();
    virtual ~Epoll() = default;


public:
    virtual void poll(int  timeoutUs, std::vector<EventDispatcher*>& triggerdEventDispatcher) override;


    virtual void registerReadEvent(int fd, EventDispatcher* dispatcher) override;
    virtual void registerWriteEvent(int fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterReadEvent(int fd, EventDispatcher* dispatcher) override;
    virtual void unRegisterWriteEvent(int fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterAllEvent(int fd, EventDispatcher* dispatcher) override;
private:

    int                     m_epollfd;
    //key=>fd, value=eventflag
    std::map<int, int32_t>  m_fdEventFlag;

private:
    Epoll(const Epoll& rhs) = delete;
    Epoll& operator=(const Epoll& rhs) = delete;

    Epoll(Epoll&& rhs) = delete;
    Epoll& operator=(Epoll&& rhs) = delete;
};


#endif // !_WIN32
#endif // !NET_EPOLL_H_
