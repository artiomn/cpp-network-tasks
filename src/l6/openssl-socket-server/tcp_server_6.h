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

#include "file_proccesing.h"


#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>


class TCPserver
{
public:
    TCPserver(socket_wrapper::Socket&& client_sock);
    TCPserver(const TCPserver&) = delete;
    TCPserver() = delete;
    void server_run();
    ~TCPserver();
private:
    int client_sock_;
    File_Proccesing f_proc;
};






//==========================================================================
// VARIANT I - with file proccesing in tcp_server_6 class                 ||
//==========================================================================

// class TCPserver
// {
// public:
//     TCPserver(socket_wrapper::Socket&& client_sock);
//     TCPserver(const TCPserver&) = delete;
//     TCPserver() = delete;
//     std::string get_request();
//     void server_run();
//     bool send_file(fs::path const& file_path);
//     bool send_buffer(const std::vector<char>& buffer);
//     std::optional<fs::path> recv_file_path();
//     bool process();
//     ~TCPserver();
// private:
//     static bool need_to_repeat()
//     {
//         switch (errno)
//         {
//         case EINTR:
//         case EAGAIN:
//             // case EWOULDBLOCK: // EWOULDBLOCK == EINTR.
//             return true;
//         }

//         return false;
//     };
// private:
//     socket_wrapper::Socket client_sock_;
// };