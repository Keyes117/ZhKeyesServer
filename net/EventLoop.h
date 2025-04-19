#pragma once

/**
 * @brief  事件循环类 EventLoop
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
//自定义任务的回调函数
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

    //获取线程ID
    void setThreadID(const std::thread::id& threadID);
    std::thread::id getThreadID() const;

    //读写事件相关方法
    void registerReadEvent(int fd, EventDispatcher* dispatcher);
    void registerWriteEvent(int fd, EventDispatcher* dispatcher);
    void unRegisterReadEvent(int fd, EventDispatcher* dispatcher);
    void unRegisterWriteEvent(int fd, EventDispatcher* dispatcher);
    void unRegisterAllEvent(int fd, EventDispatcher* dispatcher);

    //注册唤醒事件方法
    void RegisterCustomTask(CustomTask&& task);


    // 定时器业务
    virtual int64_t addTimer(int32_t intervalMs, int64_t repeatCount, TimerTask task, TimerMode mode = TimerMode::TimerModeFixedInterval) override;
    virtual void  removeTimer(int64_t timerID) override;

private:
    //唤醒事件FD
    bool createWakeUpfd();

    //处理其他任务方法
    void doOtherTasks();

    //检查并且处理定时器任务
    void checkAndDoTimers();

    //检查时任务是否是其 所属线程调用
    bool isCallableInOwnerThread() const;

    //定时器内部方法
    void addTimerInternal(std::shared_ptr<Timer> timer);
    void removeTimerInternal(int64_t timerId);
private:
    bool                                     m_isBaseLoop;                  //主事件循环的标志
    bool                                     m_running{ false };            //事件循环运行标志
    bool                                     m_isCheckTimers;               //标识当前是否是正常检查定时器逻辑。
    int                                      m_eventfd;                     //暂时没用
    int                                      m_wakeUpFd;                    //唤醒fd

    std::unique_ptr<IOMultiplex>             m_spIOMultiplex;               //当前事件循环的IO复用模型
    std::thread::id                          m_threadID;                    //当前时间循环对应的线程ID

    WakeUpEventDispatcher* m_pWakeUpEventDispatcher{ nullptr };             //唤醒事件处理

    std::vector<CustomTask>                  m_customTasks;                 //自定义任务列表
    std::mutex                               m_mutexTasks;                  //自定义任务 同步锁

    //TODO: 将Timer容器换成根据 下次触发时间 自动排序的，
    std::vector<std::shared_ptr<Timer>>      m_timers;                      //定时器任务列表
    std::vector<std::shared_ptr<Timer>>      m_pendingAddTimer;             //待添加的定时器任务列表
    std::vector<int64_t>                     m_pendingRemoveTimer;          //待删除的定时器任务ID
    std::mutex                               m_mutexTimers;                 //定时器任务 同步速度
};

#endif //! NET_EVENTLOOP_H_