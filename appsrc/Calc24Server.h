#pragma once

#include "TCPServer.h"

#include "Calc24Session.h"

#include <map>
#include <string.h>
#include <vector>


class Calc24Server  final
{
public:
    Calc24Server() = default;
    ~Calc24Server() = default;

    bool init(int32_t threadNum, const std::string& ip = "", uint16_t port = 8888);
    void close();

    void onDisConnected(int32_t id);

    void sendAll(const std::string& msg, bool includeSelf, int32_t id);

private:
    void onConnected(std::shared_ptr<TCPConnection>& spConn);

    std::string generateCards();

private:
    TCPServer   m_TcpServer;

    //std::vector<std::shared_ptr<Calc24Session>> 
    //key => 
    std::map<int32_t, std::shared_ptr<Calc24Session>>  m_sessions;
    std::vector<std::shared_ptr<Calc24Session>>         m_pendingToDeleteSessions;

    int64_t                                             m_checkHandupTimerID{ 0 };
};

