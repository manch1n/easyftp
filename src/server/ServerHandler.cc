#include "ServerHandler.h"

#include <vector>

#include "src/Interpreter.h"
#include "src/ErrCheck.h"

using namespace myftp;

ServerHandler::ServerHandler(uint16_t port) : acceptor_(InetAddress(port)), fileio_("./myftpwkdir")
{
    cmdToMethod_[Command::kEXIT] = std::bind(&ServerHandler::handleEXIT, this, std::placeholders::_1, std::placeholders::_2);
    cmdToMethod_[Command::kREMOTE_LIST] = std::bind(&ServerHandler::handleREMOTE_LIST, this, std::placeholders::_1, std::placeholders::_2);
    cmdToMethod_[Command::kSTORE] = std::bind(&ServerHandler::handleSTORE, this, std::placeholders::_1, std::placeholders::_2);
    cmdToMethod_[Command::kRETRIEVE] = std::bind(&ServerHandler::handleRETRIEVE, this, std::placeholders::_1, std::placeholders::_2);
}

void ServerHandler::start()
{
    while (run_)
    {
        TcpStreamPtr newConn = acceptor_.accept();
        newConn->setTcpNoDelay(true);
        std::thread worker(&ServerHandler::handleNewClient, this, std::move(newConn));
        worker.detach();
    }
}

void ServerHandler::handleNewClient(TcpStreamPtr &&strm)
{
    ClientStrm thisClient(std::move(strm));
    while (thisClient.run)
    {
        std::vector<char> rawMsg = receiveMsg(thisClient.strm.get());
        Message msg = Interpreter::parseRawMsg(rawMsg);
        Command remoteCmd = static_cast<Command>(msg.cmdOrReply);
        std::cout << "recv cmd: " << msg.cmdOrReply << std::endl;
        if (cmdToMethod_.find(remoteCmd) != cmdToMethod_.end())
        {
            cmdToMethod_[remoteCmd](thisClient, msg);
        }
        else
        {
            sendFailPacket(thisClient);
        }
    }
}

void ServerHandler::handleREMOTE_LIST(ClientStrm &strm, Message &msg)
{
    std::cout << "call rmeotelist" << std::endl;
    auto fileNames = fileio_.getRemoteFiles();
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Reply::kSUCCESS), fileNames);
    sendMsg(strm.strm.get(), packet);
}

void ServerHandler::handleSTORE(ClientStrm &strm, Message &msg)
{
    std::cout << "call store" << std::endl;
    fileio_.writeFile(msg.data);
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Reply::kSUCCESS));
    sendMsg(strm.strm.get(), packet);
}

void ServerHandler::handleRETRIEVE(ClientStrm &strm, Message &msg)
{
    std::cout << "call retreive" << std::endl;
    std::string fileName = vecStrToStr(msg.data);
    if (!fileio_.fileExist(fileName))
    {
        sendFailPacket(strm);
    }

    auto fileData = fileio_.readFile(fileName);
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Reply::kSUCCESS), fileData);
    sendMsg(strm.strm.get(), packet);
}

void ServerHandler::handleEXIT(ClientStrm &strm, Message &msg)
{
    std::cout << "call exit" << std::endl;
    strm.run = false;
    EASY_LOG("%s has exit\n", strm.strm->getPeerAddr().toIpPort().c_str());
}

void ServerHandler::sendFailPacket(ClientStrm &strm)
{
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Reply::kFAIL), std::vector<char>(0));
    sendMsg(strm.strm.get(), packet);
}
