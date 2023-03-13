#include <iostream>
#include <memory>
#include <utility>
#include <cstdlib>
#include <cwctype>
#include <stdexcept>
#include <boost/asio.hpp>
#include <optional>
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>
#include <thread>
#include <boost/algorithm/string/trim.hpp>

 using namespace boost::algorithm;
 using boost::asio::ip::tcp;
 const auto buffer_size = 4096;
 namespace fs = std::filesystem;


 #if defined(_WIN32)
 const wchar_t separ = fs::path::preferred_separator;
 #else
 //const wchar_t separ = *reinterpret_cast<const wchar_t*>(&fs::path::preferred_separator);
 const char separ = *reinterpret_cast<const wchar_t*>(&fs::path::preferred_separator);
 #endif

 class session
   : public std::enable_shared_from_this<session>
 {
 public:
   session(tcp::socket socket)
     : socket_(std::move(socket))
   {
   }

   void start()
   {
     do_read();
   }

 private:
   std::optional<fs::path> recv_file_path()
   {
       std::string trim_data = std::string(data_);
       std::string request_data = trim_right_copy(trim_data);

       if (!request_data.size()) return std::nullopt;

       auto cur_path = fs::current_path().u8string();
       auto file_path = fs::weakly_canonical(request_data).u8string();


       #if defined(_WIN32)
       std::transform(cur_path.begin(), cur_path.end(), cur_path.begin(),
           [](wchar_t c) { return std::towlower(c); }
       );
       std::transform(file_path.begin(), file_path.end(), file_path.begin(),
           [](wchar_t c) { return std::towlower(c); }
       );
       #endif
       if (file_path.find(cur_path) == 0)
       {
           file_path = file_path.substr(cur_path.length());
       }

       return fs::weakly_canonical(cur_path + separ + file_path);
   }



   bool process()
   {
   auto file_to_send = recv_file_path();
   bool result = false;

   if (std::nullopt != file_to_send)
   {
       std::cout << "Trying to send " << *file_to_send << "..." << std::endl;
       if (send_file(*file_to_send))
       {
           std::cout << "File was sent." << std::endl;
           do_read();
       }
       else
       {
           std::cerr << "File sending error!" << std::endl;
           do_read();
       }
       result = true;
   }

   return result;
   }


   bool send_buffer(const std::vector<char>& buffer)
   {
       const auto size = buffer.size();

       auto self(shared_from_this());
       boost::asio::async_write(socket_, boost::asio::buffer(buffer, size),
           [this, self](boost::system::error_code ec, std::size_t length)
           {
               if (!ec)
               {
                   std::cout << "Its work!" << std::endl;
                   std::cout << ec.message() << std::endl;
                   return true;
               }
               else
               {
                   std::cout << ec.message() << std::endl;
                   return false;
               }
           });
       return true;
   }


   bool send_file(fs::path const& file_path)
   {
       if (!(fs::exists(file_path) && fs::is_regular_file(file_path))) return false;
       std::vector<char> buffer(buffer_size);
       std::ifstream file_stream(file_path, std::ifstream::binary);

       if (!file_stream) return false;

       std::cout << "Sending file " << file_path << "..." << std::endl;
       while (file_stream)
       {


           file_stream.read(&buffer[0], buffer.size());

           if(file_stream.gcount() <= buffer.size())
           {
               buffer.resize(file_stream.gcount());
           }

           if (!send_buffer(buffer)) return false;
       }
       return true;
   }


   void do_read()
   {
     auto self(shared_from_this());
     std::fill_n(data_, max_length, 0);
     socket_.async_read_some(boost::asio::buffer(self->data_, max_length),
         [this, self](boost::system::error_code ec, std::size_t length)
         {
           if (!ec)
           {
             std::cout << data_ << " " << length << std::endl;
             //do_write(length);
             self->process();
           }
         });
   }


   tcp::socket socket_;
   enum { max_length = 260 };
   char data_[max_length];
 };




 class server
 {
 public:
   server(boost::asio::io_context& io_context, short port)
     : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
   {
     do_accept();
   }

 private:
   void do_accept()
   {
     acceptor_.async_accept(
         [this](boost::system::error_code ec, tcp::socket socket)
         {
           if (!ec)
           {
            std::cout << "Client tid = " << std::this_thread::get_id() << std::endl;
             std::make_shared<session>(std::move(socket))->start();
           }

           do_accept();
         });
   }

   tcp::acceptor acceptor_;
 };

 int main(int argc, char* argv[])
 {
   try
   {
     if (argc != 2)
     {
       std::cerr << "Usage: async_tcp_echo_server <port>\n";
       return 1;
     }

     boost::asio::io_context io_context;

     server s(io_context, std::atoi(argv[1]));

     io_context.run();
   }
   catch (std::exception& e)
   {
     std::cerr << "Exception: " << e.what() << "\n";
   }

   return 0;
 }
