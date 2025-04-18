#pragma once

#include <cstdint>
#include <string>

#include "Acceptor.h"
#include "EventLoop.h"
#include "TCPConnection.h"
#include "ThreadPool.h"
#include <map>


using ConnectedCallBack = std::function<void(std::shared_ptr<TCPConnection>& spConn)>;
using DisConnectedCallBack = std::function<void(const std::shared_ptr<TCPConnection>& spConn)>;

class TCPServer
{
public:
    TCPServer();
    ~TCPServer() = default;

    bool init(int32_t threadNum, const std::string& ip, int port);
    void start();
    void close();

    void setConnectionCallBack(ConnectedCallBack&& callback)
    {
        m_connectionCallBack = std::move(callback);
    }
    void setDisConnectionCallBack(DisConnectedCallBack&& callback)
    {
        m_disconnectionCallBack = std::move(callback);
    }


    EventLoop& getBaseEventLoop();


    void onConnected(std::shared_ptr<TCPConnection>& spConn);
    void onDisConnected(std::shared_ptr<TCPConnection>& spConn);

private:
    void onAccept(int clientfd);



private:
    std::string                 m_ip;
    uint16_t                    m_port;

    ThreadPool                  m_threadPool;
    EventLoop                   m_eventLoop;
    std::unique_ptr<Acceptor>   m_spAcceptor;

    std::map<int, std::shared_ptr<TCPConnection>>    m_connections;

    ConnectedCallBack           m_connectionCallBack;
    DisConnectedCallBack        m_disconnectionCallBack;

};


