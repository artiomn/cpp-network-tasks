#pragma once
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cstdlib>
#include <cwctype>
#include <stdexcept>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <array>
#include <optional>
#include <filesystem>
#include <thread>
#include <vector>


#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>


class TCPclient
{
public:
    TCPclient(socket_wrapper::Socket&& server_sock);
    TCPclient(const TCPclient&) = delete;
    TCPclient() = delete;
    bool send_request(char *message);
    bool recv_packet(char *message);
    void proccesing();
    ~TCPclient();
private:
    int server_sock_;
    char message_[256];
};