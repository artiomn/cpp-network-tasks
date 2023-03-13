#include "openssl_socket_client.h"
#include "connector.h"
#include "tcp_client_6.h"


int main(int argc, const char * const argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    const int port{ std::stoi(argv[1]) };
    socket_wrapper::SocketWrapper sock_wrap;
    Connector connector;
    socket_wrapper::Socket sock = connector.connect_to_server(port);

    TCPclient cl(std::move(sock));
    cl.proccesing();

    // Openssl_Socket_Client osc(std::move(sock));
    // osc.ssl_init();
    // osc.server_connect();

    return EXIT_SUCCESS;
}







// VARIANT I - all in 1 main file

// #include <iostream>
// #include <algorithm>
// #include <cstdlib>
// #include <cassert>
// #include <iomanip>
// #include <iostream>
// #include <string>
// #include <cstring>
// #include <vector>
// #include <filesystem>
// #include <fstream>

// #include <socket_wrapper/socket_headers.h>
// #include <socket_wrapper/socket_wrapper.h>
// #include <socket_wrapper/socket_class.h>


// extern "C"
// {
// #include <openssl/ssl.h>
// #include <openssl/err.h>
// }


// const auto buffer_size = 256;


// bool recv_packet(SSL *ssl, const std::string &message)
// {
//     // char buf[buffer_size];
//     int len = 0;

//     // do
//     // {
//     //     len = SSL_read(ssl, buf, buffer_size - 1);
//     //     buf[len] = 0;
//     //     std::cout << buf << std::endl;
//     // }
//     // while (len > 0);

//     std::vector<char> buffer(4096);
//     len = SSL_read(ssl, &(buffer.data()[0]), buffer.size());

//     if (len > 0)
//     {
//         buffer.resize(len);
//         std::fstream file;
//         file.open(message, std::ios_base::out | std::ios_base::binary);

//         if (file.is_open())
//         {
//             std::cout << "Received file!" << std::endl;
//             for (auto& b : buffer)
//             {
//                 std::cout << b;
//             }
//             std::cout << std::endl;
//             file.write(&buffer[0], buffer.size());
//         }
//     }

//     if (len < 0)
//     {
//         switch (SSL_get_error(ssl, len))
//         {
//             // Not an error.
//             case SSL_ERROR_WANT_READ:
//             case SSL_ERROR_WANT_WRITE:
//                 return true;
//             break;
//             case SSL_ERROR_ZERO_RETURN:
//             case SSL_ERROR_SYSCALL:
//             case SSL_ERROR_SSL:
//                 return false;
//         }
//     }

//     return true;
// }


// bool send_packet(const std::string &buf, SSL *ssl)
// {
//     int len = SSL_write(ssl, buf.c_str(), buf.size());
//     if (len < 0)
//     {
//         int err = SSL_get_error(ssl, len);
//         switch (err)
//         {
//             case SSL_ERROR_WANT_WRITE:
//             case SSL_ERROR_WANT_READ:
//                 return true;
//             break;
//             case SSL_ERROR_ZERO_RETURN:
//             case SSL_ERROR_SYSCALL:
//             case SSL_ERROR_SSL:
//             default:
//                 return false;
//         }
//     }

//     return false;
// }


// void log_ssl()
// {
//     for (int err = ERR_get_error(); err; err = ERR_get_error())
//     {
//         char *str = ERR_error_string(err, 0);
//         if (!str) return;
//         std::cerr << str << std::endl;
//     }
// }


// int main(int argc, const char * const argv[])
// {
//     if (argc != 2)
//     {
//         std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
//         return EXIT_FAILURE;
//     }

//     const int port{ std::stoi(argv[1]) };
//     //const int port{ std::stoi("15234")};
//     socket_wrapper::SocketWrapper sock_wrap;
    


//     std::cout << "Starting TCP-client on the port " << port << "...\n";


//     addrinfo hints =
//     {
//         .ai_family = AF_INET,
//         .ai_socktype = SOCK_STREAM,
//         .ai_protocol = IPPROTO_TCP
//     };

//     addrinfo* s_i = nullptr;
//     int status = 0;

//     if ((status = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &s_i)) != 0)
//     {
//         std::string msg{ "getaddrinfo error: " };
//         msg += gai_strerror(status);
//         std::cout << msg;
//         exit(EXIT_FAILURE);
//     }

//     std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> servinfo{ s_i, freeaddrinfo };

//     for (auto const* s = servinfo.get(); s != nullptr; s = s->ai_next)
//     {

//         assert(s->ai_family == s->ai_addr->sa_family);
//         if (AF_INET == s->ai_family)
//         {
//             char ip[INET_ADDRSTRLEN];

