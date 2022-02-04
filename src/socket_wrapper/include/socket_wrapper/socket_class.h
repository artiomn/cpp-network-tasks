#pragma once

namespace socket_wrapper
{

class Socket
{
public:
    Socket(int domain, int type, int protocol);
    Socket(int socket_descriptor);
    virtual ~Socket();

public:
    bool opened() const;

public:
    operator bool() const { return opened(); }
    operator int() const { return socket_descriptor_; }

protected:
    void open(int domain, int type, int protocol);
    int close();

private:
    int socket_descriptor_;
};

} // socket_wrapper

