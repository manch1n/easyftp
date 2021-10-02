#pragma once
#include "CommandReply.h"

#include "TcpStream.h"
class Handler
{
protected:
    std::vector<char> receiveMsg(TcpStream &strm);
    void sendMsg(TcpStream &strm, const std::vector<char> &msg);
    std::vector<std::vector<char>> splitByUnitSeperator(const std::vector<char> &s, bool splitOnce = false);

    const char unitSeperator = '\31';
};