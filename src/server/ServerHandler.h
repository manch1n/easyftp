#pragma once

#include <thread>
#include <map>
#include <functional>
#include <string>

#include "blocklib/include/Acceptor.h"

#include "src/Handler.h"
#include "src/Fileio.h"

namespace myftp
{
    class ClientStrm;
    class ServerHandler : public Handler
    {
    public:
        ServerHandler(uint16_t port);

        ServerHandler(const ServerHandler &) = delete;
        ServerHandler &operator=(const ServerHandler &) = delete;

        void start();

    private:
        struct ClientStrm
        {
            explicit ClientStrm(TcpStreamPtr &&ptr) : strm(std::move(ptr)), run(true) {}
            TcpStreamPtr strm;
            bool run;
        };

        void handleNewClient(TcpStreamPtr &&strm);

        void handleREMOTE_LIST(ClientStrm &strm, Message &msg);

        void handleSTORE(ClientStrm &strm, Message &msg);

        void handleRETRIEVE(ClientStrm &strm, Message &msg);

        void handleEXIT(ClientStrm &strm, Message &msg);

        void sendFailPacket(ClientStrm &strm);

        inline std::string vecStrToStr(const std::vector<char> &filename) const
        {
            return std::string(filename.begin(), filename.end());
        }

        bool run_ = true;

        Acceptor acceptor_;

        std::map<Command, std::function<void(ClientStrm &strm, Message &)>> cmdToMethod_;

        Fileio fileio_;
    };
} // namespace myftp