//             sockaddr_in* const sin = reinterpret_cast<sockaddr_in* const>(s->ai_addr);
//             sin->sin_family = AF_INET;
//             sin->sin_port = htons(port);
//             //sin->sin_addr.s_addr = INADDR_ANY; // not working 
//             inet_pton(AF_INET, "10.0.2.15", &sin->sin_addr);

//             socket_wrapper::Socket s = { AF_INET, SOCK_STREAM, IPPROTO_TCP };

//             if (!s)
//             {
//                 std::cout << "socket error" << std::endl;
//                 exit(EXIT_FAILURE);
//             }

//             if (connect(s, reinterpret_cast<const sockaddr*>(sin), sizeof(sockaddr_in)))
//             {
//                 std::cout << "Connect IPv4 error!" << std::endl;
//                 return EXIT_FAILURE;
//             }

        
//             SSL *ssl = nullptr;

//             SSL_library_init();
//             SSLeay_add_ssl_algorithms();
//             SSL_load_error_strings();

//             const SSL_METHOD *meth = TLS_client_method();
//             SSL_CTX *ctx = SSL_CTX_new(meth);
//             ssl = SSL_new(ctx);

//             if (!ssl)
//             {
//                 std::cerr << "Error creating SSL." << std::endl;
//                 log_ssl();
//                 return EXIT_FAILURE;
//            }

//            SSL_set_fd(ssl, s);

//            int err = SSL_connect(ssl);
//            if (err <= 0)
//            {
//                 std::cerr << "Error creating SSL connection.  err = " << err << std::endl;
//                 log_ssl();
//                 return EXIT_FAILURE;
//            }
//            std::cout << "SSL connection using " << SSL_get_cipher(ssl) << std::endl;

    

//            while (true)
//            {
//                 std::string message;
//                 printf("Enter message: ");
//                 std::getline(std::cin, message);

//                 // send the message
//                 send_packet(message, ssl);

//                 recv_packet(ssl, message);
//                 // std::vector<char> buffer(4096);
//                 // int reply = SSL_read(ssl, &(buffer.data()[0]), buffer.size());

//                 // if (reply > 0)
//                 // {
//                 //     buffer.resize(reply);
//                 //     std::fstream file;
//                 //     file.open(message, std::ios_base::out | std::ios_base::binary);

//                 //     if (file.is_open())
//                 //     {
//                 //         std::cout << "Received file!" << std::endl;
//                 //         for (auto& b : buffer)
//                 //         {
//                 //             std::cout << b;
//                 //         }
//                 //         std::cout << std::endl;
//                 //         file.write(&buffer[0], buffer.size());
//                 //     }
//                 // }
             
               
//             }
//             SSL_shutdown(ssl);
     
//         }
//         else if (AF_INET6 == s->ai_family)
//         {
//             char ip6[INET6_ADDRSTRLEN];

//             sockaddr_in6* const sin = reinterpret_cast<sockaddr_in6* const>(s->ai_addr);

//             sin->sin6_family = AF_INET6;
//             sin->sin6_port = htons(port);
//             //sin->sin6_addr = in6addr_any; // not working 
//             inet_pton(AF_INET6, "", &sin->sin6_addr);

//             socket_wrapper::Socket s = { AF_INET6, SOCK_STREAM, IPPROTO_TCP };

//             if (!s)
//             {
//                 std::cout << "Socket error!" << std::endl;
//                 exit(EXIT_FAILURE);
//             }

//             if (connect(s, reinterpret_cast<const sockaddr*>(sin), sizeof(sockaddr_in6)) < 0)
//             {
//                 std::cout << "Connect IPv6 error!" << std::endl;
//                 return EXIT_FAILURE;
//             }

            

//             while (true)
//             {

//                 char message[256];
//                 printf("Enter message: ");
//                 std::cin.getline(message, 256);

//                 // send the message
//                 send(s, message, strlen(message), 0);


//                 std::vector<char> buffer(256);
//                 recv(s, &(buffer.data()[0]), buffer.size(), 0);

//                 if (!buffer.empty())
//                 {
//                     std::fstream file;
//                     file.open(message, std::ios_base::out | std::ios_base::binary);

//                     if (file.is_open())
//                     {
//                         std::cout << "Received file!" << std::endl;
//                         for (auto& b : buffer)
//                         {
//                             std::cout << b;
//                         }
//                         std::cout << std::endl;
//                         file.write(&buffer[0], buffer.size());
//                     }
//                 }
//             }
//         }
//     }  // for

//     return EXIT_SUCCESS;
// }