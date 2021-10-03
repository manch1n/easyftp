#include "ServerHandler.h"

#include <vector>

#include "src/Interpreter.h"
#include "src/ErrCheck.h"

ServerHandler::ServerHandler(uint16_t port) : acceptor(InetAddress(port)), fileio("./myftpwkdir")
{
    cmdToMethod[Command::EXIT] = std::bind(&ServerHandler::handleEXIT, this, std::placeholders::_1, std::placeholders::_2);
    cmdToMethod[Command::REMOTE_LIST] = std::bind(&ServerHandler::handleREMOTE_LIST, this, std::placeholders::_1, std::placeholders::_2);
    cmdToMethod[Command::STORE] = std::bind(&ServerHandler::handleSTORE, this, std::placeholders::_1, std::placeholders::_2);
    cmdToMethod[Command::RETRIEVE] = std::bind(&ServerHandler::handleRETRIEVE, this, std::placeholders::_1, std::placeholders::_2);
}

void ServerHandler::start()
{
    while (run)
    {
        TcpStreamPtr newConn = acceptor.accept();
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
        std::vector<char> rawMsg = receiveMsg(*thisClient.strm);
        Message msg = Interpreter::parseRawMsg(rawMsg);
        Command remoteCmd = static_cast<Command>(msg.cmdOrReply);
        std::cout << "recv cmd: " << msg.cmdOrReply << std::endl;
        if (cmdToMethod.find(remoteCmd) != cmdToMethod.end())
        {
            cmdToMethod[remoteCmd](thisClient, msg);
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
    auto fileNames = fileio.getRemoteFiles();
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Reply::SUCCESS), fileNames);
    sendMsg(*(strm.strm), packet);
}

void ServerHandler::handleSTORE(ClientStrm &strm, Message &msg)
{
    std::cout << "call store" << std::endl;
    fileio.writeFile(msg.data);
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Reply::SUCCESS));
    sendMsg(*strm.strm, packet);
}

void ServerHandler::handleRETRIEVE(ClientStrm &strm, Message &msg)
{
    std::cout << "call retreive" << std::endl;
    std::string fileName = vecStrToStr(msg.data);
    if (!fileio.fileExist(fileName))
    {
        sendFailPacket(strm);
    }

    auto fileData = fileio.readFile(fileName);
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Reply::SUCCESS), fileData);
    sendMsg(*strm.strm, packet);
}

void ServerHandler::handleEXIT(ClientStrm &strm, Message &msg)
{
    std::cout << "call exit" << std::endl;
    strm.run = false;
    EASY_LOG("%s has exit\n", strm.strm->getPeerAddr().toIpPort().c_str());
}

void ServerHandler::sendFailPacket(ClientStrm &strm)
{
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Reply::FAIL), std::vector<char>(0));
    sendMsg(*strm.strm, packet);
}
