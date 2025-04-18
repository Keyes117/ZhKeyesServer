#pragma once

#include <functional>
#include <stdint.h>

enum class TimerMode
{
    TimerModeFixedInterval = 0,
    TimerModeFixedRate
};

using TimerTask = std::function<void(int64_t timerID)>;

class TimerService
{

public:
    virtual ~TimerService() = default;

    virtual int64_t addTimer(int32_t intervalMs, int64_t repeatCount, TimerTask task, TimerMode mode) = 0;
    virtual void  removeTimer(int64_t timerID) = 0;

};

