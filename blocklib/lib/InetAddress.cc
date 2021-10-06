#include "InetAddress.h"

#include <arpa/inet.h>
#include <assert.h>
#include <algorithm>
#include <netdb.h>

InetAddress::InetAddress(const std::string &ip, uint16_t port)
{
    int ret = -1;
    memZero(&addr6_, sizeof(addr6_));
    if (ip.find(":") == std::string::npos) //IPV4
    {
        ret = ::inet_pton(AF_INET, ip.c_str(), &addr_.sin_addr);
        addr_.sin_family = AF_INET;
        addr_.sin_port = ::htons(port);
    }
    else //IPV6
    {
        ret = ::inet_pton(AF_INET6, ip.c_str(), &addr6_.sin6_addr);
        addr6_.sin6_family = AF_INET6;
        addr6_.sin6_port = ::htons(port);
    }
    assert(ret == 1);
}

InetAddress::InetAddress(const struct sockaddr &saddr)
{
    if (saddr.sa_family == AF_INET)
    {
        ::memcpy(&addr_, &saddr, sizeof(addr_));
    }
    else if (saddr.sa_family == AF_INET6)
    {
        ::memcpy(&addr6_, &saddr, sizeof(addr6_));
    }
    else
    {
        assert(false);
    }
}

std::string InetAddress::toIp() const
{
    std::string buf(64, '\0');
    if (family() == AF_INET)
    {
        ::inet_ntop(AF_INET, &addr_.sin_addr, buf.data(), buf.size());
    }
    else
    {
        ::inet_ntop(AF_INET6, &addr6_.sin6_addr, buf.data(), buf.size());
    }
    auto zeroloc = buf.find('\0');
    buf.resize(zeroloc);
    return buf;
}

std::string InetAddress::toIpPort() const
{
    std::string ipstr = toIp();
    std::string portstr = std::to_string(port());
    return ipstr + " : " + portstr;
}

bool InetAddress::operator==(const InetAddress &rhs) const
{
    if (toIp() == rhs.toIp() && port() == rhs.port())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool InetAddress::resolve(const std::string &hostname, uint16_t port, InetAddress *outAddr)
{
    auto addrs = resolveAll(hostname, port);
    if (addrs.size() != 0)
    {
        *outAddr = addrs[0];
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<InetAddress> InetAddress::resolveAll(const std::string &hostname, uint16_t port)
{
    std::vector<InetAddress> addrs;
    struct addrinfo *rawAddrs;
    struct addrinfo hints;
    memZero(&hints, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    int ret = ::getaddrinfo(hostname.c_str(), "domain", &hints, &rawAddrs);
    if (ret != 0)
    {
        printf("error: %s \n", ::gai_strerror(ret));
    }
    struct addrinfo *cur = rawAddrs;
    while (cur != NULL)
    {
        if (cur->ai_family == AF_INET || cur->ai_family == AF_INET6)
        {
            std::string ipbuf(60, '\0');
            if (cur->ai_family == AF_INET)
            {
                struct sockaddr_in *saddr = reinterpret_cast<sockaddr_in *>(cur->ai_addr);
                ::inet_ntop(cur->ai_family, &saddr->sin_addr, ipbuf.data(), static_cast<socklen_t>(ipbuf.size()));
            }
            else
            {
                struct sockaddr_in6 *saddr = reinterpret_cast<sockaddr_in6 *>(cur->ai_addr);
                ::inet_ntop(cur->ai_family, &saddr->sin6_addr, ipbuf.data(), static_cast<socklen_t>(ipbuf.size()));
            }
            addrs.emplace_back(ipbuf, port);
            cur = cur->ai_next;
        }
    }
    freeaddrinfo(rawAddrs);
    return addrs;
}

InetAddress::InetAddress(uint16_t port, bool ipv6)
{
    if (ipv6)
    {
        memZero(&addr6_, sizeof(addr6_));
        addr6_.sin6_family = AF_INET6;
        addr6_.sin6_port = ::htons(port);
        addr6_.sin6_addr = in6addr_any;
    }
    else
    {
        memZero(&addr_, sizeof(addr_));
        addr_.sin_family = AF_INET;
        addr_.sin_addr.s_addr = ::htonl(INADDR_ANY);
        addr_.sin_port = ::htons(port);
    }
}
