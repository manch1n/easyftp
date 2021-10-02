#pragma once
#include "CommandReply.h"

class Interpreter
{
public:
    static Message parseRawMsg(const std::vector<char> &rawMsg);
    static std::vector<char> formatPacket(int32_t cmd, const std::vector<char> &data = std::vector<char>(0));
};