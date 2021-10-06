#pragma once

#include <memory>

#include "Common.h"
#include "Socket.h"
#include "TcpStream.h"
#include "InetAddress.h"

typedef std::unique_ptr<TcpStream> TcpStreamPtr;

class Acceptor : noncopyable
{
public:
  explicit Acceptor(const InetAddress &listenAddr);

  //listenSock is a resource
  ~Acceptor() = default;
  Acceptor(Acceptor &&) = default;
  Acceptor &operator=(Acceptor &&) = default;

  // thread safe
  TcpStreamPtr accept();
  Socket acceptSocketOrDie();

private:
  Socket listenSock_;
};
