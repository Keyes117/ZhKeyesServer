#ifndef APPSRC_CALC24PROTOCOL_H_
#define APPSRC_CALC24PROTOCOL_H_

#include <stdint.h>

enum class MsgType
{
    MsgTypeWelcome,
    MsgTypeInitCard,
    MsgTypeChatMsg
};

struct MsgHeader {
    uint32_t  packageSize;
    uint32_t  msgType;
};

struct WelconMsg
{
    uint32_t msgType;
    char welcomeMsg[32];
};

#endif //!APPSRC_CALC24PROTOCOL_H_