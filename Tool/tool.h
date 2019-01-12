#ifndef WANJUN_TOOL_H
#define WANJUN_TOOL_H

#include <stdio.h>
#include <netinet/in.h>     // sockaddr_in 

#include <thread>

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

namespace Thread{

    // RAII  Resource Acquisition Is Initialization
    // 资源获取即初始化
    class thread_guard
    {
        std::thread& t;
    public:
        explicit thread_guard(std::thread& t_):
        t(t_){

        }
        ~thread_guard(){
            if(t.joinable()){
                t.join();
            }
        }

        thread_guard(thread_guard const&)=delete;
        thread_guard& operator=(thread_guard const&)=delete;
    };

    class Scoped_thread
    {
        std::thread t;
    public:
        explicit Scoped_thread(std::thread t_):
            t(std::move(t_)){
                if(!t.joinable())
                    throw std::logic_error("No thread");
            }
        
        ~Scoped_thread(){
            t.join();
        }

        Scoped_thread(Scoped_thread const&)=delete;
        Scoped_thread& operator=(Scoped_thread const&)=delete;
    };

};


#endif // !WANJUN_TOOL_H