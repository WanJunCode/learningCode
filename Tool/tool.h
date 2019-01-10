#ifndef WANJUN_TOOL_H
#define WANJUN_TOOL_H

#include <stdio.h>
#include <netinet/in.h>     // sockaddr_in 

namespace NET{

    bool isBig();
    // host network change
    unsigned long int host2Netlong(unsigned long int hostlong);
    unsigned short int host2NetShort(unsigned short int hostshort);
    unsigned long int net2Hostlong(unsigned long int netlong);
    unsigned short int net2HostShort(unsigned short int netshort);

    // 调用成功返回一个 socket ，失败返回 -1，并设置 errno
    int tcp_socket();
    int udp_socket();

    // 明明 socket
    int socketBind(int sockfd,const char * ip,int port);

    typedef struct remote_client{
        int clientfd;
        struct sockaddr_in client_address;
        remote_client():clientfd(0){

        }
    } remote_client;

    int socketAccept(int listener);
    remote_client Accept(int listener);
    remote_client Connect(const char* ip, int port);

};


#endif // !WANJUN_TOOL_H