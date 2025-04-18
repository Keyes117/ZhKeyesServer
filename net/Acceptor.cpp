#include "Acceptor.h"


#include <string.h>

#include "common.h"

#ifdef _WIN32
#else
#include "sys/socket.h"
#include "arpa/inet.h"
#include "unistd.h"
#endif // _WIN32


Acceptor::Acceptor(EventLoop* m_pEventLoop) :
    m_pEventLoop(m_pEventLoop)
{
}

Acceptor::~Acceptor()
{
    if (m_listenfd != INVALID_FD)
    {
#ifdef _WIN32
#else
        ::close(m_listenfd);
#endif // _WIN32

        m_listenfd == INVALID_FD;
    }
}

void Acceptor::setAcceptCallBack(AcceptCallBack&& callBack)
{
    m_acceptCallBack = std::move(callBack);
}

bool Acceptor::startListen(const std::string& ip, int port)
{
#ifndef _WIN32
    m_listenfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (m_listenfd == INVALID_FD)
    {
        return false;
    }

    int optval = 1;
    ::setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval));
    ::setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEPORT, (char*)&optval, sizeof(optval));


    struct sockaddr_in  bindaddr;
    bindaddr.sin_family = AF_INET;

    if (ip == "")
        bindaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        bindaddr.sin_addr.s_addr = inet_addr(ip.c_str());
    bindaddr.sin_port = htons(port);

    if (::bind(m_listenfd, (struct sockaddr*)&bindaddr, sizeof(bindaddr)) == -1)
    {
        return false;
    }

    if (::listen(m_listenfd, SOMAXCONN) == -1)
        return false;

    m_pEventLoop->registerReadEvent(m_listenfd, this);

    return true;
#endif
}

void Acceptor::onRead()
{
#ifdef _WIN32
#else
    while (true)
    {
        struct sockaddr_in clientAddr;

        socklen_t clientAddrLen = sizeof(clientAddr);

        int clientfd = ::accept4(m_listenfd, (struct sockaddr*)&clientAddr, &clientAddrLen, SOCK_NONBLOCK);
        if (clientfd > 0)
        {
            //成功接受连接
            m_acceptCallBack(clientfd);

        }
        else if (clientfd == -1)
        {
            if (errno == EWOULDBLOCK)
                //没有连接了
                return;
            else
                //出错了
                return;

        }
    }



#endif // _WIN32


}

void Acceptor::onWrite()
{
}

void Acceptor::onClose()
{
}

void Acceptor::enableRead(bool isEnabled)
{
}

void Acceptor::enableWrite(bool isEnabled)
{
}
