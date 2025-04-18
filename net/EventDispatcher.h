#pragma once
/**
 * @zhkeyes 2025/4/6
 * �¼��ַ��ӿ���
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

