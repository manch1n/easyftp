#include "Acceptor.h"

#include "assert.h"

Acceptor::Acceptor(const InetAddress &listenAddr) : listenSock_(std::move(Socket::createTCP(AF_INET)))
{
    listenSock_.bindOrDie(listenAddr);
    listenSock_.listenOrDie();
}

TcpStreamPtr Acceptor::accept()
{
    int sockfd = ::accept(listenSock_.fd(), NULL, NULL);
    assert(sockfd != -1);
    return std::make_unique<TcpStream>(Socket(sockfd));
}

Socket Acceptor::acceptSocketOrDie()
{
    int sockfd = ::accept(listenSock_.fd(), NULL, NULL);
    assert(sockfd != -1);
    return Socket(sockfd);
}
