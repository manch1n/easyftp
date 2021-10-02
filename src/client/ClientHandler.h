#pragma once
#include "Interpreter.h"
#include "Fileio.h"
#include "Handler.h"
#include "TcpStream.h"
#include <map>
#include <functional>

class ClientHandler : public Handler
{
public:
    explicit ClientHandler(const std::string &dir, TcpStreamPtr &&netPtr);

    void handleLOCAL_LIST();
    void handleREMOTE_LIST();
    void handleUserInput(const std::string &userInput);
    void handleEXIT();
    void handleRETRIEVE();
    void handleSTORE();
    bool getRun();

private:
    inline std::string vecstrConvertToStr(const std::vector<char> &vecstr)
    {
        std::string str;
        str.assign(vecstr.begin(), vecstr.end());
        return str;
    }

    std::vector<std::string> splitBySpace(const std::string &str);

    inline void printErr()
    {
        std::cout << "fail,something wrong. try again." << std::endl;
    }
    Fileio fileio;
    std::map<std::string, std::function<void(void)>> strToMethods;
    TcpStreamPtr netStrm;
    std::string fileName;
    bool run = true;
};