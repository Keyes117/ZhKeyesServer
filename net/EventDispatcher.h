#pragma once
/**
 * @zhkeyes 2025/4/6
 * 事件分发接口类
 */
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

