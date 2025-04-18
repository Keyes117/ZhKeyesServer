#pragma once
/*
* zhkeyes 2025/4/6 remake
* IO���ú�����װ�ӿ���
*
*/
#include <vector>

#include "EventDispatcher.h"

class IOMultiplex
{
public:
    virtual ~IOMultiplex() = default;
    /**
     * @brief ����IO����ģ�� ����ʱ��
     * @param timeoutUs ��ʱ�¼�
     * @param triggerdEventDispatcher ÿһ��EventLoop�лᶨ��һ��Dispatcher����
     */
    virtual void poll(int  timeoutUs, std::vector<EventDispatcher*>& triggerdEventDispatcher) = 0;


    virtual void registerReadEvent(int fd, EventDispatcher* dispatcher) = 0;
    virtual void registerWriteEvent(int fd, EventDispatcher* dispatcher) = 0;

    virtual void unRegisterReadEvent(int fd, EventDispatcher* dispatcher) = 0;
    virtual void unRegisterWriteEvent(int fd, EventDispatcher* dispatcher) = 0;

    virtual void unRegisterAllEvent(int fd, EventDispatcher* dispatcher) = 0;
};