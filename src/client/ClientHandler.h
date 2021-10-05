#pragma once

#include <map>
#include <functional>
#include <string>
#include <vector>

#include "blocklib/include/TcpStream.h"

#include "src/Interpreter.h"
#include "src/Fileio.h"
#include "src/Handler.h"

namespace myftp
{
    class ClientHandler : public Handler
    {
    public:
        explicit ClientHandler(const std::string &dir, TcpStreamPtr &&netPtr);

        ClientHandler(const ClientHandler &) = delete;
        ClientHandler &operator=(const ClientHandler &) = delete;

        void handleLOCAL_LIST();
        void handleREMOTE_LIST();
        void handleUserInput(const std::string &userInput);
        void handleEXIT();
        void handleRETRIEVE();
        void handleSTORE();
        bool getRun();

    private:
        inline std::string vecstrConvertToStr(const std::vector<char> &vecstr) const
        {
            std::string str;
            str.assign(vecstr.begin(), vecstr.end());
            return str;
        }

        std::vector<std::string> splitBySpace(const std::string &str) const;

        inline void printErr() const
        {
            std::cout << "fail,something wrong. try again." << std::endl;
        }
        Fileio fileio_;
        std::map<std::string, std::function<void(void)>> strToMethods_;
        TcpStreamPtr netStrm_;
        std::string fileName_;
        bool run_ = true;
    };
} //namespace myftp