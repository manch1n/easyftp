#include "ClientHandler.h"

int main(int argc, char **argv)
{
    ASSERT_EXIT(argc == 2, "usage: ./client <serverip>");
    InetAddress servAddr(argv[1], 55555);
    TcpStreamPtr strm = TcpStream::connect(servAddr);

    ClientHandler handler("./myftplocal", std::move(strm));
    std::string userInput;
    bool run = handler.getRun();
    while (run && std::getline(std::cin, userInput))
    {
        handler.handleUserInput(userInput);
        run = handler.getRun();
    }
    return 0;
}