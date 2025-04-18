#include "TCPServer.h"

#ifdef _WIN32
#else
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<poll.h>
#include<iostream>
#include<string.h>
#include<vector>
#include<errno.h>
#include<iostream>
#endif
#include<functional>
#include <memory>
#include <iostream>

#include "common.h"
#include "TCPConnection.h"


TCPServer::TCPServer()
    :m_eventLoop(true)
{
}

bool TCPServer::init(int32_t threadNum, const std::string& ip, int port)
{

    m_threadPool.start(threadNum);
    m_ip = ip;
    m_port = port;

    m_eventLoop.init();

    m_spAcceptor = std::make_unique<Acceptor>(&m_eventLoop);
    if (!m_spAcceptor->startListen(ip, port))
    {
        return false;
    }

    //TODO: AcceptCallbakc 只接受一个参数, 但是这里穿了两个参数，会有问题么
    m_spAcceptor->setAcceptCallBack(std::bind(&TCPServer::onAccept, this, std::placeholders::_1));

    return true;
    //m_eventLoop.registerReadEvent(m_listenfd, NULL);

#ifdef _WIN32
#else

     //m_eventLoop->registerReadEvent(m_listenfd);
#endif // _WIN32

}

void TCPServer::start()
{
    std::cout << "base EventLoop threadID " << std::this_thread::get_id() << std::endl;
    m_eventLoop.setThreadID(std::this_thread::get_id());
    m_eventLoop.run();
}

void TCPServer::close()
{
    m_threadPool.stop();

}

EventLoop& TCPServer::getBaseEventLoop()
{
    return m_eventLoop;
}

void TCPServer::onConnected(std::shared_ptr<TCPConnection>& spConn)
{
}

void TCPServer::onDisConnected(std::shared_ptr<TCPConnection>& spConn)
{
}

void TCPServer::onAccept(int clientfd)
{
    auto spEventLoop = m_threadPool.getNextEventLoop();
    auto spTCPConnection = std::make_shared<TCPConnection>(clientfd, spEventLoop);

    std::cout << "TCPServer::OnAccept:"
        << clientfd << "threadID:"
        << spEventLoop->getThreadID() << std::endl;

    spTCPConnection->startRead();

    m_connectionCallBack(spTCPConnection);

    //m_connections[clientfd] = std::move(spTCPConnection);

 /*   spEventLoop->registerReadEvent(clientfd, dynamic_cast<EventDispatcher*>(spEventLoop));*/
}


