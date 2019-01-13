#include "tool.h"
#include <assert.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <mutex>

#include <future>

using namespace NET;

// once call 用于延迟初始化
std::once_flag resource_flag;
void init_resoutce(){

}

void foo(){
    std::call_once(resource_flag,init_resoutce);
    //other thing...
}

int find_the_answer(){
    return 1;
}
void do_other_stuff(){
}
void std_future(){
    std::future<int> future = std::async(find_the_answer);
    do_other_stuff();
    std::cout<<"result = "<<future.get()<<std::endl;
}


static bool stop = false;

int x = 0;
static void handle_term(int sig){
    if(x++==5){
        stop = true;
        printf("退出\n");
    }else{
        printf("signal\n");
    }
}

int main(int argc, char const *argv[])
{
    if(isBig()){
        printf("big endian\n");
    }else{
        printf("small endian\n");
    }

    signal(SIGINT,handle_term);

    int sock = tcp_socket();

    int ret = socketBind(sock,"192.168.0.105",12345);
    listen(sock,5);

    while(!stop){
        sleep(1);
        printf(".\n");
    }

    close(sock);
    
    std::mutex m_mutex;
    // std::lock_guard<std::mutex> locker(m_mutex);
    std::unique_lock<std::mutex> locker(m_mutex);

    locker.unlock();

    return 0;
}