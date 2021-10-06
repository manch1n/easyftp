#include "TcpStream.h"

#include <assert.h>

TcpStream::TcpStream(Socket &&sock) : sock_(std::move(sock))
{
}

TcpStreamPtr TcpStream::connect(const InetAddress &serverAddr)
{
    Socket sock = Socket::createTCP(AF_INET);
    int ret = sock.connect(serverAddr);
    assert(ret != -1);
    return std::make_unique<TcpStream>(std::move(sock));
}

int TcpStream::receiveAll(void *buf, int len)
{
    int nleft = len;
    while (nleft)
    {
        int nread = sock_.recv(static_cast<char *>(buf) + len - nleft, nleft);
        if (nread <= 0)
        {
            return len - nleft;
        }
        nleft -= nread;
    }
    return len;
}

int TcpStream::receiveSome(void *buf, int len)
{
    return sock_.recv(buf, len);
}

int TcpStream::sendAll(const void *buf, int len)
{
    int nleft = len;
    while (nleft)
    {
        int nsend = sock_.send(static_cast<const char *>(buf) + len - nleft, nleft);
        if (nsend <= 0)
        {
            return len - nleft;
        }
        nleft -= nsend;
    }
    return len;
}

int TcpStream::sendSome(const void *buf, int len)
{
    return sock_.send(buf, len);
}

void TcpStream::setTcpNoDelay(bool on)
{
    sock_.setTcpNoDelay(on);
}

void TcpStream::shutdownWrite()
{
    sock_.shutdownWrite();
}
