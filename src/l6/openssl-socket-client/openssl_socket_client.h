#pragma once
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <filesystem>
#include <fstream>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>


extern "C"
{
#include <openssl/ssl.h>
#include <openssl/err.h>
}

const auto buffer_size = 256;

class Openssl_Socket_Client
{
public:
    Openssl_Socket_Client(socket_wrapper::Socket&& server_sock);
    bool ssl_init();
    void server_connect();
    bool send_packet(const std::string &buf, SSL *ssl);
    bool recv_packet(SSL *ssl, const std::string &message);
    void log_ssl();
    ~Openssl_Socket_Client();

private:
    socket_wrapper::Socket server_sock_;
    SSL *ssl = nullptr;
    const SSL_METHOD *meth{nullptr};
    SSL_CTX *ctx{nullptr};
};