#pragma once


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