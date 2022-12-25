#include <iostream>
#include <string.h>

#include <arpa/inet.h>
#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>


#define MSG_LENGTH 256

int main(int argc, char const *argv[])
{

    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <ip-address> <port>" << std::endl;
        return EXIT_FAILURE;
    }


    const int port { std::stoi(argv[2]) };
    struct hostent* p_he = gethostbyname(argv[1]);
    if (!p_he){
        std::cerr << "Failed to resolve address for " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    //resolving address
    char ip4[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,p_he->h_addr_list[0],ip4,INET_ADDRSTRLEN);
    std::cout << "Resolved ip-address:\t" << ip4 << std::endl;

    socket_wrapper::SocketWrapper sock_wrap;
    socket_wrapper::Socket sock = {AF_INET, SOCK_DGRAM, IPPROTO_UDP};
    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }
    //wait time for response
    struct timeval p_tv {.tv_sec = 5, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,reinterpret_cast<const char*>(&p_tv), sizeof(p_tv));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = inet_addr(ip4);//*(long*)p_he->h_addr_list[0];

    while(true){
        char message[MSG_LENGTH];
        std::cout << "Please enter the message:";
        std::cin.getline(message,MSG_LENGTH);

        if (sendto(sock,message,MSG_LENGTH,0,reinterpret_cast<struct sockaddr*>(&address),sizeof(sockaddr_in)) <= 0)
        {
            std::cerr << sock_wrap.get_last_error_string() << std::endl;
            return EXIT_FAILURE;
        }

        char response[MSG_LENGTH];
        memset(response,'\0',MSG_LENGTH);
        socklen_t addr_len = sizeof(sockaddr_in);
        if (recvfrom(sock,response,MSG_LENGTH,0,reinterpret_cast<struct sockaddr*>(&address),&addr_len) <= 0)
        {
            std::cerr << sock_wrap.get_last_error_string() << std::endl;
            return EXIT_FAILURE;
        }

        std::cout << "Server replied: " << response << std::endl;
    }
    close(sock);
    return EXIT_SUCCESS;
}

