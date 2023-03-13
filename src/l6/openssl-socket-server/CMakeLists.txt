cmake_minimum_required(VERSION 3.10)

project(openssl-socket-server C CXX)

find_package(OpenSSL REQUIRED)

set(SRC tcp_server_6.cpp connector.cpp main.cpp open_SSL.cpp file_proccesing.cpp)
set(HEADER tcp_server_6.h connector.h open_SSL.h file_proccesing.h)

add_executable(openssl-socket-server ${SRC} ${HEADER})


#add_executable("${PROJECT_NAME}" "main.cpp" "tcp_server.cpp" "tcp_server.h")
target_link_libraries("${PROJECT_NAME}" pthread OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries("${PROJECT_NAME}" socket-wrapper)
