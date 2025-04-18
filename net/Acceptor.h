#pragma once
#include "EventDispatcher.h"


#include "EventLoop.h"
#include <functional>
#include <string>

using AcceptCallBack = std::function<void(int clientfd)>;

class Acceptor  final : public EventDispatcher
{
public:
    Acceptor(EventLoop* m_pEventLoop);
    ~Acceptor();


    void setAcceptCallBack(AcceptCallBack&& callBack);


public:
    bool startListen(const std::string& ip, int port);

    virtual void onRead() override;
    virtual void onWrite() override;
    virtual void onClose() override;

    virtual void enableRead(bool isEnabled) override;
    virtual void enableWrite(bool isEnabled) override;



private:
    int         m_listenfd;

    EventLoop* m_pEventLoop;
    AcceptCallBack  m_acceptCallBack;
};

