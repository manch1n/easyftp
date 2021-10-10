#include "Interpreter.h"

#include <cstring>
#include <algorithm>

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

void myftp::Interpreter::parseFileInfo(const std::vector<char> &data, size_t *length, std::string *fileName)
{
    auto strs = splitByUnitSeperator(data, true);
    std::string namestr(strs[0].begin(), strs[0].end());
    std::string lenstr(strs[1].begin(), strs[1].end());
    *length = std::stol(lenstr);
    *fileName = namestr;
}

std::vector<char> myftp::Interpreter::formatFileInfo(const std::string &fileName, size_t len)
{
    std::vector<char> ret(fileName.begin(), fileName.end());
    ret.push_back(kunitSeperator);
    std::string lenstr = std::to_string(len);
    for (auto c : lenstr)
    {
        ret.push_back(c);
    }
    return ret;
}

std::vector<std::vector<char>> myftp::Interpreter::splitByUnitSeperator(const std::vector<char> &s, bool splitOnce)
{
    std::vector<std::vector<char>> ret;
    auto start = s.begin();
    auto end = std::find(s.cbegin(), s.cend(), kunitSeperator);
    if (splitOnce == true)
    {
        ret.push_back(std::vector<char>(s.begin(), end));
        if (end != s.cend())
        {
            ret.push_back(std::vector<char>(end + 1, s.cend()));
        }
        return ret;
    }
    while (1)
    {
        if (start == end)
        {
            break;
        }
        ret.push_back(std::vector<char>(start, end));
        if (end == s.cend())
        {
            break;
        }
        start = end + 1;
        end = std::find(start, s.cend(), kunitSeperator);
    }
    return ret;
}
