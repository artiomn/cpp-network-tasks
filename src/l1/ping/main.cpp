#include <string>
#include <iostream>
#include <unistd.h>
#include <iomanip>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <bitset>

#define ICMP_RECV_TIMEOUT 4
#define ICMP_PKT_SIZE 16
#define ICMP_TIME_INTERVAL_SEC 1
#define RESPONSE_PKT_SIZE 192

struct icmp_packet{
    struct icmphdr hdr;
    char data[ICMP_PKT_SIZE - sizeof(struct icmphdr)];
};

// checksum calc
uint16_t checksum(void *data, int len){
    uint16_t *buf = (uint16_t*)data;
    uint32_t sum=0;
    uint16_t result;

    for ( sum = 0; len > 1; len -= 2 )
        sum += *buf++;
    if ( len == 1 )
        sum += *(uint8_t*)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

void print_icmp_packet(icmp_packet &pkt){
    std::cout << "Binary output of header and message of ICMP-packet" << std::endl;
    std::cout << std::bitset<8>(pkt.hdr.type) <<std::bitset<8>(pkt.hdr.code) << std::bitset<16>(pkt.hdr.checksum) << std::endl;
    std::cout << std::bitset<16>(pkt.hdr.un.echo.id) << std::bitset<16>(pkt.hdr.un.echo.sequence)  << std::endl;
    std::cout << pkt.data << std::endl;
    std::cout << "==================================================" << std::endl;
}

int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <hostname>" << std::endl;
        return EXIT_FAILURE;
    }

    //resolve of IP-address
    struct hostent* p_he = gethostbyname(argv[1]);
    if (!p_he){
        std::cerr << "Failed to resolve address for " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    char ip4[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,p_he->h_addr_list[0],ip4,INET_ADDRSTRLEN);
    std::cout << "Resolved ip-address:\t" << ip4 << std::endl;

    struct sockaddr_in address;
    address.sin_family = p_he->h_addrtype;
    address.sin_port = htons(0);
    address.sin_addr.s_addr = *(long*)p_he->h_addr_list[0];

    //reverse dns-lookup
    char host_name[50];
    if (getnameinfo(reinterpret_cast<const sockaddr *>(&address), sizeof(address), host_name, sizeof(host_name), NULL, 0, NI_NAMEREQD))
        std::cout << "Can't resolve hostname!" << std::endl;
    else
        std::cout << "Resolved hostname:\t" << host_name << std::endl;



    //open raw socket
    socket_wrapper::SocketWrapper sock_wrap;
    socket_wrapper::Socket sock = {AF_INET,SOCK_RAW,IPPROTO_ICMP};

    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        close(sock);
        return EXIT_FAILURE;
    }

    //set recv_timeout for icmp packet
    struct timeval p_tv {.tv_sec = ICMP_RECV_TIMEOUT, .tv_usec = 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,reinterpret_cast<const char*>(&p_tv), sizeof(p_tv));

    bool run = true;
    struct icmp_packet my_pkt;
    struct icmphdr my_hdr;

    struct icmp_packet* p_pkt;
    uint8_t response[RESPONSE_PKT_SIZE];
    uint16_t cksum;

    int sent_count=0, flag_sent, i;
    long double time = 0.0;

    struct timespec time_start, time_end;
    struct sockaddr_in address_recv;
    socklen_t add_len = sizeof(address_recv);

    //main cycle
    while(run){

        flag_sent = true;

        //preparing icmp-packet
        my_hdr.type = ICMP_ECHO;
        my_hdr.code = 0;
        my_hdr.checksum = 0;
        my_hdr.un.echo.id = getpid();
        my_hdr.un.echo.sequence = sent_count++;
        my_pkt.hdr = my_hdr;

        for (i = 0; i < sizeof(my_pkt.data)-1; ++i){
            my_pkt.data[i]='a'+i;
        }
        my_pkt.data[i] = 0;

        //filling checksum
        my_pkt.hdr.checksum = checksum(&my_pkt,ICMP_PKT_SIZE);


        //send packet
        //print_icmp_packet(my_pkt);
        clock_gettime(CLOCK_MONOTONIC, &time_start);
        if (sendto(sock, &my_pkt, ICMP_PKT_SIZE, 0, reinterpret_cast<struct sockaddr*>(&address), sizeof(address)) <= 0)
        {
            std::cout << "Packet Sending Failed!" << std::endl;
            flag_sent=false;
        };

        //receive packet
        if (recvfrom(sock, (char*)response, 56, 0, reinterpret_cast<struct sockaddr*>(&address_recv), &add_len) <= 0
              && sent_count>0 && flag_sent)
        {
            std::cout << "Packet receive failed!" <<std::endl;
        }
        else
        {
            clock_gettime(CLOCK_MONOTONIC, &time_end);
            p_pkt = (struct icmp_packet*)(response + sizeof(struct ip));
            //print_icmp_packet(*p_pkt);

            double timeElapsed = ((double)(time_end.tv_nsec - time_start.tv_nsec))/1000000.0;
            time = (time_end.tv_sec- time_start.tv_sec) * 1000.0 + timeElapsed;

            // if packet was not sent, don't receive
            if(flag_sent)
            {
                    if (p_pkt->hdr.type!=0 && p_pkt->hdr.code!=0)
                    {
                        std::cout << "Response with type = " << p_pkt->hdr.type << "and code = " << p_pkt->hdr.code << std::endl;
                    }
                    else
                    {
                        //checking checksum
                        cksum = p_pkt->hdr.checksum;
                        p_pkt->hdr.checksum = 0;
                        if (checksum(p_pkt,ICMP_PKT_SIZE)!=cksum){
                            std::cout << "Checksum of echo-response failed!" << std::endl;
                            std::cout << checksum(p_pkt,ICMP_PKT_SIZE) << " vs " << cksum << std::endl;
                        }
                    }
                    std::cout << ICMP_PKT_SIZE <<" bytes from " << host_name << "(" << ip4 << ") icmp_seq=" << sent_count << " time=" << std::setprecision(3) <<time<< "ms" << std::endl;
            }
        }
        sleep(ICMP_TIME_INTERVAL_SEC);
    }
}
