
#ifndef NET_TIMER_H_
#define NET_TIMER_H_
#include "TimerService.h"
#include <cstdint>

class Timer
{

public:
    Timer(int32_t intervalMs, int64_t repeatCount,
        TimerTask task, TimerMode mode = TimerMode::TimerModeFixedInterval);
    virtual ~Timer() = default;

    int64_t getId();

    static int64_t generateTimerId();

    int64_t nextTriggeredTimeMs();

    void doTimer(int64_t timerId, int64_t nowMs);
private:
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    Timer(Timer&&) = delete;
    Timer& operator=(Timer&&) = delete;

private:
    int64_t     m_id;

    int32_t     m_intervalMs;
    /**
 * @Ϊ -1 ��ʶһֱ�ظ�, 0 ��ʶ���ظ� ��>0 �����ظ�����
 */
    int64_t     m_repeatedCount;

    TimerTask   m_task;
    TimerMode   m_mode;

    int64_t     m_nextTriggeredTimeMs;
};
#endif //!NET_TIMER_H_