#ifndef NET_POLL_H_
#define NET_POLL_H_

/**
 * @zhkeyes 2025/4/6
 * poll ·â×°Àà
 */

 /**
  *   int select(int nfds, fd_set *readfds, fd_set *writefds,
  *               fd_set *exceptfds, struct timeval *timeout);
  */

#include "IOMultiplex.h"

#ifdef _WIN32
#else
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>

#endif // _WIN32

#include <vector>
#include <unordered_map>
#include <tuple>

typedef std::tuple<struct pollfd, EventDispatcher*> PollData;


class Poll : public IOMultiplex
{
public:
    Poll() = default;
    virtual ~Poll() = default;

    virtual void poll(int  timeoutUs, std::vector<EventDispatcher*>& triggerdEventDispatchers) override;

    virtual void registerReadEvent(int fd, EventDispatcher* dispatcher) override;
    virtual void registerWriteEvent(int fd, EventDispatcher* dispatcher) override;

    virtual void unRegisterReadEvent(int fd, EventDispatcher* dispatcher) override;
    virtual  void unRegisterWriteEvent(int fd, EventDispatcher* dispatcher) override;


    virtual void unRegisterAllEvent(int fd, EventDispatcher* dispatcher) override;
private:
    std::unordered_map<int, PollData*>   m_fdEventMap;


};

#endif