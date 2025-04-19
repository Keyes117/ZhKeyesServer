#include "EventLoop.h"

#include <functional>
#include <iostream>


#include "common.h"
#include "Epoll.h"
#include "Poll.h"
#include "Select.h"



#ifdef _WIN32
#else
#include <sys/eventfd.h>

#endif // _WIN32


EventLoop::EventLoop(bool isBaseLoop)
    :m_isBaseLoop(isBaseLoop)
{
}

EventLoop::~EventLoop()
{
    if (m_pWakeUpEventDispatcher)
    {
        delete m_pWakeUpEventDispatcher;
        m_pWakeUpEventDispatcher = nullptr;
    }
}

bool EventLoop::init(IOMultiplexType type /*= IOMultiplexType::Epoll*/)
{
#ifdef _WIN32
#else
    if (type == IOMultiplexType::Epoll)
    {
        m_spIOMultiplex = std::make_unique<Epoll>();
    }
    else if (type == IOMultiplexType::Select)
    {
        m_spIOMultiplex = std::make_unique<Select>();
    }
    else if (type == IOMultiplexType::Poll)
    {
        m_spIOMultiplex = std::make_unique<Poll>();
    }
#endif // _WIN32

    if (!createWakeUpfd())
        return false;

    m_pWakeUpEventDispatcher = new WakeUpEventDispatcher(m_wakeUpFd);

    registerReadEvent(m_wakeUpFd, m_pWakeUpEventDispatcher);

    m_running = true;

    return true;
}


void EventLoop::run()
{

    std::cout << "threadID :" << m_threadID << std::endl;

    std::vector<EventDispatcher*> eventDispatchers;
    while (m_running)
    {
        //1�����ʹ���ʱ��ʱ��
        checkAndDoTimers();

        //2��ʹ��select/poll/epoll ��IO���ú������һ��socket�Ķ�д�¼�
        eventDispatchers.clear();
        m_spIOMultiplex->poll(500000, eventDispatchers);

        //3�������д�¼�
        for (size_t i = 0; i < eventDispatchers.size(); i++)
        {
            auto eventDispatcher = eventDispatchers[i];
            if (eventDispatcher == nullptr)
                continue;
            if (eventDispatcher)
                eventDispatcher->onRead();
            if (eventDispatcher)
                eventDispatcher->onWrite();
        }

        //4������fd�Ļ��ѻ��ƴ���
        doOtherTasks();
    }
}

void EventLoop::setThreadID(const std::thread::id& threadID)
{
    m_threadID = threadID;
}

std::thread::id EventLoop::getThreadID() const
{
    return m_threadID;
}


//epoll_ctl
void EventLoop::registerReadEvent(int fd, EventDispatcher* dispatcher)
{
    m_spIOMultiplex->registerReadEvent(fd, dispatcher);
}

void EventLoop::registerWriteEvent(int fd, EventDispatcher* dispatcher)
{
    m_spIOMultiplex->registerWriteEvent(fd, dispatcher);
}

void EventLoop::unRegisterReadEvent(int fd, EventDispatcher* dispatcher)
{
    m_spIOMultiplex->unRegisterReadEvent(fd, dispatcher);
}

void EventLoop::unRegisterWriteEvent(int fd, EventDispatcher* dispatcher)
{
    m_spIOMultiplex->unRegisterWriteEvent(fd, dispatcher);
}

void EventLoop::unRegisterAllEvent(int fd, EventDispatcher* dispatcher)
{
    m_spIOMultiplex->unRegisterAllEvent(fd, dispatcher);
}

void EventLoop::RegisterCustomTask(CustomTask&& task)
{
    {
        std::lock_guard<std::mutex> scopedLock(m_mutexTasks);
        m_customTasks.push_back(std::move(task));
    }

    m_pWakeUpEventDispatcher->wakeUp();
}


int64_t EventLoop::addTimer(int32_t intervalMs,
    int64_t repeatCount, TimerTask task, TimerMode mode /*= TimerMode::TimerModeFixedInterval*/)
{

    auto spTimer = std::make_shared<Timer>(intervalMs,
        repeatCount, task, mode);

    addTimerInternal(spTimer);

    return spTimer->getId();
}

