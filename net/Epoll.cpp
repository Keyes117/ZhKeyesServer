#include "Epoll.h"

#ifndef _WIN32
#endif // !_WIN32


#include <string.h>
#include <iostream>
#include "common.h"
#include "util.h"


#ifdef _WIN32

#else

#include <sys/epoll.h>
Epoll::Epoll()
{
    m_epollfd = ::epoll_create1(EPOLL_CLOEXEC);
    if (m_epollfd == INVALID_FD)
        crash();
}

void Epoll::poll(int timeoutUs, std::vector<EventDispatcher*>& triggerdEventDispatcher)
{
#ifndef _WIN32

    struct epoll_event events[1024];
    int timeoutMs = timeoutUs / 1000;

    bool enableRead = false;
    bool enableWrite = false;

    int n = ::epoll_wait(m_epollfd, events, 1024, timeoutMs);
    for (int i = 0; i < n; i++)
    {
        if (events[i].events & EPOLLIN)
            enableRead = true;
        else
            enableRead = false;

        if (events[i].events & EPOLLOUT)
            enableWrite = true;
        else
            enableWrite = false;

        EventDispatcher* dispatcher = static_cast<EventDispatcher*>(events[i].data.ptr);

        dispatcher->enableRead(enableRead);
        dispatcher->enableWrite(enableWrite);

        triggerdEventDispatcher.push_back(dispatcher);
    }

#endif // !_WIN32


}

void Epoll::registerReadEvent(int fd, EventDispatcher* dispatcher)
{
    int32_t eventFlag = 0;

    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
    {
        eventFlag |= EPOLLIN;
        m_fdEventFlag[fd] = eventFlag;
    }
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLIN)
            return;

        eventFlag |= EPOLLIN;
        m_fdEventFlag[fd] = eventFlag;
    }


    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        crash();
    }
}

void Epoll::registerWriteEvent(int fd, EventDispatcher* dispatcher)
{
    int32_t eventFlag = 0;

    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
    {
        eventFlag |= EPOLLOUT;
        m_fdEventFlag[fd] = eventFlag;
    }
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLOUT)
            return;

        eventFlag |= EPOLLOUT;
        m_fdEventFlag[fd] = eventFlag;
    }


    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        crash();
    }
}

void Epoll::unRegisterReadEvent(int fd, EventDispatcher* dispatcher)
{
    int32_t eventFlag = 0;
    int operation;

    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
        return;
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLIN)
            eventFlag &= ~EPOLLIN;

        if (eventFlag == 0)
        {
            m_fdEventFlag.erase(iter);
            operation = EPOLL_CTL_DEL;
        }
        else
        {
            m_fdEventFlag[fd] = eventFlag;
            operation = EPOLL_CTL_MOD;
        }


    }


    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, operation, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        crash();
    }
}

void Epoll::unRegisterWriteEvent(int fd, EventDispatcher* dispatcher)
{

    int32_t eventFlag = 0;
    int operation;
    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
        return;
    else
    {
        eventFlag = iter->second;
        if (eventFlag & EPOLLOUT)
            eventFlag &= ~EPOLLOUT;

        if (eventFlag == 0)
        {
            m_fdEventFlag.erase(iter);
            operation = EPOLL_CTL_DEL;
        }
        else
        {
            m_fdEventFlag[fd] = eventFlag;
            operation = EPOLL_CTL_MOD;
        }
    }

    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, operation, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        crash();
    }
}
void Epoll::unRegisterAllEvent(int fd, EventDispatcher* dispatcher)
{

    int32_t eventFlag = 0;
    int operation;

    auto iter = m_fdEventFlag.find(fd);
    if (iter == m_fdEventFlag.end())
        return;
    else
        m_fdEventFlag.erase(iter);

    struct epoll_event iEvent;
    memset(&iEvent, 0, sizeof(iEvent));
    iEvent.events = eventFlag;

    iEvent.data.ptr = dispatcher;
    if (::epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &iEvent) < 0)
    {
        //TODO:: 打印错误日志
        crash();
    }

    std::cout << "removed fd[" << fd << "] from epoll" << std::endl;
}
#endif // _WIN32