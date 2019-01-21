#include "tool.h"
#include <assert.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>

#include <iostream>
#include <mutex>

#include <future>

#include "Time.h"

using namespace Time;
using namespace NET;
using namespace Thread;

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
    // deferred 表示函数调用延迟到　wait 或　get 函数调用时才执行
    std::future<int> future = std::async(std::launch::deferred ,find_the_answer);
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

void test1(){
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

}

int test_fun(int a,int b){
    printf("test \n");
    return 12;
}

int main(int argc, char const *argv[]){
    std::string now = getCurrentSystemTime();
    printf("now is [%s]\n",now.c_str());
    Timer timer;

    for(int i=0;i<100000;++i){

    }

    printf("used time is [%f]\n",timer.point());
    for(int i=0;i<100000;i++){
    }

    printf("used time is [%f]\n",timer.point());

    int a=12,b=13;
    // auto result = spawn_task(test_fun,a,b);

    // printf("wait for result\n");
    // printf("result is [%d]\n",result.get());

    return 0;
}