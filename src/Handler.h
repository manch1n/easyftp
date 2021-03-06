#pragma once

#include <vector>

#include "blocklib/include/TcpStream.h"
#include "src/CommandReply.h"

namespace myftp
{
    class Handler
    {
    public:
        Handler(const Handler &) = delete;
        Handler &operator=(const Handler &) = delete;
        Handler() = default;

    protected:
        std::vector<char> receiveMsg(TcpStream *strm);
        void sendMsg(TcpStream *strm, const std::vector<char> &msg);
    };
} // namespace myftp