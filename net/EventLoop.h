#pragma once

/**
 * @brief  �¼�ѭ���� EventLoop
 * @author zhkeyes
 */

#ifndef    NET_EVENTLOOP_H_
#define     NET_EVENTLOOP_H_

#include "IOMultiplex.h"

#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "Timer.h"
#include "TimerService.h"
#include "WakeUpEventDispatcher.h"

 //using CustomTask = std::function<bool(const std::string& str)>;
//�Զ�������Ļص�����
using CustomTask = std::function<void()>;

enum class IOMultiplexType
{
    Select,
    Poll,
    Epoll
};

class EventLoop final : public TimerService
{
public:
    EventLoop(bool isBaseLoop = false);
    virtual ~EventLoop();

public:
    bool init(IOMultiplexType type = IOMultiplexType::Epoll);

    void run();

    //��ȡ�߳�ID
    void setThreadID(const std::thread::id& threadID);
    std::thread::id getThreadID() const;

    //��д�¼���ط���
    void registerReadEvent(int fd, EventDispatcher* dispatcher);
    void registerWriteEvent(int fd, EventDispatcher* dispatcher);
    void unRegisterReadEvent(int fd, EventDispatcher* dispatcher);
    void unRegisterWriteEvent(int fd, EventDispatcher* dispatcher);
    void unRegisterAllEvent(int fd, EventDispatcher* dispatcher);

    //ע�ỽ���¼�����
    void RegisterCustomTask(CustomTask&& task);


    // ��ʱ��ҵ��
    virtual int64_t addTimer(int32_t intervalMs, int64_t repeatCount, TimerTask task, TimerMode mode = TimerMode::TimerModeFixedInterval) override;
    virtual void  removeTimer(int64_t timerID) override;

private:
    //�����¼�FD
    bool createWakeUpfd();

    //�����������񷽷�
    void doOtherTasks();

    //��鲢�Ҵ���ʱ������
    void checkAndDoTimers();

    //���ʱ�����Ƿ����� �����̵߳���
    bool isCallableInOwnerThread() const;

    //��ʱ���ڲ�����
    void addTimerInternal(std::shared_ptr<Timer> timer);
    void removeTimerInternal(int64_t timerId);
private:
    bool                                     m_isBaseLoop;                  //���¼�ѭ���ı�־
    bool                                     m_running{ false };            //�¼�ѭ�����б�־
    bool                                     m_isCheckTimers;               //��ʶ��ǰ�Ƿ���������鶨ʱ���߼���
    int                                      m_eventfd;                     //��ʱû��
    int                                      m_wakeUpFd;                    //����fd

    std::unique_ptr<IOMultiplex>             m_spIOMultiplex;               //��ǰ�¼�ѭ����IO����ģ��
    std::thread::id                          m_threadID;                    //��ǰʱ��ѭ����Ӧ���߳�ID

    WakeUpEventDispatcher* m_pWakeUpEventDispatcher{ nullptr };             //�����¼�����

    std::vector<CustomTask>                  m_customTasks;                 //�Զ��������б�
    std::mutex                               m_mutexTasks;                  //�Զ������� ͬ����

    //TODO: ��Timer�������ɸ��� �´δ���ʱ�� �Զ�����ģ�
    std::vector<std::shared_ptr<Timer>>      m_timers;                      //��ʱ�������б�
    std::vector<std::shared_ptr<Timer>>      m_pendingAddTimer;             //����ӵĶ�ʱ�������б�
    std::vector<int64_t>                     m_pendingRemoveTimer;          //��ɾ���Ķ�ʱ������ID
    std::mutex                               m_mutexTimers;                 //��ʱ������ ͬ���ٶ�
};

#endif //! NET_EVENTLOOP_H_