#include "WakeUpEventDispatcher.h"

#ifdef _WIN32
#else 
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#endif

#include <string.h>
#include <iostream>
#include <stdint.h>
WakeUpEventDispatcher::WakeUpEventDispatcher(int fd)
    :m_wakeUpfd(fd)
{
}

WakeUpEventDispatcher::~WakeUpEventDispatcher()
{
#ifdef _WIN32
#else
    ::close(m_wakeUpfd);
#endif // _WIN32

}

void WakeUpEventDispatcher::onRead()
{
    if (!m_enableRead)
        return;
    //������
#ifdef _WIN32

#else
    //ע�⻽��Fd �������� recv �� send

    int64_t dummyData;
    int n = ::read(m_wakeUpfd, static_cast<void*>(&dummyData), sizeof(dummyData));

    if (n != sizeof(dummyData))
        std::cout << "WakeupEventDiapatcher::onRead failed, errno " << errno << std::endl;
    else
        std::cout << "WakeupEventDiapatcher::wakeup successfully, wakefd: " << m_wakeUpfd << std::endl;

    //���

#endif // _WIN32

}

void WakeUpEventDispatcher::onWrite()
{
}

void WakeUpEventDispatcher::onClose()
{
}

void WakeUpEventDispatcher::enableRead(bool isEnabled)
{
    m_enableRead = isEnabled;
}

void WakeUpEventDispatcher::enableWrite(bool isEnabled)
{
    m_enableWrite = isEnabled;
}

void WakeUpEventDispatcher::wakeUp()
{
#ifdef _WIN32
#else
    //ע�⻽��Fd �������� recv �� send
    //ע�����eventfd ����Ҫд��8�ֽ�
    int64_t dummyData = 0;
    int n = ::write(m_wakeUpfd, static_cast<const void*>(&dummyData), sizeof(dummyData));

    if (n != sizeof(dummyData))
        std::cout << "WakeupEventDiapatcher::wakeup failed, errno: " << errno << " " << strerror(errno) << std::endl;
    else
        std::cout << "WakeupEventDiapatcher::wakeup successfully, wakefd: " << m_wakeUpfd << std::endl;
#endif // _WIN32


}
