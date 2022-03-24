#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

extern "C"
{
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
}


const auto buff_size = 1024;


bool print_error(const std::string &msg)
{
    perror(msg.c_str());
    ERR_print_errors_fp(stderr);

    return false;
}


void init_ssl()
{
    SSL_load_error_strings();
    SSL_library_init();
}


void cleanup(SSL_CTX* ctx, BIO* bio)
{
    SSL_CTX_free(ctx);
    BIO_free_all(bio);
}


bool secure_connect(const std::string &hostname)
{
    std::stringstream request;
    std::string response;

    auto port_start = hostname.find(':');
    auto host = (port_start == std::string::npos) ? hostname : hostname.substr(0, port_start);

    response.resize(buff_size);

    const SSL_METHOD* method = TLS_client_method();
    if (nullptr == method) return print_error("TLS_client_method...");

    SSL_CTX* ctx = SSL_CTX_new(method);
    if (nullptr == ctx) return print_error("SSL_CTX_new...");

    BIO* bio = BIO_new_ssl_connect(ctx);
    if (nullptr == bio) return print_error("BIO_new_ssl_connect...");

    SSL* ssl = nullptr;

    // Link BIO channel, SSL session, and server endpoint.
    BIO_get_ssl(bio, &ssl); /* session */
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY); /* robustness */
    std::cout << "Connecting to \"" << host << "\"..." << std::endl;
    BIO_set_conn_hostname(bio, host.c_str());
    BIO_set_conn_port(bio, "443");

    // Trying to connect.
    if (BIO_do_connect(bio) <= 0)
    {
        cleanup(ctx, bio);
        print_error("BIO_do_connect...");
    }

    // Verifying truststore, check cert.
    if (!SSL_CTX_load_verify_locations(ctx, "/etc/ssl/certs/ca-certificates.crt", "/etc/ssl/certs/"))
        return print_error("SSL_CTX_load_verify_locations...");

    long verify_flag = SSL_get_verify_result(ssl);
    if (verify_flag != X509_V_OK)
      std::cerr << "##### Certificate verification error (" << verify_flag << ") but continuing..." << std::endl;

    // Fetch the homepage as sample data.
    request
        << "GET / HTTP/1.1\x0D\x0A"
        << "Host: " << hostname << "\x0D\x0A"
        << "Connection: Close\x0D\x0A\x0D\x0A";
    BIO_puts(bio, request.str().c_str());

    // Read HTTP response from server and print to stdout.
    while (true)
    {
        std::fill(response.begin(), response.end(), response.size());
        int n = BIO_read(bio, &response[0], response.size());
        // 0 is end-of-stream, < 0 is an error.
        if (n <= 0) break;
        std::cout << response << std::endl;
    }

    cleanup(ctx, bio);
    return true;
}


int main(int argc, const char * const argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <host>" << std::endl;
        return EXIT_FAILURE;
    }

    init_ssl();

    std::string hostname = argv[1];
    std::cerr << "Trying an HTTPS connection to " << hostname << "..." << std::endl;
    if (!secure_connect(hostname)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

