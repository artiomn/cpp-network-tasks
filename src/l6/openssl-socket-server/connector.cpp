#include "connector.h"


// Connector
//==================================================================================
//==================================================================================

Connector::Connector() {}

socket_wrapper::Socket Connector::connect_to_client(unsigned short port)
{
    socket_wrapper::SocketWrapper sock_wrap_;

    sockaddr_storage clients_addr = { 0 };
    socklen_t clients_addr_size;
    const int flag = 1;

    addrinfo hints =
    {
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_INET,
        // TCP stream-sockets.
        .ai_socktype = SOCK_STREAM,
        // Any protocol.
        .ai_protocol = IPPROTO_TCP
    };

    addrinfo* s_i = nullptr;
    int status = 0;

    if ((status = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &s_i)) != 0)
    {
        std::string msg{ "getaddrinfo error: " };
        msg += gai_strerror(status);
        std::cout << msg;
        //throw std::runtime_error(msg);
        exit(EXIT_FAILURE);
    }

    std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> servinfo{ s_i, freeaddrinfo };

    while (true)
    {
        for (auto const* s = servinfo.get(); s != nullptr; s = s->ai_next)
        {

            assert(s->ai_family == s->ai_addr->sa_family);
            if (AF_INET == s->ai_family)
            {
                char ip[INET_ADDRSTRLEN];

                sockaddr_in* const sin = reinterpret_cast<sockaddr_in* const>(s->ai_addr);

                sin->sin_family = AF_INET;
                sin->sin_port = htons(port);
                //sin->sin_addr.s_addr = INADDR_ANY;
                inet_pton(AF_INET, "10.0.2.15", &sin->sin_addr);

                socket_wrapper::Socket s = { AF_INET, SOCK_STREAM, IPPROTO_TCP };

                // Allow reuse of port.
                if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&flag), sizeof(flag)) < 0)
                {
                    std::cout << "Set SO_REUSEADDR error" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if ((bind(s, reinterpret_cast<const sockaddr*>(sin), sizeof(sockaddr_in))) != 0)
                {
                    std::cerr << sock_wrap_.get_last_error_string() << std::endl;
                    std::cerr << "bind error" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if ((listen(s, 10)) == -1)
                {
                    std::cerr << "sever: listen\n";
                    exit(EXIT_FAILURE);
                }


                std::cout << "Trying IP Address: " << inet_ntop(AF_INET, &sin->sin_addr.s_addr, ip, INET_ADDRSTRLEN) << std::endl;
                clients_addr_size = sizeof(clients_addr);

                socket_wrapper::Socket newSock = accept(s, reinterpret_cast<sockaddr*>(&clients_addr), &clients_addr_size);
                if (!newSock)
                {
                    std::cerr << sock_wrap_.get_last_error_string() << std::endl;
                    std::cout << "accept error";
                    exit(EXIT_FAILURE);
                }
                s.close();
                return newSock;
            }
            else if (AF_INET6 == s->ai_family)
            {
                char ip6[INET6_ADDRSTRLEN];

                sockaddr_in6* const sin = reinterpret_cast<sockaddr_in6* const>(s->ai_addr);

                sin->sin6_family = AF_INET6;
                sin->sin6_port = htons(port);
                sin->sin6_addr = in6addr_any;
                //inet_pton(AF_INET6, "", &sin->sin6_addr);

                socket_wrapper::Socket s = { AF_INET6, SOCK_STREAM, IPPROTO_TCP };

                // Allow reuse of port.
                if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&flag), sizeof(flag)) < 0)
                {
                    std::cout << "Set SO_REUSEADDR error" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if ((bind(s, reinterpret_cast<const sockaddr*>(sin), sizeof(sockaddr_in6))) != 0)
                {
                    std::cerr << sock_wrap_.get_last_error_string() << std::endl;
                    std::cerr << "bind error" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if ((listen(s, 10)) == -1)
                {
                    std::cerr << "sever: listen\n";
                    exit(EXIT_FAILURE);
                }

                std::cout << "Trying IPv6 Address: " << inet_ntop(AF_INET6, &sin->sin6_addr, ip6, INET6_ADDRSTRLEN) << std::endl;
                clients_addr_size = sizeof(clients_addr);

                socket_wrapper::Socket newSock = accept(s, reinterpret_cast<sockaddr*>(&clients_addr), &clients_addr_size);
                if (!newSock)
                {
                    std::cerr << sock_wrap_.get_last_error_string() << std::endl;
                    std::cout << "accept error";
                    exit(EXIT_FAILURE);
                }
                s.close();
                return newSock;
            }
        }  // for
    }  // while
}

Connector::~Connector() {}