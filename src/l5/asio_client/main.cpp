#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };


int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    tcp::socket s(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(s, resolver.resolve(argv[1], argv[2]));

    std::cout << "Enter message: ";
    char request[max_length];
    std::cin.getline(request, max_length);
    size_t request_length = std::strlen(request);
    boost::asio::write(s, boost::asio::buffer(request, request_length));

    std::cout << "request_length is: ";
    std::cout << request_length << std::endl;


    std::vector<char> read_buffer(1024);
    //boost::asio::streambuf read_buffer;
    boost::system::error_code ec;
    size_t reply_length = boost::asio::read(s, boost::asio::buffer(read_buffer), 
    boost::asio::transfer_at_least(1), ec);
    
    if (!ec || ec == boost::asio::error::eof)
    {
        
        std::cout << "Reply is: ";
        std::cout << reply_length << std::endl;

        if (reply_length > 0)
        {
          read_buffer.resize(reply_length);
          std::fstream file;
          file.open(request, std::ios_base::out | std::ios_base::binary);

          if (file.is_open())
          {
              std::cout << "Received file!" << std::endl;
              file.write(&read_buffer[0], read_buffer.size());
          }                          
        }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}