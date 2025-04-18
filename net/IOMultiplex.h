#pragma once
/*
* zhkeyes 2025/4/6 remake
* IO复用函数包装接口类
*
*/
#include <vector>

#include "EventDispatcher.h"

class IOMultiplex
{
public:
    virtual ~IOMultiplex() = default;
    /**
     * @brief 调用IO复用模型 处理时间
     * @param timeoutUs 超时事件
     * @param triggerdEventDispatcher 每一个EventLoop中会定义一个Dispatcher对象
     */
    virtual void poll(int  timeoutUs, std::vector<EventDispatcher*>& triggerdEventDispatcher) = 0;


    virtual void registerReadEvent(int fd, EventDispatcher* dispatcher) = 0;
    virtual void registerWriteEvent(int fd, EventDispatcher* dispatcher) = 0;

    virtual void unRegisterReadEvent(int fd, EventDispatcher* dispatcher) = 0;
    virtual void unRegisterWriteEvent(int fd, EventDispatcher* dispatcher) = 0;

    virtual void unRegisterAllEvent(int fd, EventDispatcher* dispatcher) = 0;
};