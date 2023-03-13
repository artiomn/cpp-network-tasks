#pragma once
#include "tcp_server_6.h"


class Connector
{
public:
    Connector();
    socket_wrapper::Socket connect_to_client(unsigned short port);
    ~Connector();
private:

};