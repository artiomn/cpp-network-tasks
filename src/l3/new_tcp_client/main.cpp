include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include <socket_wrapper/socket_class.h>
#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>

const size_t command_size = 5;

// Trim from end (in place).
static inline std::string& rtrim(std::string& s)
{
    s.erase(std::find_if(
                s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); })
                .base());
    return s;
}

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <host> <port>" << std::endl;
        return EXIT_FAILURE;
    }

    socket_wrapper::SocketWrapper sock_wrap;
    socket_wrapper::Socket        sock = { AF_INET, SOCK_STREAM, IPPROTO_TCP };

    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    const std::string host_name = { argv[1] };
    //   const struct hostent *remote_host { gethostbyname(host_name.c_str()) };
    struct addrinfo  hints;
    struct addrinfo* result;
    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_STREAM;
    hints.ai_flags     = AI_PASSIVE;
    hints.ai_protocol  = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr      = NULL;
    hints.ai_next      = NULL;

    if (getaddrinfo(host_name.c_str(), NULL, &hints, &result) != 0)
    {
        std::cout << "Node is not available\n";
        return EXIT_FAILURE;
    }
    /*   struct sockaddr_in server_addr =
       {
           .sin_family = AF_INET,
           .sin_port = htons(std::stoi(argv[2]))
       };*/
    struct sockaddr_in server_addr =
        *reinterpret_cast<struct sockaddr_in*>(result->ai_addr);
    server_addr.sin_port = htons(std::stoi(argv[2]));
    //    server_addr.sin_addr.s_addr = *reinterpret_cast<const
    //    in_addr_t*>(remote_host->h_addr);
    socklen_t server_address_len = sizeof(server_addr);
    if (0 == connect(sock,
                     reinterpret_cast<const sockaddr* const>(&server_addr),
                     sizeof(server_addr)))
    {
        std::cout << "Connected to \"" << host_name << "\"..." << std::endl;
        while (true)
        {
            std::string          request;
            std::vector<uint8_t> buffer;
            buffer.resize(command_size);
            char buf[256];

            std::cin >> request;
            request += "\n";

            if (send(sock,
                     request.c_str(),
                     static_cast<int>(request.length()),
                     0) < 0)
            {
                std::cerr << sock_wrap.get_last_error_string() << std::endl;
                return EXIT_FAILURE;
            }
            recv(sock, &request[0], static_cast<size_t>(buffer.size()), 0);
            std::cout << request;
            std::cout << std::endl;
            for (int i = 0; i < 10; i++)
                buf[i] = '\0';
        }
    }
    sock.close();
    return EXIT_FAILURE;
}
