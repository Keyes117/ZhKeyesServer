/**
 * @zhkeyes 2025/4/6
 * 事件分发接口类
 */

#ifndef NET_EVENTDISPATCHER_H_
#define NET_EVENTDISPATCHER_H_




class EventDispatcher
{
public:
    virtual ~EventDispatcher() = default;

    virtual void onRead() = 0;
    virtual void onWrite() = 0;
    virtual void onClose() = 0;

    virtual void enableRead(bool isEnabled) = 0;
    virtual void enableWrite(bool isEnabled) = 0;

};

#endif // !NET_EVENTDISPATCHER_H_