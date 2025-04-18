#include "Poll.h"

#include "common.h"

void Poll::poll(int timeoutUs, std::vector<EventDispatcher*>& triggerdEventDispatchers)
{
#ifndef _WIN32
    /*  int n = ::poll(&m_fds[0], m_fds.size(),
    );

      for (int i = 0; i < n; i++)
      {

      }*/


#endif // !_WIN32

}

void Poll::registerReadEvent(int fd, EventDispatcher* dispatcher)
{
#ifndef _WIN32
    auto iter = m_fdEventMap.find(fd);
    if (iter == m_fdEventMap.end())
    {
        struct pollfd newfd;
        newfd.fd = fd;
        newfd.events = POLLIN;
        newfd.revents = 0;

        std::get<0>(*iter->second) = newfd;
        std::get<1>(*iter->second) = dispatcher;
    }
    else
    {
        std::get<0>(*iter->second).events |= POLLIN;
    }



#endif // !_WIN32

}

void Poll::registerWriteEvent(int fd, EventDispatcher* dispatcher)
{
#ifndef _WIN32
    auto iter = m_fdEventMap.find(fd);
    if (iter == m_fdEventMap.end())
    {
        struct pollfd newfd;
        newfd.fd = fd;
        newfd.events = POLLOUT;
        newfd.revents = 0;

        std::get<0>(*iter->second) = newfd;
        std::get<1>(*iter->second) = dispatcher;

    }
    else
    {
        std::get<0>(*iter->second).events |= POLLOUT;
    }



#endif // !_WIN32
}

void Poll::unRegisterReadEvent(int fd, EventDispatcher* dispatcher)
{
#ifndef _WIN32
    auto iter = m_fdEventMap.find(fd);
    if (iter == m_fdEventMap.end())
    {
        return;
    }
    else
    {

        std::get<0>(*iter->second).events &= ~POLLIN;
    }



#endif // !_WIN32
}

void Poll::unRegisterWriteEvent(int fd, EventDispatcher* dispatcher)
{
#ifndef _WIN32
    auto iter = m_fdEventMap.find(fd);
    if (iter == m_fdEventMap.end())
    {
        return;
    }
    else
    {
        std::get<0>(*iter->second).events &= ~POLLIN;
    }



#endif // !_WIN32
}

void Poll::unRegisterAllEvent(int fd, EventDispatcher* dispatcher)
{
}
