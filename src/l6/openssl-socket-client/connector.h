#pragma once
#include "openssl_socket_client.h"


class Connector
{
public:
    Connector();
    socket_wrapper::Socket connect_to_server(unsigned short port);
    ~Connector();
};