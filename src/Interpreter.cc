#include "Interpreter.h"

#include <cstring>

#include "src/ErrCheck.h"

Message Interpreter::parseRawMsg(const std::vector<char> &rawMsg)
{
    Message msg;
    const char *rawData = rawMsg.data();
    size_t rawSize = rawMsg.size();
    ASSERT_EXIT(rawSize >= HEADER_SIZE, "bad msg format");
    msg.cmdOrReply = *(reinterpret_cast<const int32_t *>(rawData));
    msg.length = *(reinterpret_cast<const u_int64_t *>(rawData + sizeof(int32_t)));
    msg.data.resize(rawSize - HEADER_SIZE);
    std::memcpy(msg.data.data(), rawData + HEADER_SIZE, rawSize - HEADER_SIZE);
    return msg;
}

std::vector<char> Interpreter::formatPacket(int32_t cmd, const std::vector<char> &data)
{
    u_int64_t length = HEADER_SIZE + data.size();
    std::vector<char> msg(length);
    std::memcpy(msg.data(), &cmd, sizeof(int32_t));
    std::memcpy(msg.data() + sizeof(int32_t), &length, sizeof(u_int64_t));
    std::memcpy(msg.data() + HEADER_SIZE, data.data(), data.size());
    return msg;
}
