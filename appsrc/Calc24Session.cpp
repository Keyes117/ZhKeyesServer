#include "Calc24Session.h"

#include "Calc24Server.h"

#include <iostream>
#include <sstream>
#include <string.h>

#define  MAX_MSG_LENGTH  64
#define  READ_COMMAND  "!ready"
//#define  CALC_COMMAND  

//std::atomic<int32_t> Calc24Session::m_baseID{0};

Calc24Session::Calc24Session(Calc24Server* pServer, std::shared_ptr<TCPConnection>&& spConn)
    :m_pServer(pServer),
    m_spConn(std::move(spConn))
{

    m_id = Calc24Session::generateID();

    m_spConn->setReadCallBack(std::bind(&Calc24Session::onRead, this, std::placeholders::_1));
    m_spConn->setWriteCallBack(std::bind(&Calc24Session::onWrite, this));
    m_spConn->setCloseCallBack(std::bind(&Calc24Session::onClose, this));
    std::cout << "Calc24Session::ctor" << m_id << std::endl;
}

Calc24Session::~Calc24Session()
{
    std::cout << "Calc24Session::dtor" << m_id << std::endl;
}

void Calc24Session::onRead(ByteBuffer& recvBuf)
{
    while (true)
    {
        /*        if (recvBuf.remaining() <= sizeof(MsgHeader))
                {
                    return ;
                }
                   */
        DecodePackageResult result = decodePackage(recvBuf);

        if (result == DecodePackageResult::DecodePackageExpectMore)
            return;

        if (result == DecodePackageResult::DecodePackageFailed)
        {
            forceClose();
            return;
        }

        //继续解包


        //MsgHeader msgHeader;
   /*     recvBuf.peek(reinterpret_cast<char*>(&msgHeader), sizeof(MsgHeader));

        if (msgHeader.packageSize <= recvBuf.remaining())
        {
            if (decodePackage(recvBuf, msgHeader))
                return ;
        }*/
    }


}

void Calc24Session::onWrite()
{
}

void Calc24Session::onClose()
{
    m_pServer->onDisConnected(m_id);
}

bool Calc24Session::sendWelcomeMsg()
{
    const std::string& welcomeMsg = "Welcome to Calc24";
    return m_spConn->send(welcomeMsg);
}

void Calc24Session::notifyUserToBeReady()
{
    m_spConn->registerTimer(5000, -1, [=](int64_t timeId)->void {
        if (m_status == static_cast<int8_t>(SessionStatus::IDLE))
            m_spConn->send("please ready....\n");
        else
            m_spConn->unregisterTimer(timeId);

        });
}

void Calc24Session::sendMsg(const std::string msg)
{
    m_spConn->send(msg);
}

void Calc24Session::sendCards(const std::string& cards)
{

    if (m_status == static_cast<int8_t>(SessionStatus::READY))
        m_spConn->send(cards);
    m_status = static_cast<int8_t>(SessionStatus::IN_GAME);
}

void Calc24Session::forceClose()
{
    m_spConn->onClose();
}


DecodePackageResult Calc24Session::decodePackage(ByteBuffer& recvBuf, const MsgHeader& header)
{
    if (static_cast<MsgType>(header.msgType) == MsgType::MsgTypeWelcome)
    {
        WelconMsg welcomeMsg;
        recvBuf.retrieve(reinterpret_cast<char*>(&welcomeMsg), sizeof(welcomeMsg));

        //TODO: 处理Msg

        return DecodePackageResult::DecodePackageSuccess;

    }
    return DecodePackageResult::DecodePackageFailed;
}

DecodePackageResult Calc24Session::decodePackage(ByteBuffer& byteBuffer)
{

    size_t positionLF = byteBuffer.findLF();
    if (positionLF == std::string::npos)
    {
        if (byteBuffer.remaining() > MAX_MSG_LENGTH)
            return DecodePackageResult::DecodePackageFailed;

        return DecodePackageResult::DecodePackageExpectMore;
    }

    std::string currentPackage;
    byteBuffer.retrieve(currentPackage, positionLF + 1);
    processPackage(currentPackage);

    return  DecodePackageResult::DecodePackageSuccess;
}

bool Calc24Session::processPackage(const std::string& package)
{


    if (package.empty())
        return true;

    if (package[0] != '!')
    {
        //聊天消息
        processChatMsg(package);
    }
    else
    {
        //特殊指令
        processCommand(package);
    }

    return true;


}

bool Calc24Session::processChatMsg(const std::string& package)
{
    std::cout << "clientfd[" << m_id << "] says: " << package << std::endl;

    std::ostringstream msgWithPrefix;

    msgWithPrefix << "client["
        << std::to_string(m_id)
        << "],says " << package;


    m_pServer->sendAll(msgWithPrefix.str(), false, m_id);
    return true;
}

bool Calc24Session::processCommand(const std::string& package)
{
    //!ready 表示举手
    //! 2 3 4 5 标识对结果进行计算 
    if (package.substr(0, 6) == READ_COMMAND)
    {
        m_status = static_cast<int8_t>(SessionStatus::READY);
        return true;
    }


    return false;

}

int32_t Calc24Session::generateID()
{
    static std::atomic<int32_t>     m_baseID{ 0 };
    int32_t result = ++m_baseID;
    return result;
}

bool Calc24Session::isReady() const
{
    return m_status == static_cast<int8_t>(SessionStatus::READY);
}

