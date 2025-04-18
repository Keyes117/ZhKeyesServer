#include "Select.h"

void Select::poll(int timeoutUs, std::vector<EventDispatcher*>& triggerdEventDispatchers)
{
#ifdef _WIN32
#else
    struct timeval tv;
    tv.tv_sec = timeoutUs / 1000000;
    tv.tv_usec = timeoutUs - timeoutUs / 1000000 * 1000000;

    int n = ::select(m_maxfd + 1, &m_readfds, &m_writefds, nullptr, &tv);

    //TODO : 遍历m_readfds. 调用FD_ISSET判断那些socket可读
     //TODO : 遍历m_readfds. 调用FD_ISSET判断那些socket可写
    for (int i = 0; i < m_maxfd; i++)
    {
        if ((FD_ISSET(i, &m_readfds) || FD_ISSET(i, &m_writefds)))
            triggerdEventDispatchers.push_back(m_EventMap[i]);

    }

#endif // _WIN32


}

void Select::registerReadEvent(int fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
#else
    if (m_maxfd < fd)
        m_maxfd = fd;

    FD_SET(fd, &m_readfds);
    m_EventMap[fd] = dispatcher;
#endif // _WIN32


}

void Select::registerWriteEvent(int fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
#else
    if (m_maxfd < fd)
        m_maxfd = fd;

    FD_SET(fd, &m_writefds);
    m_EventMap[fd] = dispatcher;
#endif // _WIN32
}

void Select::unRegisterReadEvent(int fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
#else
    if (m_maxfd < fd)
        m_maxfd = fd;

    FD_CLR(fd, &m_readfds);
    m_EventMap.erase(fd);
#endif // _WIN32
}

void Select::unRegisterWriteEvent(int fd, EventDispatcher* dispatcher)
{
#ifdef _WIN32
#else
    if (m_maxfd < fd)
        m_maxfd = fd;

    FD_CLR(fd, &m_writefds);
    m_EventMap.erase(fd);
#endif // _WIN32
}

void Select::unRegisterAllEvent(int fd, EventDispatcher* dispatcher)
{
}
