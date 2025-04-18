#pragma once

#include <atomic>
#include <memory>
#include <stdint.h>

#include "ByteBuffer.h"
#include "Calc24Protocol.h"
#include "TCPConnection.h"

enum class DecodePackageResult {
    DecodePackageSuccess,
    DecodePackageFailed,
    DecodePackageExpectMore
};

enum class SessionStatus
{
    IDLE,           //����״̬
    READY,          //����״̬
    IN_GAME          //��Ϸ״̬
};

class Calc24Server;
class Calc24Session : public std::enable_shared_from_this<Calc24Session>
{
public:

    Calc24Session(Calc24Server* pServer, std::shared_ptr<TCPConnection>&& spConn);
    ~Calc24Session();

    void onRead(ByteBuffer& byteBuffer);
    void onWrite();
    void onClose();

    int32_t getID() const {
        return m_id;
    }

    /**
     * @brief ���ͻ�ӭ��Ϣ
     * @return
     */
    bool sendWelcomeMsg();

    //֪ͨ���׼��
    void notifyUserToBeReady();

    void sendMsg(const std::string msg);

    void sendCards(const std::string& cards);

    void forceClose();

    bool isReady() const;
private:
    //��ͷ�����ʽ���
    DecodePackageResult decodePackage(ByteBuffer& byteBuffer, const MsgHeader& header);
    //���з����
    DecodePackageResult decodePackage(ByteBuffer& byteBuffer);

    bool processPackage(const std::string& package);

    bool processChatMsg(const std::string& package);

    bool processCommand(const std::string& package);

    static int32_t generateID();



private:
    Calc24Server* m_pServer;
    std::shared_ptr<TCPConnection>  m_spConn;

    int                             m_id;
    std::atomic< int8_t>            m_status{ static_cast<int8_t>(SessionStatus::IDLE) };
};

