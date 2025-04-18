#pragma once
/**
 * @zhkeyes 2024/4/7 œﬂ≥Ã≥ÿ¿‡
 */

#include <memory>
#include <stdint.h>
#include <thread>
#include <vector>

#include "EventLoop.h"

class ThreadPool final
{

public:
    ThreadPool() = default;
    ~ThreadPool() = default;

    void start(int32_t threadNum = 1);
    void stop();

    std::shared_ptr<EventLoop>  getNextEventLoop();


private:
    void threadFunc(size_t index);
private:
    bool                                        m_stop{ false };

    size_t                                      m_lastEventLoopNo{ 0 };

    std::vector<std::shared_ptr<EventLoop>>     m_eventLoops;
    std::vector<std::shared_ptr<std::thread>>   m_threads;

private:
    ThreadPool(const ThreadPool& rhs) = delete;
    ThreadPool& operator=(const ThreadPool& rhs) = delete;
    ThreadPool(ThreadPool&& rhs) = delete;
    ThreadPool& operator=(ThreadPool&& rhs) = delete;
};

