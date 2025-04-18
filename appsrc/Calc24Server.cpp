#include "Calc24Server.h"

#include <iostream>


bool Calc24Server::init(int32_t threadNum, const std::string& ip, uint16_t port)
{

    m_TcpServer.setConnectionCallBack(std::bind(&Calc24Server::onConnected, this, std::placeholders::_1));
    //m_TcpServer.setDisConnectionCallBack(std::bind(&Calc24Server::onDisConnected, this,std::placeholders::_1));
    if (!m_TcpServer.init(5, "0.0.0.0", 8888))
    {
        return false;
    }

    m_checkHandupTimerID = m_TcpServer.getBaseEventLoop().addTimer(1000, -1, [=](int64_t timeID)->void
        {
            static constexpr int REQUIRED_MIN_READY_COUNT = 2;

            //每隔1秒检测是否有足够的玩家
            int readySessionCount = 0;
            std::cout << "detecting any handsup! " << std::endl;
            for (auto& iter : m_sessions)
            {
                if (iter.second->isReady())
                {
                    readySessionCount++;

                }
            }

            if (readySessionCount < REQUIRED_MIN_READY_COUNT)
                return;

            std::string newCards = generateCards();

            for (auto& iter : m_sessions)
            {
                if (iter.second->isReady())
                {
                    iter.second->sendCards(newCards);
                }
            }
        });
    m_TcpServer.start();
    return true;
}

void Calc24Server::close()
{
    if (m_checkHandupTimerID > 0)
    {
        m_TcpServer.getBaseEventLoop().removeTimer(m_checkHandupTimerID);
    }
    m_TcpServer.close();
}

void Calc24Server::onConnected(std::shared_ptr<TCPConnection>& spConn)
{
    m_pendingToDeleteSessions.clear();
    std::cout << "Calc24Server get a connection\n " << std::endl;
    auto spCalc24Session = std::make_shared<Calc24Session>(this, std::move(spConn));

    spCalc24Session->sendWelcomeMsg();
    spCalc24Session->notifyUserToBeReady();
    m_sessions.emplace(spCalc24Session->getID(), std::move(spCalc24Session));
}

std::string Calc24Server::generateCards()
{
    static constexpr char allCards[] = { 'A','2','3','4','5','6','7','8','9','X','J','Q','K','w','W' };
    static constexpr int allCardsCount = sizeof(allCards) / sizeof(allCards[0]);

    int index1 = rand() % allCardsCount;
    int index2 = rand() % allCardsCount;
    int index3 = rand() % allCardsCount;
    int index4 = rand() % allCardsCount;

    char newCards[24];
    sprintf(newCards, "Your cards is: %c %c %c %c\n",
        allCards[index1],
        allCards[index2],
        allCards[index3],
        allCards[index4]);

    return std::string(newCards, strlen(newCards));
}

void Calc24Server::onDisConnected(int32_t id)
{

    auto iter = m_sessions.find(id);
    if (iter != m_sessions.end())
    {
        std::shared_ptr<Calc24Session> pendingToDeleteSession = iter->second;
        m_pendingToDeleteSessions.push_back(std::move(pendingToDeleteSession));
        m_sessions.erase(iter);
        std::cout << "Calc24Server::onDisConnected" << std::endl;
    }

}

void Calc24Server::sendAll(const std::string& msg, bool includeSelf, int32_t id)
{
    for (const auto& session : m_sessions)
    {
        //iter => std::pair<int, session>
        if (!includeSelf)
        {
            if (session.second->getID() == id)
                continue;
        }

        session.second->sendMsg(msg);
    }
}
