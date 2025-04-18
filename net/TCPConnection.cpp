#include "TCPConnection.h"
#ifdef _WIN32

#else
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif // _WIN32

#include <iostream>
#include <functional>

TCPConnection::TCPConnection(int fd, const std::shared_ptr<EventLoop>& spEventLoop)
    :m_fd(fd), m_spEventLoop(spEventLoop)
{
    std::cout << "TCPConnection::ctor" << m_fd << std::endl;
}

TCPConnection::~TCPConnection()
{
#ifdef _WIN32
#else
    ::close(m_fd);
#endif // _WIN32

    std::cout << "TCPConnection::dtor" << m_fd << std::endl;
}


void TCPConnection::startRead()
{
    m_spEventLoop->registerReadEvent(m_fd, this);
}

bool TCPConnection::send(const char* buf, size_t bufLen)
{
    return send(std::string(buf, bufLen));
}

bool TCPConnection::send(const std::string& buf)
{
    //当前线程和TCPConnection在一个线程中，则直接发送， 否则 先交给对应的TcpConnection
    if (isCallableInOwnerThread())
    {
        std::cout << "TCPConnection::send " << buf.length() << "bytes"
            << ", fd: " << m_fd <<
            ",threadID: " << m_spEventLoop->getThreadID() << std::endl;
        return sendInterval(buf.c_str(), buf.length());
    }
    else
    {
        //这里std::bind 绑定的参数 是不定参数， 但是send方法有两种形式，因此编译器无法判定是哪一个send
        //m_spEventLoop->RegisterCustomTask(std::bind(&TCPConnection::send,this,buf)
        m_spEventLoop->RegisterCustomTask(std::bind(static_cast<bool(TCPConnection::*)(const std::string&)>(&TCPConnection::send), this, buf));
        return true;
    }
}

void TCPConnection::onRead()
{
    if (!m_enableRead)
        return;
    //收数据
#ifdef _WIN32

#else
    char buf[1024];
    int n = ::recv(m_fd, buf, sizeof(buf), 0);
    if (n == 0)
    {
        //对端关闭了连接
        onClose();
    }
    else if (n < 0)
    {
        if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
        {
            return;
        }

        //真的出错了，关闭连接
        onClose();
    }
    else
    {
        //正常收数据
        m_recvBuf.append(buf, n);

        //Calc24Session::onRead
        m_readCallBack(m_recvBuf);

    }


    //解包

#endif // _WIN32
}

void TCPConnection::onWrite()
{

    if (!m_enableWrite)
        return;

#ifdef _WIN32
#else
    while (true)
    {
        int n = ::send(m_fd, m_sendBuf, m_sendBuf.remaining(), 0);

        if (n == 0)
        {
            //对端关闭了连接
            onClose();
            return;
        }
        else if (n < 0)
        {
            if (errno == EINTR)
            {
                return;
            }
            else if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                //由于当前TCP窗口太小， 数据发布出去了
                m_writeCallBack();

                registerWriteEvent();

                return;
            }

            //真出错了
            onClose();
            return;
        }

        //发送成功了
        m_sendBuf.erase(n);
        if (m_sendBuf.isEmpty())
            return;
    }
#endif // _WIN32
}

void TCPConnection::onClose()
{

    //m_closeCallBack(this->shared_from_this());
    unregisterAllEvent();
    //Session::OnClose => Calc24Server::onDisConnected => 从map中删除Session
    //->析构 TCPConnection => Session => 关闭Connection fd
    //因此要注销 读写事件
    m_closeCallBack();

}

void TCPConnection::enableRead(bool isEnabled)
{
    m_enableRead = isEnabled;
}

void TCPConnection::enableWrite(bool isEnabled)
{
    m_enableWrite = isEnabled;
}

int64_t TCPConnection::registerTimer(int32_t intervalMs, int64_t repeatCount, TimerTask task)
{
    return m_spEventLoop->addTimer(intervalMs, repeatCount, task);
}

void TCPConnection::unregisterTimer(int64_t timerId)
{
    m_spEventLoop->removeTimer(timerId);
}

bool TCPConnection::sendInterval(const char* buf, size_t bufLen)
{
#ifdef _WIN32
#else
    m_sendBuf.append(buf, bufLen);

    while (true)
    {
        int n = ::send(m_fd, m_sendBuf, m_sendBuf.remaining(), 0);

        if (n == 0)
        {
            //对端关闭了连接
            onClose();
            return false;
        }
        else if (n < 0)
        {
            if (errno == EINTR)
            {
                return true;
            }
            else if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                //由于当前TCP窗口太小， 数据发布出去了
                m_writeCallBack();
                registerWriteEvent();
                return true;
            }

            //真出错了
            onClose();
            return false;
        }

        //发送成功了
        m_sendBuf.erase(n);
        if (m_sendBuf.isEmpty())
            return true;
    }

#endif // _WIN32
}

bool TCPConnection::isCallableInOwnerThread()
{
    return std::this_thread::get_id() == m_spEventLoop->getThreadID();
}

void TCPConnection::setReadCallBack(ReadCallBack&& readCallBack)
{
    m_readCallBack = std::move(readCallBack);
}

void TCPConnection::setWriteCallBack(WriteCallBack&& writeCallBack)
{
    m_writeCallBack = std::move(writeCallBack);
}

void TCPConnection::setCloseCallBack(CloseCallBack&& closeCallBack)
{
    m_closeCallBack = std::move(closeCallBack);
}

void TCPConnection::registerReadEvent()
{
    if (m_registerReadEvent)
        return;

    m_spEventLoop->registerReadEvent(m_fd, this);
}

void TCPConnection::registerWriteEvent()
{
    if (m_registerWriteEvent)
        return;

    //向IO复用函数注册写事件
    m_spEventLoop->registerWriteEvent(m_fd, this);
}

void TCPConnection::unregisterReadEvent()
{
    if (!m_registerReadEvent)
        return;
    //向IO复用函数反注册写事件
    m_spEventLoop->unRegisterReadEvent(m_fd, this);
    m_registerReadEvent = false;
}

void TCPConnection::unregisterWriteEvent()
{
    if (!m_registerWriteEvent)
        return;
    //向IO复用函数反注册写事件
    m_spEventLoop->unRegisterWriteEvent(m_fd, this);
    m_registerWriteEvent = false;

}

void TCPConnection::unregisterAllEvent()
{
    //向IO复用函数反注册所有事件
    m_spEventLoop->unRegisterAllEvent(m_fd, this);
}
