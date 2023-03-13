#include "openssl_socket_client.h"


Openssl_Socket_Client::Openssl_Socket_Client(socket_wrapper::Socket&& server_sock) 
    : server_sock_(std::move(server_sock)){}

bool Openssl_Socket_Client::ssl_init()
{
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();

    meth = TLS_client_method();
    ctx = SSL_CTX_new(meth);

    if (!ssl)
    {
        std::cerr << "Error creating SSL." << std::endl;
        log_ssl();
        return false;
    }
    return true;
}

bool Openssl_Socket_Client::recv_packet(SSL *ssl, const std::string &message)
{
    int len = 0;
    std::vector<char> buffer(4096);
    len = SSL_read(ssl, &(buffer.data()[0]), buffer.size());

    if (len > 0)
    {
        buffer.resize(len);
        std::fstream file;
        file.open(message, std::ios_base::out | std::ios_base::binary);

        if (file.is_open())
        {
            std::cout << "Received file!" << std::endl;
            for (auto &b : buffer)
            {
                std::cout << b;
            }
            std::cout << std::endl;
            file.write(&buffer[0], buffer.size());
        }
    }

    if (len < 0)
    {
        switch (SSL_get_error(ssl, len))
        {
        // Not an error.
        case SSL_ERROR_WANT_READ:
        case SSL_ERROR_WANT_WRITE:
            return true;
            break;
        case SSL_ERROR_ZERO_RETURN:
        case SSL_ERROR_SYSCALL:
        case SSL_ERROR_SSL:
            return false;
        }
    }

    return true;
}

bool Openssl_Socket_Client::send_packet(const std::string &buf, SSL *ssl)
{
    int len = SSL_write(ssl, buf.c_str(), buf.size());
    if (len < 0)
    {
        int err = SSL_get_error(ssl, len);
        switch (err)
        {
        case SSL_ERROR_WANT_WRITE:
        case SSL_ERROR_WANT_READ:
            return true;
            break;
        case SSL_ERROR_ZERO_RETURN:
        case SSL_ERROR_SYSCALL:
        case SSL_ERROR_SSL:
        default:
            return false;
        }
    }

    return false;
}

void Openssl_Socket_Client::log_ssl()
{
    for (int err = ERR_get_error(); err; err = ERR_get_error())
    {
        char *str = ERR_error_string(err, 0);
        if (!str)
            return;
        std::cerr << str << std::endl;
    }
}

void Openssl_Socket_Client::server_connect()
{
    ssl = SSL_new(ctx);

    if (!ssl)
    {
        std::cerr << "Error creating SSL." << std::endl;
        log_ssl();
        exit(-1);
    }

    SSL_set_fd(ssl, server_sock_);

    int err = SSL_connect(ssl);
    if (err <= 0)
    {
        std::cerr << "Error creating SSL connection.  err = " << err << std::endl;
        log_ssl();
        exit(-1);
    }

    std::cout << "SSL connection using " << SSL_get_cipher(ssl) << std::endl;

    while (true)
    {
        std::string message;
        printf("Enter message: ");
        std::getline(std::cin, message);

        // send the message
        send_packet(message, ssl);

        recv_packet(ssl, message);
    }
}


Openssl_Socket_Client::~Openssl_Socket_Client()
{
    SSL_shutdown(ssl);
}