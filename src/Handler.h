#pragma once

#include <vector>

#include "blocklib/include/TcpStream.h"
#include "src/CommandReply.h"

namespace myftp
{
    class Handler
    {
    public:
    protected:
        std::vector<char> receiveMsg(TcpStream *strm);
        void sendMsg(TcpStream *strm, const std::vector<char> &msg);
        std::vector<std::vector<char>> splitByUnitSeperator(
            const std::vector<char> &s, bool splitOnce) const;
    };
} // namespace myftp