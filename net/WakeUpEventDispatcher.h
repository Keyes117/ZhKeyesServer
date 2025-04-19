#ifndef NET_WAKEUPEVENTDISPATCHER_H_
#define NET_WAKEUPEVENTDISPATCHER_H_
#include "EventDispatcher.h"

class WakeUpEventDispatcher :public  EventDispatcher
{
public:
    WakeUpEventDispatcher(int fd);
    virtual ~WakeUpEventDispatcher();

    virtual void onRead() override;
    virtual void onWrite() override;
    virtual void onClose()override;

    virtual void enableRead(bool isEnabled) override;
    virtual void enableWrite(bool isEnabled)override;

    void wakeUp();

private:
    int             m_wakeUpfd;
    bool            m_enableRead{ false };
    bool            m_enableWrite{ false };
};

#endif //! NET_WAKEUPEVENTDISPATCHER_H_