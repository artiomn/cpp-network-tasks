#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstring>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>

#define BUFLEN 512  // max length of answer


int main(int argc, char const* argv[])
{
    sockaddr_in server;

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }


    socket_wrapper::SocketWrapper sock_wrap;
    const int port{ std::stoi(argv[1]) };

    socket_wrapper::Socket sock = { AF_INET, SOCK_DGRAM, IPPROTO_UDP };

    std::cout << "Starting echo client on the port " << port << "...\n";

    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, "192.168.4.73", &server.sin_addr);
    while (true)
    {
        char message[BUFLEN];
        printf("Enter message: ");
        std::cin.getline(message, BUFLEN);

        // send the message
        sendto(sock, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in));


        // receive a reply and print it
        // clear the answer by filling null, it might have previously received data
        char answer[BUFLEN] = {};

        // try to receive some data, this is a blocking call
        int slen = sizeof(sockaddr_in);
        int answer_length;
        //answer_length = recvfrom(sock, answer, BUFLEN, 0, (sockaddr*)&server, &slen);
        if (answer_length > 0)
        {
            std::cout << answer << "\n";
        }



        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}

