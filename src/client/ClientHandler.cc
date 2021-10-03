#include "ClientHandler.h"

using namespace myftp;

ClientHandler::ClientHandler(const std::string &dir, TcpStreamPtr &&netPtr) : fileio(dir), netStrm(std::move(netPtr))
{
    strToMethods["LOCALLIST"] = std::bind(&ClientHandler::handleLOCAL_LIST, this);
    strToMethods["REMOTELIST"] = std::bind(&ClientHandler::handleREMOTE_LIST, this);
    strToMethods["EXIT"] = std::bind(&ClientHandler::handleEXIT, this);
    strToMethods["RETRIEVE"] = std::bind(&ClientHandler::handleRETRIEVE, this);
    strToMethods["STORE"] = std::bind(&ClientHandler::handleSTORE, this);
}

void ClientHandler::handleLOCAL_LIST()
{
    std::vector<std::string> files = fileio.getLocalFiles();
    std::cout << "in diretory: " << fileio.getCurDir() << std::endl;
    for (const std::string &fileName : files)
    {
        std::cout << fileName << " ";
    }
    std::cout << std::endl;
}

void ClientHandler::handleREMOTE_LIST()
{
    std::vector<char> msg = Interpreter::formatPacket(static_cast<int32_t>(Command::REMOTE_LIST), std::vector<char>(0));
    sendMsg(*netStrm, msg);
    std::vector<char> recvRaw = receiveMsg(*netStrm);
    Message recvMsg = Interpreter::parseRawMsg(recvRaw);
    if (static_cast<Reply>(recvMsg.cmdOrReply) == Reply::FAIL)
    {
        printErr();
        return;
    }
    auto nameList = splitByUnitSeperator(recvMsg.data);
    for (const std::vector<char> &fileName : nameList)
    {
        std::cout << vecstrConvertToStr(fileName) << " ";
    }
    std::cout << std::endl;
}

void ClientHandler::handleUserInput(const std::string &userInput)
{
    std::vector<std::string> strs = splitBySpace(userInput);
    if (strs.size() == 0)
    {
        printErr();
        return;
    }

    std::transform(strs[0].begin(), strs[0].end(), strs[0].begin(), ::toupper);
    if (strToMethods.find(strs[0]) == strToMethods.end())
    {
        printErr();
        return;
    }

    if (strs[0] == "RETRIEVE" || strs[0] == "STORE")
    {
        if (strs.size() != 2)
        {
            printErr();
        }
        else
        {
            fileName = strs[1];
        }
    }
    strToMethods[strs[0]]();
}

void ClientHandler::handleEXIT()
{
    auto exitPacket = Interpreter::formatPacket(static_cast<int32_t>(Command::EXIT), std::vector<char>(0));
    sendMsg(*netStrm, exitPacket);
    run = false;
}

void ClientHandler::handleRETRIEVE()
{
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Command::RETRIEVE), std::vector<char>(fileName.begin(), fileName.end()));
    sendMsg(*netStrm, packet);

    auto rawPacket = receiveMsg(*netStrm);
    auto msg = Interpreter::parseRawMsg(rawPacket);
    if (static_cast<Reply>(msg.cmdOrReply) == Reply::FAIL)
    {
        printErr();
        return;
    }
    fileio.writeFile(msg.data);
    std::cout << "Success" << std::endl;
}

void ClientHandler::handleSTORE()
{
    if (fileio.fileExist(fileName) == false)
    {
        printErr();
        return;
    }
    auto pakcet = fileio.readFile(fileName);
    auto msg = Interpreter::formatPacket(static_cast<int32_t>(Command::STORE), pakcet);
    sendMsg(*netStrm, msg);
    auto rawResponse = receiveMsg(*netStrm);
    auto resMsg = Interpreter::parseRawMsg(rawResponse);
    if (resMsg.cmdOrReply != static_cast<int32_t>(Reply::SUCCESS))
    {
        printErr();
    }
    else
    {
        std::cout << "Success" << std::endl;
    }
}

bool ClientHandler::getRun()
{
    return run;
}

std::vector<std::string> ClientHandler::splitBySpace(const std::string &str)
{
    std::vector<std::string> ret;
    size_t lastPos = str.find_first_not_of(' ', 0);
    size_t pos = str.find_first_of(' ', lastPos);
    while (pos != std::string::npos || lastPos != std::string::npos)
    {
        ret.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(' ', pos);
        pos = str.find_first_of(' ', lastPos);
    }
    return ret;
}
