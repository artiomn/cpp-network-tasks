#include <boost/asio.hpp>
#include <cerrno>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <thread>
#include <utility>
using boost::asio::ip::tcp;
class session : public std::enable_shared_from_this<session> {
public:
  session(tcp::socket &&socket) : socket_(std::move(socket)) {}
  void start() 
  {
    do_read();
  }

private:
  void do_read() /*читаем и разбираем запрос. Если клиент присылает EXIT - сервер останавливает свою работу */
  {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(data_,
                            max_length), /*читаем запрос масимум 1024 символа*/
        [this, self](boost::system::error_code ec,
                     std::size_t length) /*захватываем this для использования
                                            членов класса*/
        {
          const std::string out = "EXIT";
          if (!ec) {
            std::string input_str;
            data_[length] = '\0';
            input_str = data_;
            if (*(input_str.end() - 1) != ' ')
              input_str += ' '; /* разделитель - знак пробела*/
            std::cout << input_str << "symbol"
                      << std::endl; /*отладочная информация*/
            std::string::size_type name_length = input_str.find(' ');
            if (name_length == std::string::npos) {
              std::cout << "File name is not defined"
                        << std::endl; /*нет имени файла - нечего читать*/
            }
            filename_ = input_str.substr(0, name_length);
            std::string::size_type bytes_length =
                input_str.find(' ', name_length + 1);
            if (bytes_length == std::string::npos) {
              std::cout << "Bytes shift is not defined" << std::endl;
              bytes_shift_ = 0;
            } else {
              std::string bytes_shift =
                  input_str.substr(name_length + 1, bytes_length - name_length);
              bytes_shift_ = std::stoi(bytes_shift);
              std::string::size_type bytes_for_transmission_length =
                  input_str.find(' ', bytes_length + 1);
              if (bytes_for_transmission_length == std::string::npos) {
                std::cout << "Size for transmission is not defined"
                          << std::endl;
                bytes_for_transmission_ = 0;
              } else {
                bytes_for_transmission_ = std::stoi(input_str.substr(
                    bytes_length + 1,
                    bytes_for_transmission_length - bytes_length));
              }
            }
            std::cout << filename_ << ' ' << bytes_shift_ << ' '
                      << bytes_for_transmission_ << std::endl;
            if (!send_file())
              std::cout << "File is not exist" << std::endl;
          }
          if (filename_ != out)
            do_read(); //продолжаем читать
          else {
            socket_.close(ec);
            std::cout << "Session is closed" << std::endl;
            throw("Exit filename");// выбрасываем исключение, по которому остановим работу сервера
          }
        });
  }

  bool send_file() /*передача заданного фрагмента файла со  смещением и файла
                      целиком, в зависимости от запроса*/
  {
    auto self(shared_from_this());
    std::vector<char> buffer(max_length);
    auto path = std::filesystem::current_path().string();
    path += filename_;
    std::ifstream file_stream(path, std::ifstream::binary);
    if (!file_stream)
      return false;
    std::cout << "Sending file " << filename_ << "..." << std::endl;
    std::string buffer_string;
    if (bytes_for_transmission_ != 0) {
      for (int i = bytes_shift_ / buffer.size(); (i > 0) && (file_stream);
           --i) /*вычитываю сдвиг*/
      {
        file_stream.read(&buffer[0], buffer.size());
      }
      int count = bytes_shift_ % buffer.size();
      file_stream.read(&buffer[0], count);
      for (count = bytes_for_transmission_ / buffer.size(); count > 0;
           --count) {
        file_stream.read(&buffer[0], buffer.size());
        auto number_of_bytes = file_stream.gcount();
        boost::asio::async_write(
            socket_, boost::asio::buffer(buffer, number_of_bytes),
            [this, self, number_of_bytes](boost::system::error_code ec,
                                          std::size_t /*length*/) {
              if (!ec) {
                std::cout << number_of_bytes << std::endl;
              }
            });
      }

      count = bytes_for_transmission_ % buffer.size();
      file_stream.read(&buffer[0], count);
      auto number_of_bytes = file_stream.gcount();
      boost::asio::async_write(
          socket_, boost::asio::buffer(buffer, number_of_bytes),
          [this, self, number_of_bytes](boost::system::error_code ec,
                                        std::size_t /*length*/) {
            if (!ec) {
              std::cout << number_of_bytes << std::endl;
            }
          });
    } else {
      while (file_stream) {
        file_stream.read(&buffer[0], buffer.size());
        auto number_of_bytes = file_stream.gcount();
        boost::asio::async_write(
            socket_, boost::asio::buffer(buffer, number_of_bytes),
            [this, self, number_of_bytes](boost::system::error_code ec,
                                          std::size_t /*length*/) {
              if (!ec) {
                std::cout << number_of_bytes << std::endl;
              }
            });
      }
    }
    file_stream.close();
    return true;
  }
  tcp::socket socket_;
  enum { max_length = 1024 };
  char data_[max_length];
  std::string filename_;
  int bytes_shift_ = 0;
  int bytes_for_transmission_ = 0;
};

class server {
public:
  server(boost::asio::io_context &io_context, short port)
      : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
    do_accept();
  }

private:
  void do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
          if (!ec) {
            auto s = std::make_shared<session>(std::move(socket));
            s -> start();
          }
        });
  }
  tcp::acceptor acceptor_;
};

int main(int argc, char *argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: async_tcp_echo_server <port>\n";
      return 1;
    }

    boost::asio::io_context io_context;

    server s(io_context, std::atoi(argv[1]));

    io_context.run();
  } cat
ch (char const *s) {
    std::cout << s << std::endl;
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
