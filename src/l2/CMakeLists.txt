cmake_minimum_required(VERSION 3.10)

project(cpp-network-lesson-2 C CXX)

if (UNIX)
    add_subdirectory("linux_call_intercepter")
endif()

if (WIN32)
    add_subdirectory("pil")
endif()

add_subdirectory("incorrect_udp_client")
add_subdirectory("promisc_switcher")
add_subdirectory("raw_sniffer")
add_subdirectory("pcap_sniffer")
add_subdirectory("simple_http_proxy")
