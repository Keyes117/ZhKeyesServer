#ifndef NET_SELECT_H_
#define NET_SELECT_H_
#include "IOMultiplex.h"

/**
 * @zhkeyes 2025/4/6
 * select ·â×°Àà
 */

 /**
  *   int select(int nfds, fd_set *readfds, fd_set *writefds,
  *               fd_set *exceptfds, struct timeval *timeout);
  */

#ifdef _WIN32
#else
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#endif // _WIN32


class Select :public IOMultiplex
{
public:
    Select() = default;
    virtual ~Select() = default;

    virtual void poll(int  timeoutUs, std::vector<EventDispatcher*>& triggerdEventDispatchers) override;

    virtual void registerReadEvent(int fd, EventDispatcher* dispatcher) override;
    virtual void registerWriteEvent(int fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterReadEvent(int fd, EventDispatcher* dispatcher) override;
    virtual  void unRegisterWriteEvent(int fd, EventDispatcher* dispatcher) override;


    virtual void unRegisterAllEvent(int fd, EventDispatcher* dispatcher) override;
private:
#ifdef _WIN32
#else
    int     m_maxfd;
    fd_set  m_readfds;
    fd_set  m_writefds;
    fd_set  m_exceptfds;

    std::unordered_map<int, EventDispatcher*> m_EventMap;

#endif // _WIN32


};

#endif