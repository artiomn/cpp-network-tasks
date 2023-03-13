#include "open_SSL.h"


open_SSL::open_SSL(socket_wrapper::Socket&& client_sock) : client_sock_(std::move(client_sock)) {}

bool open_SSL::ssl_init()
{
    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();

    meth = TLS_server_method();
    ctx = SSL_CTX_new(meth);

    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        return false;
    }

    fs::path path_cert = fs::current_path() / "../certificates" / "server.pem";
    fs::path path_key  = fs::current_path() / "../certificates" / "key.pem";

    std::string serv_pem = path_cert.string();
    std::string key_pem = path_key.string();

    //Загрузка сертификата.
    if (SSL_CTX_use_certificate_file(ctx, serv_pem.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        return false;
    }

    // Загрузка ключа.
    if (SSL_CTX_use_PrivateKey_file(ctx, key_pem.c_str(), SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        return false;
    }

    // verify private key
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        ERR_print_errors_fp(stderr);
        return false;
    }

    return true;
}

std::string open_SSL::get_request()
{
    std::array<char, MAX_PATH + 1> buffer;
    size_t recv_bytes = 0;
    const auto size = buffer.size() - 1;

    std::cout << "Reading user request..." << std::endl;
    while (true)
    {
        auto result = SSL_read(ssl, &buffer[recv_bytes], size - recv_bytes);

        if (!result) break;

        if (-1 == result)
        {
            if (need_to_repeat()) continue;
            throw std::logic_error("Socket reading error");
        }

        auto fragment_begin = buffer.begin() + recv_bytes;
        auto ret_iter = std::find_if(fragment_begin, fragment_begin + result,
            [](char sym) { return '\n' == sym || '\r' == sym;  });
        if (ret_iter != buffer.end())
        {
            *ret_iter = '\0';
            recv_bytes += std::distance(fragment_begin, ret_iter);
            break;
        }
        recv_bytes += result;
        if (size == recv_bytes) break;
    }

    buffer[recv_bytes] = '\0';

    auto result = std::string(buffer.begin(), buffer.begin() + recv_bytes);
    std::cout << "Request = \"" << result << "\"" << std::endl;

    return result;
}

bool open_SSL::send_buffer(const std::vector<char>& buffer)
{
    size_t transmit_bytes_count = 0;
    const auto size = buffer.size();

    while (transmit_bytes_count != size)
    {
        auto result = SSL_write(ssl, &(buffer.data()[0]) + transmit_bytes_count, size - transmit_bytes_count);
        if (-1 == result)
        {
            if (need_to_repeat()) continue;
            return false;
        }

        transmit_bytes_count += result;
    }

    return true;
}

bool open_SSL::send_file(fs::path const& file_path)
{
    if (!(fs::exists(file_path) && fs::is_regular_file(file_path))) return false;

    std::vector<char> buffer(buf_size);
    std::ifstream file_stream(file_path, std::ifstream::binary);

    if (!file_stream) return false;

    std::cout << "Sending file " << file_path << "..." << std::endl;
    while (file_stream)
    {
        file_stream.read(&buffer[0], buffer.size());
        if (!send_buffer(buffer)) return false;
    }

    return true;
}

std::optional<fs::path> open_SSL::recv_file_path()
{
    auto request_data = get_request();
    if (!request_data.size()) return std::nullopt;

    auto cur_path = fs::current_path().wstring();
    auto file_path = fs::weakly_canonical(request_data).wstring();

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

    return fs::weakly_canonical(cur_path + sep + file_path);
}

bool open_SSL::process()
{
    auto file_to_send = recv_file_path();
    bool result = false;

    if (std::nullopt != file_to_send)
    {
        std::cout << "Trying to send " << *file_to_send << "..." << std::endl;
        if (send_file(*file_to_send))
        {
            std::cout << "File was sent." << std::endl;
        }
        else
        {
            std::cerr << "File sending error!" << std::endl;
        }
        result = true;
    }

    return result;
}





bool open_SSL::recv_packet(SSL *ssl)
{
    char buf[buf_size];
    int len = 0;

    do
    {
        len = SSL_read(ssl, buf, buf_size - 1);
        buf[len] = 0;
        std::cout << buf << std::endl;
    }
    while (len > 0);

    if (len < 0)
    {
        switch (SSL_get_error(ssl, len))
        {
            // Not an error.
            case SSL_ERROR_WANT_READ:
            case SSL_ERROR_WANT_WRITE:
                return true;
            break;
            case SSL_ERROR_ZERO_RETURN:
            case SSL_ERROR_SYSCALL:
            case SSL_ERROR_SSL:
                return false;
        }
    }

    return true;
}

bool open_SSL::send_packet(const std::string &buf, SSL *ssl)
{
    int len = SSL_write(ssl, buf.c_str(), buf.size());
    if (len < 0)
    {
        int err = SSL_get_error(ssl, len);
        switch (err)
        {
            case SSL_ERROR_WANT_WRITE:
            case SSL_ERROR_WANT_READ:
                return true;
            break;
            case SSL_ERROR_ZERO_RETURN:
            case SSL_ERROR_SYSCALL:
            case SSL_ERROR_SSL:
            default:
                return false;
        }
    }

    return false;
}

void open_SSL::log_ssl()
{
    for (int err = ERR_get_error(); err; err = ERR_get_error())
    {
        char *str = ERR_error_string(err, 0);
        if (!str) return;
        std::cerr << str << std::endl;
    }
}

void open_SSL::client_processing()
{
    ssl = SSL_new(ctx);

    if (!ssl)
    {
        std::cerr << "Error creating SSL." << std::endl;
        log_ssl();
        exit(-1);
    }

    SSL_set_fd(ssl, client_sock_);

     
    int err = SSL_accept(ssl);
    if (err <= 0)
    {
        std::cerr << "Error creating SSL connection.  err = " << err << std::endl;
        log_ssl();
        exit(-1);
    }

    std::cout << "SSL connection using " << SSL_get_cipher(ssl) << std::endl;
    
    while (true)
    {
        std::cout << "Client tid = " << std::this_thread::get_id() << std::endl;
        
        process();
    }
    
}

open_SSL::~open_SSL() {SSL_shutdown(ssl);}