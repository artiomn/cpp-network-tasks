#include <iostream>
#include <cstdlib>

#include <string>
#include <cstring>

#include <unistd.h> 
#include <netdb.h>  
#include <sys/socket.h>
#include <arpa/inet.h>

const int BUFFER_SZ = 256;
char buffer[BUFFER_SZ];

int main(int argc, char const* argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <ip_addr> <port> " << std::endl;
        return EXIT_FAILURE;
    }

    const char* ip = argv[1];
    const int port = { std::stoi(argv[2]) };

    auto sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == -1)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFFER_SZ) << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    //    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_pton(addr.sin_family, ip, &(addr.sin_addr));

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        std::cerr << strerror_r(errno, &buffer[0], BUFFER_SZ) << std::endl;
        close(sock);

        return EXIT_FAILURE;
    }

    while (strcmp(buffer, "exit") != 0)
    {
        std::cout << "<< ";
        std::cin >> buffer;

        send(sock, buffer, sizeof(buffer), 0);
        recv(sock, buffer, sizeof(buffer), 0);

        std::cout << ">> " << buffer << '\n';
    }
    close(sock);

    return EXIT_SUCCESS;
}
