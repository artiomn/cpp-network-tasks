#include "connector.h"

Connector::Connector() {}

socket_wrapper::Socket Connector::connect_to_server(unsigned short port)
{

    socket_wrapper::SocketWrapper sock_wrap;
    std::cout << "Starting TCP-client on the port " << port << "...\n";

    addrinfo hints =
        {
            .ai_family = AF_INET,
            .ai_socktype = SOCK_STREAM,
            .ai_protocol = IPPROTO_TCP};

    addrinfo *s_i = nullptr;
    int status = 0;

    if ((status = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &s_i)) != 0)
    {
        std::string msg{"getaddrinfo error: "};
        msg += gai_strerror(status);
        std::cout << msg;
        exit(EXIT_FAILURE);
    }

    std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> servinfo{s_i, freeaddrinfo};

    while (true)
    {
        for (auto const *s = servinfo.get(); s != nullptr; s = s->ai_next)
        {

            assert(s->ai_family == s->ai_addr->sa_family);
            if (AF_INET == s->ai_family)
            {
                char ip[INET_ADDRSTRLEN];

                sockaddr_in *const sin = reinterpret_cast<sockaddr_in *const>(s->ai_addr);
                sin->sin_family = AF_INET;
                sin->sin_port = htons(port);
                // sin->sin_addr.s_addr = INADDR_ANY; // not working
                inet_pton(AF_INET, "10.0.2.15", &sin->sin_addr);

                socket_wrapper::Socket s = {AF_INET, SOCK_STREAM, IPPROTO_TCP};

                if (!s)
                {
                    std::cout << "socket error" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (connect(s, reinterpret_cast<const sockaddr *>(sin), sizeof(sockaddr_in)))
                {
                    std::cout << "Connect IPv4 error!" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return s;
            }
            else if (AF_INET6 == s->ai_family)
            {
                char ip6[INET6_ADDRSTRLEN];

                sockaddr_in6 *const sin = reinterpret_cast<sockaddr_in6 *const>(s->ai_addr);

                sin->sin6_family = AF_INET6;
                sin->sin6_port = htons(port);
                // sin->sin6_addr = in6addr_any; // not working
                inet_pton(AF_INET6, "", &sin->sin6_addr);

                socket_wrapper::Socket s = {AF_INET6, SOCK_STREAM, IPPROTO_TCP};

                if (!s)
                {
                    std::cout << "Socket error!" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (connect(s, reinterpret_cast<const sockaddr *>(sin), sizeof(sockaddr_in6)) < 0)
                {
                    std::cout << "Connect IPv6 error!" << std::endl;
                    exit(EXIT_FAILURE);
                }
                return s;
            }
        } // for
    }// while
}

Connector::~Connector() {}