#include "tool.h"
#include <netinet/in.h>
#include <arpa/inet.h>
// socket
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

namespace NET{

    unsigned long int host2Netlong(unsigned long int hostlong){
        return htonl(hostlong);
    }
    unsigned short int host2NetShort(unsigned short int hostshort){
        return htons(hostshort);
    }
    unsigned long int net2Hostlong(unsigned long int netlong){
        return ntohl(netlong);
    }
    unsigned short int net2HostShort(unsigned short int netshort){
        return ntohs(netshort);
    }

    bool isBig(){
        // 匿名联合体
        union{
            short value;
            char union_bytes[sizeof(short)];
        } test;
        test.value = 0x0102;
        if((test.union_bytes[0]==1)&&(test.union_bytes[1]==2)){
            return true;
        }else{
            return false;
        }
    // short
    //  01  [1]
    //  02  [0]
    // 数据大端  数据的低位保存在内存的高地址中
    }

    int tcp_socket(){
        return socket(PF_INET,SOCK_STREAM,0);
    }
    int udp_socket(){
        return socket(PF_INET,SOCK_DGRAM,0);
    }

    int socketBind(int sockfd, const char * ip, int port){
        struct sockaddr_in address;
        bzero(&address,sizeof(address));
        address.sin_family = AF_INET;
        // presentation to network
        inet_pton(AF_INET, ip, &address.sin_addr);
        address.sin_port = htons(port);
        return bind(sockfd, (struct sockaddr*)&address, sizeof(address));
    }

    // 快速接收一个 client，
    int socketAccept(int listener){
        struct sockaddr_in client;
        socklen_t client_addrlength = sizeof(client);
        int clientfd = accept(listener, (struct sockaddr*)&client, &client_addrlength);
        if(clientfd < 0){
            printf("errno is: %d\n",errno);
        }else{
            char remote[INET_ADDRSTRLEN];
            printf("%s %d\n",inet_ntop(AF_INET,&client.sin_addr, remote, INET_ADDRSTRLEN), 
                            ntohs(client.sin_port));
        }
        return clientfd;
    }

    // 返回一个结构体 包含 clientfd client_address
    remote_client_t Accept(int listener){
        remote_client_t client;
        socklen_t client_addrlength = sizeof(client.client_address);
        client.clientfd = accept(listener, (struct sockaddr*)&client.client_address , &client_addrlength);
        return client;
    }

    // 封装了 client connection 操作
    remote_client_t Connect(const char* ip, int port){
        remote_client_t client;
        bzero(&client.client_address,sizeof(client.client_address));
        client.client_address.sin_family = AF_INET;
        inet_pton(AF_INET,ip,&client.client_address.sin_addr);
        client.client_address.sin_port = htons(port);

        client.clientfd = tcp_socket();
        assert(client.clientfd);
        if(0==connect(client.clientfd,(struct sockaddr*)&client.client_address,sizeof(client.client_address))){
            return client;
        }else{
            //  连接失败，返回 clientfd == 0
            remote_client_t tmp;
            return tmp;
        }
    }

    int Close(remote_client_t client){
        close(client.clientfd);
    }
    
    int Shutdown(remote_client_t client, int howto){
        shutdown(client.clientfd,howto);
    }

    int NumInSocket(int sock,int &length){
        length = 0;
        return ioctl(sock,FIONREAD,&length);
    }

}

namespace Thread{

    thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value = ULONG_MAX;
    hierarchical_mutex::hierarchical_mutex(unsigned long value):
            hierarchy_value(value),
            previous_hierarchy_value(0)
    {}
    void hierarchical_mutex::check_for_hierarchy_violation(){
        if( this_thread_hierarchy_value <= hierarchy_value )
            throw std::logic_error("mutex hierarchy violated");
    }
    void hierarchical_mutex::updata_hierarchy_value(){
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }
    void hierarchical_mutex::lock(){
        check_for_hierarchy_violation();
        internal_mutex.lock();
        updata_hierarchy_value();
    }
    void hierarchical_mutex::unlock(){
        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }
    bool hierarchical_mutex::try_lock(){
        check_for_hierarchy_violation();
        if(!internal_mutex.try_lock()){
            return false;
        }
        updata_hierarchy_value();
        return true;
    }
}