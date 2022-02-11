#pragma once

#include "socket_headers.h"

namespace socket_wrapper
{

class Socket
{
public:
    Socket(int domain, int type, int protocol);
    Socket(SocketDescriptorType socket_descriptor);
    virtual ~Socket();

public:
    bool opened() const;

public:
    operator bool() const { return opened(); }
    operator SocketDescriptorType() const { return socket_descriptor_; }

public:
    int close();

protected:
    void open(int domain, int type, int protocol);

private:
    SocketDescriptorType socket_descriptor_;
};

} // socket_wrapper

