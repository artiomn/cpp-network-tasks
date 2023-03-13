#include "tcp_server_6.h"
#include "connector.h"
#include "open_SSL.h"


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
    std::cout << "Starting TCP-server on the port " << port << "...\n";
    socket_wrapper::Socket sock = connector.connect_to_client(port);

    //==========================================
    // without openSSl
    //==========================================

    TCPserver tcpserver(std::move(sock));
    tcpserver.server_run();

    //==========================================
    // with openSSL
    //==========================================
    // open_SSL oSSL(std::move(sock));
    // oSSL.ssl_init();
    // oSSL.client_processing();
    

    return EXIT_SUCCESS;
}