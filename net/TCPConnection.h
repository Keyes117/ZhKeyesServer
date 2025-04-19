/**
 * @brief TCPConenction 连接类
 */
#ifndef NET_TCPCONNECTION_H_
#define NET_TCPCONNECTION_H_
#include "EventDispatcher.h"

#include <functional>
#include <memory>

#include "ByteBuffer.h"
#include "EventLoop.h"


class TCPConnection;

//readcallBack 如果返回False，则业务逻辑认为解包出错
using ReadCallBack = std::function<void(ByteBuffer&)>;
using WriteCallBack = std::function<void()>;
//using CloseCallBack = std::function<void(const std::shared_ptr<TCPConnection>&)>;
using CloseCallBack = std::function<void()>;


//CRTP
class TCPConnection : public EventDispatcher, public std::enable_shared_from_this<TCPConnection>
{
public:
    TCPConnection(int fd, const std::shared_ptr<EventLoop>& spEventLoop);
    virtual ~TCPConnection();

    //TODO:

public:

    void startRead();

    bool send(const char* buf, size_t bufLen);
    bool send(const std::string& buf);



public:
    virtual void onRead() override;
    virtual void onWrite() override;
    virtual void onClose() override;

    virtual void enableRead(bool isEnabled) override;
    virtual void enableWrite(bool isEnabled) override;

    int64_t registerTimer(int32_t intervalMs, int64_t repeatCount, TimerTask task);
    void unregisterTimer(int64_t timerId);


    void setReadCallBack(ReadCallBack&& readCallBack);
    void setWriteCallBack(WriteCallBack&& writeCallBack);
    void setCloseCallBack(CloseCallBack&& closeCallBack);
private:

    bool sendInterval(const char* buf, size_t bufLen);

    bool isCallableInOwnerThread();

    void registerReadEvent();
    void registerWriteEvent();
    void unregisterReadEvent();
    void unregisterWriteEvent();

    void unregisterAllEvent();

private:
    int             m_fd;
    bool            m_registerReadEvent{ false };
    bool            m_registerWriteEvent{ false };

    bool            m_enableRead{ false };
    bool            m_enableWrite{ false };


    ByteBuffer      m_recvBuf;
    ByteBuffer      m_sendBuf;

    ReadCallBack    m_readCallBack;
    WriteCallBack   m_writeCallBack;
    CloseCallBack   m_closeCallBack;

    std::shared_ptr<EventLoop>  m_spEventLoop;
};

#endif //!NET_TCPCONNECTION_H_