#include "Socket.h"

#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <netinet/tcp.h>

#include "InetAddress.h" //FIXME F_D could be deleted
Socket::Socket(int sockfd) : sockfd_(sockfd)
{
}

Socket::~Socket()
{
    ::close(sockfd_);
}

void Socket::bindOrDie(const InetAddress &addr)
{
    int ret = ::bind(sockfd_, addr.get_sockaddr(), addr.length());
    assert(ret != -1);
}

void Socket::listenOrDie()
{
    int ret = ::listen(sockfd_, SOMAXCONN);
    assert(ret != -1);
}

int Socket::connect(const InetAddress &addr)
{
    int ret = ::connect(sockfd_, addr.get_sockaddr(), addr.length());
    return ret;
}

void Socket::shutdownWrite()
{
    ::shutdown(sockfd_, SHUT_WR);
}

void Socket::setReuseAddr(bool on)
{
    if (on)
    {
        int one = 1;
        int ret = ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        assert(ret != -1);
    }
}

void Socket::setTcpNoDelay(bool on)
{
    if (on)
    {
        int one = 1;
        int ret = ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
        assert(ret != -1);
    }
}

InetAddress Socket::getLocalAddr() const
{
    struct sockaddr localaddr;
    memZero(&localaddr, sizeof(localaddr));
    socklen_t len = sizeof(localaddr);
    int ret = ::getsockname(sockfd_, &localaddr, &len);
    if (ret == -1)
    {
        printf("err:%s\n", strerror(errno));
    }
    assert(ret != -1);
    return InetAddress(localaddr);
}

InetAddress Socket::getPeerAddr() const
{
    struct sockaddr peeraddr;
    memZero(&peeraddr, sizeof(peeraddr));
    socklen_t len = sizeof(peeraddr);
    int ret = ::getpeername(sockfd_, &peeraddr, &len);
    assert(ret != -1);
    return InetAddress(peeraddr);
}

int Socket::recv(void *buf, int len)
{
    int nrecv = 0;
    TEMP_FAILURE_RETRY(nrecv = ::recv(sockfd_, buf, len, 0));
    return nrecv;
}

int Socket::send(const void *buf, int len)
{
    int nsend = 0;
    TEMP_FAILURE_RETRY(nsend = ::send(sockfd_, buf, len, 0));
    return nsend;
}

Socket Socket::createTCP(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_STREAM, 0);
    return Socket(sockfd);
}

Socket Socket::createUDP(sa_family_t family)
{
    int sockfd = ::socket(family, SOCK_DGRAM, 0);
    return Socket(sockfd);
}