void EventLoop::removeTimer(int64_t timerID)
{
    removeTimerInternal(timerID);
}

bool EventLoop::createWakeUpfd()
{
#ifdef _WIN32
#else
    m_wakeUpFd = ::eventfd(0, EFD_NONBLOCK);
    return m_wakeUpFd != INVALID_FD;
#endif // _WIN32

}

void EventLoop::doOtherTasks()
{

    //Ϊ�˼��� ���Ŀ����ȣ������ʱ����һֱռ����
    //��Ҫִ�е����� �ó���
    //swap �������κο������ƶ�
    std::vector<CustomTask> tasks;
    {
        std::lock_guard<std::mutex> scopedLock(m_mutexTasks);
        tasks.swap(m_customTasks);
    }

    for (auto& task : tasks)
    {
        task();
    }
}

void EventLoop::checkAndDoTimers()
{
    /*if (!m_isBaseLoop)
        return;*/

        //ȡ��ǰϵͳʱ��
    int64_t nowMs = std::chrono::duration_cast<std::chrono::milliseconds>
        (std::chrono::system_clock::now().time_since_epoch()).count();


    //�����־���������ڣ����һ���¼��������¼��߼��ڵ���һ����ӻ����Ƴ����߼���
    //�ͻᵼ�� �Լ����Լ�ɾ�����������������⡣
    //��˴���ĳһ����ʱ�����߼�ʱ�����������Ӻ�ɾ���Ĳ���
    m_isCheckTimers = true;
    {
        //std::lock_guard<std::mutex> scopedLock(m_mutexTimers);
        for (auto iter = m_timers.begin(); iter != m_timers.end(); iter++)
        {
            //�����鵱ǰ �¼� �� �¼���һ�δ������¼�
            while (nowMs >= (*iter)->nextTriggeredTimeMs())
            {
                std::cout << "addTimerInternal, timerID " << (*iter)->getId()
                    << " threadId " << std::this_thread::get_id() << std::endl;


                (*iter)->doTimer((*iter)->getId(), nowMs);
            }
        }
    }
    m_isCheckTimers = false;

    if (m_pendingRemoveTimer.empty())
    {
        for (auto& timerId : m_pendingRemoveTimer)
            removeTimerInternal(timerId);
    }

    if (m_pendingAddTimer.empty())
    {
        for (auto& spTimer : m_pendingAddTimer)
            addTimerInternal(spTimer);
    }

}

bool EventLoop::isCallableInOwnerThread() const
{
    return std::this_thread::get_id() == m_threadID;
}

void EventLoop::addTimerInternal(std::shared_ptr<Timer> spTimer)
{
    //TODO  ��Ч�Ե�У��
    if (isCallableInOwnerThread())
    {
        if (m_isCheckTimers)
        {
            m_pendingAddTimer.push_back(spTimer);
        }
        else
        {
            std::cout << "addTimerInternal, timerID " << spTimer->getId()
                << " threadId " << std::this_thread::get_id() << std::endl;

            m_timers.push_back(spTimer);
        }

    }
    else
    {
        RegisterCustomTask(std::bind(&EventLoop::addTimerInternal, this, spTimer));
    }
}

void EventLoop::removeTimerInternal(int64_t timerId)
{
    //���û��ѵĻ��ƣ�������ǵ�ǰ�߳������¼�ѭ�����򽻸��������Զ���������
    if (isCallableInOwnerThread())
    {
        if (m_isCheckTimers)
        {
            m_pendingRemoveTimer.push_back(timerId);
        }
        else
        {
            for (auto iter = m_timers.begin(); iter != m_timers.end(); iter++)
            {
                if ((*iter)->getId() == timerId)
                {
                    std::cout << "removeTimerInternal, timerID " << (*iter)->getId()
                        << " threadId " << std::this_thread::get_id() << std::endl;

                    //������Timer�Լ��Ļ��յ����Ӷ����³������
                    m_timers.erase(iter);
                    return;
                }
            }
        }

    }
    else
    {
        RegisterCustomTask(std::bind(&EventLoop::removeTimerInternal,
            this, timerId));
    }
}

