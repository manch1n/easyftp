#include "ClientHandler.h"

using namespace myftp;

ClientHandler::ClientHandler(const std::string &dir, TcpStreamPtr &&netPtr) : fileio_(dir), netStrm_(std::move(netPtr))
{
    strToMethods_["LOCALLIST"] = std::bind(&ClientHandler::handleLOCAL_LIST, this);
    strToMethods_["REMOTELIST"] = std::bind(&ClientHandler::handleREMOTE_LIST, this);
    strToMethods_["EXIT"] = std::bind(&ClientHandler::handleEXIT, this);
    strToMethods_["RETRIEVE"] = std::bind(&ClientHandler::handleRETRIEVE, this);
    strToMethods_["STORE"] = std::bind(&ClientHandler::handleSTORE, this);
}

void ClientHandler::handleLOCAL_LIST()
{
    std::vector<std::string> files = fileio_.getLocalFiles();
    std::cout << "in diretory: " << fileio_.getCurDir() << std::endl;
    for (const std::string &fileName : files)
    {
        std::cout << fileName << " ";
    }
    std::cout << std::endl;
}

void ClientHandler::handleREMOTE_LIST()
{
    std::vector<char> msg = Interpreter::formatPacket(static_cast<int32_t>(Command::kREMOTE_LIST), std::vector<char>(0));
    sendMsg(netStrm_.get(), msg);
    std::vector<char> recvRaw = receiveMsg(netStrm_.get());
    Message recvMsg = Interpreter::parseRawMsg(recvRaw);
    if (static_cast<Reply>(recvMsg.cmdOrReply) == Reply::kFAIL)
    {
        printErr();
        return;
    }
    auto nameList = Interpreter::splitByUnitSeperator(recvMsg.data, false);
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
    if (strToMethods_.find(strs[0]) == strToMethods_.end())
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
            fileName_ = strs[1];
        }
    }
    strToMethods_[strs[0]]();
}

void ClientHandler::handleEXIT()
{
    auto exitPacket = Interpreter::formatPacket(static_cast<int32_t>(Command::kEXIT), std::vector<char>(0));
    sendMsg(netStrm_.get(), exitPacket);
    run_ = false;
}

void ClientHandler::handleRETRIEVE()
{
    auto packet = Interpreter::formatPacket(static_cast<int32_t>(Command::kRETRIEVE), std::vector<char>(fileName_.begin(), fileName_.end()));
    sendMsg(netStrm_.get(), packet);

    auto rawPacket = receiveMsg(netStrm_.get());
    auto msg = Interpreter::parseRawMsg(rawPacket);
    if (static_cast<Reply>(msg.cmdOrReply) == Reply::kFAIL)
    {
        printErr();
        return;
    }
    std::string fileName;
    size_t fileLen;
    Interpreter::parseFileInfo(msg.data, &fileLen, &fileName);

    fileio_.enrollFile(fileName, Fileio::RWOption::kWrite, fileLen);
    while (1)
    {
        std::vector<char> rawMsg = receiveMsg(netStrm_.get());
        Message msg = Interpreter::parseRawMsg(rawMsg);
        bool completed = fileio_.writeFilePiece(fileName, msg.data);
        if (completed == true)
        {
            break;
        }
    }

    std::cout << "Success" << std::endl;
}

void ClientHandler::handleSTORE()
{
    if (fileio_.fileExist(fileName_) == false)
    {
        printErr();
        return;
    }
    //auto pakcet = fileio_.readFile(fileName_);
    //auto msg = Interpreter::formatPacket(static_cast<int32_t>(Command::kSTORE), pakcet);
    //sendMsg(netStrm_.get(), msg);

    size_t fileSize = fileio_.getFileSize(fileName_);
    auto fileInfo = Interpreter::formatFileInfo(fileName_, fileSize);
    auto msg = Interpreter::formatPacket(static_cast<int32_t>(Command::kSTORE), fileInfo);
    sendMsg(netStrm_.get(), msg);

    fileio_.enrollFile(fileName_, Fileio::RWOption::kRead, fileSize);
    while (1)
    {
        std::vector<char> fileData;
        bool completed = fileio_.readFilePiece(fileName_, &fileData);
        auto msg = Interpreter::formatPacket(static_cast<int32_t>(Command::kTRANSFERING), fileData);
        sendMsg(netStrm_.get(), msg);
        if (completed == true)
        {
            break;
        }
    }

    auto rawResponse = receiveMsg(netStrm_.get());
    auto resMsg = Interpreter::parseRawMsg(rawResponse);
    if (resMsg.cmdOrReply != static_cast<int32_t>(Reply::kSUCCESS))
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
    return run_;
}

std::vector<std::string> ClientHandler::splitBySpace(const std::string &str) const
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
