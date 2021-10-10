#include <algorithm>

#include "src/Handler.h"
#include "src/ErrCheck.h"

using namespace myftp;

std::vector<char> Handler::receiveMsg(TcpStream *strm)
{
    std::vector<char> buf(kHEADER_SIZE);
    size_t ret = strm->receiveSome(buf.data(), kHEADER_SIZE);
    ASSERT_EXIT(ret == kHEADER_SIZE, "recv error");
    uint64_t length = *(reinterpret_cast<uint64_t *>(buf.data() + sizeof(int32_t)));
    uint64_t dataLen = length - kHEADER_SIZE;
    buf.resize(length);
    if (dataLen == 0)
    {
        return buf;
    }
    ret = strm->receiveSome(buf.data() + kHEADER_SIZE, dataLen);
    ASSERT_EXIT(ret == dataLen, "recv error");
    return buf;
}

void Handler::sendMsg(TcpStream *strm, const std::vector<char> &msg)
{
    size_t ret = strm->sendAll(msg.data(), msg.size());
    ASSERT_EXIT(ret == msg.size(), "send error");
}