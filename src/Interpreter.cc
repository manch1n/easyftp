#include "Interpreter.h"

#include <cstring>

#include "src/ErrCheck.h"

using namespace myftp;

Message Interpreter::parseRawMsg(const std::vector<char> &rawMsg)
{
    Message msg;
    const char *rawData = rawMsg.data();
    size_t rawSize = rawMsg.size();
    ASSERT_EXIT(rawSize >= kHEADER_SIZE, "bad msg format");
    msg.cmdOrReply = *(reinterpret_cast<const int32_t *>(rawData));
    msg.length = *(reinterpret_cast<const u_int64_t *>(rawData + sizeof(int32_t)));
    msg.data.resize(rawSize - kHEADER_SIZE);
    std::memcpy(msg.data.data(), rawData + kHEADER_SIZE, rawSize - kHEADER_SIZE);
    return msg;
}

std::vector<char> Interpreter::formatPacket(int32_t cmd, const std::vector<char> &data)
{
    u_int64_t length = kHEADER_SIZE + data.size();
    std::vector<char> msg(length);
    std::memcpy(msg.data(), &cmd, sizeof(int32_t));
    std::memcpy(msg.data() + sizeof(int32_t), &length, sizeof(u_int64_t));
    std::memcpy(msg.data() + kHEADER_SIZE, data.data(), data.size());
    return msg;
}

std::vector<char> myftp::Interpreter::formatPacket(int32_t cmd)
{
    u_int64_t length = kHEADER_SIZE;
    std::vector<char> msg(length);
    std::memcpy(msg.data(), &cmd, sizeof(int32_t));
    std::memcpy(msg.data() + sizeof(int32_t), &length, sizeof(u_int64_t));
    return msg;
}
