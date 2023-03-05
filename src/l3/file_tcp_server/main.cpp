#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
extern "C"
{
#include <sys/fcntl.h>
#include <sys/sendfile.h>
}
#include <socket_wrapper/socket_class.h>
#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>

// Trim from end (in place).
static inline std::string& rtrim(std::string& s)
{
    s.erase(std::find_if(
                s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); })
                .base());
    return s;
}

int main(int argc, char const* argv[])
{

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    socket_wrapper::SocketWrapper sock_wrap;
    const int                     port{ std::stoi(argv[1]) };

    socket_wrapper::Socket sock = { AF_INET, SOCK_STREAM, IPPROTO_TCP };

    std::cout << "Starting echo server on the port " << port << "...\n";

    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(port),
    };
    addr.sin_addr.s_addr = INADDR_ANY;
    std::cout << "Test\n";
    if (bind(sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        // Socket will be closed in the Socket destructor.
        return EXIT_FAILURE;
    }
    listen(sock, 10);

    char buffer[256] = { 0 };

    // socket address used to store client address
    struct sockaddr_storage client_address     = { 0 };
    socklen_t               client_address_len = sizeof(sockaddr_in);
    ssize_t                 recv_len           = 0;

    std::cout << "Running echo server...\n" << std::endl;
    char client_address_buf[INET_ADDRSTRLEN];
    int  connect_socket = accept(sock,
                                reinterpret_cast<sockaddr*>(&client_address),
                                &client_address_len);

    // Read content into buffer from an incoming client.
    /*recv_len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                        reinterpret_cast<sockaddr *>(&client_address),
                        &client_address_len);*/
    recv_len         = recv(connect_socket, buffer, sizeof(buffer) - 1, 0);
    std::string name = buffer;
    int         file = open(name.c_str(), O_RDONLY);
    if (file <= 0)
    {
        std::cout << "File is not exist\n";
        std::cout << file;
        return EXIT_FAILURE;
    }

    std::cout << file << name.c_str();
    if (recv_len > 0)
    {
        buffer[recv_len] = '\0';
        std::cout << buffer << std::endl;

        send(connect_socket, buffer, recv_len, 0);
        off_t offset = 0;
        int   i      = 0;
        while (sendfile(connect_socket, file, &offset, 3) == 3)
        {
            std::cout << ++i << std::endl;
        }
        int file_lenght = 3;
        std::cout << file_lenght << " " << errno << std::endl;
    }
    ~sock.close();
    close(connect_socket);
    // std::cout << std::endl;

    return EXIT_SUCCESS;
}
