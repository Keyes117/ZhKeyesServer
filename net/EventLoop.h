#pragma once
/**
 * @Zhkeyes  2025/4/6 remake
 *
 */
#ifndef     EVENTLOOP_H
#define     EVENTLOOP_H

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
    bool createWakeUpfd();

    void doOtherTasks();

    void checkAndDoTimers();

    bool isCallableInOwnerThread() const;

    void addTimerInternal(std::shared_ptr<Timer> timer);
    void removeTimerInternal(int64_t timerId);
private:
    bool                                     m_isBaseLoop;
    bool                                     m_running{ false };
    bool                                     m_isCheckTimers;
    int                                      m_epollfd;
    int                                      m_wakeUpFd;

    std::unique_ptr<IOMultiplex>             m_spIOMultiplex;
    std::thread::id                          m_threadID;

    WakeUpEventDispatcher* m_pWakeUpEventDispatcher{ nullptr };

    std::vector<CustomTask>                  m_customTasks;
    std::mutex                               m_mutexTasks;

    //TODO: ��Timer�������ɸ��� �´δ���ʱ�� �Զ�����ģ�
    std::vector<std::shared_ptr<Timer>>      m_timers;
    std::vector<std::shared_ptr<Timer>>      m_pendingAddTimer;
    std::vector<int64_t>      m_pendingRemoveTimer;
    std::mutex                               m_mutexTimers;
};

#endif