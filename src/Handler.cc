#include <algorithm>

#include "src/Handler.h"
#include "src/ErrCheck.h"

using namespace myftp;

std::vector<char> Handler::receiveMsg(TcpStream &strm)
{
    std::vector<char> buf(HEADER_SIZE);
    size_t ret = strm.receiveSome(buf.data(), HEADER_SIZE);
    ASSERT_EXIT(ret == HEADER_SIZE, "recv error");
    uint64_t length = *(reinterpret_cast<uint64_t *>(buf.data() + sizeof(int32_t)));
    uint64_t dataLen = length - HEADER_SIZE;
    buf.resize(length);
    if (dataLen == 0)
    {
        return buf;
    }
    ret = strm.receiveSome(buf.data() + HEADER_SIZE, dataLen);
    ASSERT_EXIT(ret == dataLen, "recv error");
    return buf;
}

void Handler::sendMsg(TcpStream &strm, const std::vector<char> &msg)
{
    size_t ret = strm.sendAll(msg.data(), msg.size());
    ASSERT_EXIT(ret == msg.size(), "send error");
}

std::vector<std::vector<char>> Handler::splitByUnitSeperator(const std::vector<char> &s, bool splitOnce)
{
    std::vector<std::vector<char>> ret;
    auto start = s.begin();
    auto end = std::find(s.cbegin(), s.cend(), unitSeperator);
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
        end = std::find(start, s.cend(), unitSeperator);
    }
    return ret;
}
