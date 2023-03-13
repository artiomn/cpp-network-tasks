#include "tcp_client_6.h"

TCPclient::TCPclient(socket_wrapper::Socket &&server_sock)
    : server_sock_(std::move(server_sock)) {}

bool TCPclient::send_request(char *message)
{
    int len = send(server_sock_, message, strlen(message), 0);
    if (len < 0)
    {
        printf("Send error!");
        return false;
    }
    return true;
}

bool TCPclient::recv_packet(char *message)
{
    std::vector<char> buffer_(4096);
    int len = recv(server_sock_, &(buffer_.data()[0]), buffer_.size(), 0);

    if (len > 0)
    {
        buffer_.resize(len);
        std::fstream file;
        file.open(message, std::ios_base::out | std::ios_base::binary);

        if (file.is_open())
        {
            std::cout << "Received file!" << std::endl;
            for (auto &b : buffer_)
            {
                std::cout << b;
            }
            std::cout << std::endl;
            file.write(&buffer_[0], buffer_.size());
        }
        return true;
    }
    else
    {
        printf("Read error!");
        return false;
    }
}

void TCPclient::proccesing()
{
    while (true)
    {
        printf("Enter message: ");
        std::cin.getline(message_, 256);

        send_request(message_);

        recv_packet(message_);
    }
}

TCPclient::~TCPclient() {}