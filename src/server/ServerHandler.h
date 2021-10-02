#pragma once
#include "Handler.h"
#include "Acceptor.h"
#include <thread>
#include <map>
#include <functional>
#include <Fileio.h>

class ClientStrm;
class ServerHandler : public Handler
{
public:
    ServerHandler(uint16_t port);
    void start();

private:
    void handleNewClient(TcpStreamPtr &&strm);

    void handleREMOTE_LIST(ClientStrm &strm, Message &msg);

    void handleSTORE(ClientStrm &strm, Message &msg);

    void handleRETRIEVE(ClientStrm &strm, Message &msg);

    void handleEXIT(ClientStrm &strm, Message &msg);

    void sendFailPacket(ClientStrm &strm);

    inline std::string vecStrToStr(const std::vector<char> &filename)
    {
        return std::string(filename.begin(), filename.end());
    }

    bool run = true;

    Acceptor acceptor;

    std::map<Command, std::function<void(ClientStrm &strm, Message &)>> cmdToMethod;

    Fileio fileio;
};

struct ClientStrm
{
    explicit ClientStrm(TcpStreamPtr &&ptr) : strm(std::move(ptr)), run(true) {}
    TcpStreamPtr strm;
    bool run;
};