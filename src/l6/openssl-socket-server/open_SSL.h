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

namespace fs = std::filesystem;
const auto buf_size = 4096;

#if defined(_WIN32)
const wchar_t separ = fs::path::preferred_separator;
#else
const wchar_t sep = *reinterpret_cast<const wchar_t*>(&fs::path::preferred_separator);
#endif

extern "C"
{
#include <openssl/ssl.h>
#include <openssl/err.h>
}

#if !defined(MAX_PATH)
#   define MAX_PATH (256)
#endif


class open_SSL
{
private:
  socket_wrapper::Socket client_sock_;
  const SSL_METHOD *meth {nullptr};
  SSL_CTX *ctx {nullptr};
  SSL *ssl = nullptr;
  private:
    static bool need_to_repeat()
    {
        switch (errno)
        {
        case EINTR:
        case EAGAIN:
            // case EWOULDBLOCK: // EWOULDBLOCK == EINTR.
            return true;
        }

        return false;
    };
public:
    open_SSL(socket_wrapper::Socket&& client_sock);
    bool ssl_init();
    std::string get_request();
    bool send_file(fs::path const& file_path);
    bool send_buffer(const std::vector<char>& buffer);
    std::optional<fs::path> recv_file_path();
    bool process();
    bool recv_packet(SSL *ssl);
    bool send_packet(const std::string &buf, SSL *ssl);
    void client_processing();
    void log_ssl();
    ~open_SSL();
};