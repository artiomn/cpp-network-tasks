#pragma once

#include "socket_headers.h"

namespace socket_wrapper
{

class Socket
{
public:
    Socket(int domain, int type, int protocol);
    Socket(SocketdecriptorType socket_descriptor);
    virtual ~Socket();

public:
    bool opened() const;

public:
    operator bool() const { return opened(); }
    operator SocketdecriptorType() const { return socket_descriptor_; }

protected:
    void open(int domain, int type, int protocol);
    int close();

private:
    SocketdecriptorType socket_descriptor_;
};

} // socket_wrapper

