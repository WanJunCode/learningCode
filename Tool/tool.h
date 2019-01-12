#ifndef WANJUN_TOOL_H
#define WANJUN_TOOL_H

#include <stdio.h>
#include <netinet/in.h>     // sockaddr_in 

#include <thread>
#include <numeric>          // std::distance
#include <vector>
#include <algorithm>
#include <exception>
#include <memory>
#include <mutex>
#include <stack>
#include <climits>

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
    int socketBind(int sockfd,const char* ip,int port);

    typedef struct remote_client{
        int clientfd;
        struct sockaddr_in client_address;
        remote_client():clientfd(0){}
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
        t(t_){}
        ~thread_guard(){
            // 析构函数 判断是否可以等待线程结束
            if(t.joinable()){
                t.join();
            }
        }
        thread_guard(thread_guard const&)=delete;
        thread_guard& operator=(thread_guard const&)=delete;
    };

    class Scoped_thread{
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
    private:
        std::thread t;
    };

    // ========================================================================
    // ========================================================================
    template<typename Iterator,typename T>
    struct accumulate_block{
        void operator()(Iterator first,Iterator last,T& result){
            result = std::accumulate(first,last,result);
        }
    };

    template<typename Iterator,typename T>
    T parallel_accumulate(Iterator first,Iterator last,T init){
        // 获得两个迭代器的长度
        unsigned long const length = std::distance(first,last);

        if(!length)
            return init;

        unsigned long const min_per_thread = 25;
        // (20 + 25)  /  25
        unsigned long const max_threads = (length+min_per_thread-1)/min_per_thread;

        unsigned long const hardware_threads = std::thread::hardware_concurrency();

        // 线程数量
        unsigned long const num_threads = 
            std::min( hardware_threads != 0 ? hardware_threads:2, max_threads );
        // 每个线程 分配的 块大小
        unsigned long const block_size = length / num_threads;

        std::vector<T> results(num_threads);
        std::vector<std::thread> threads( num_threads-1 );

        Iterator block_start = first;
        for(unsigned long i = 0;i<(num_threads-1);++i){
            // 从 迭代器 头部开始 每次取 block_size 大小的迭代器放入 线程中进行计算

            Iterator block_end = block_start;
            // A generalization of pointer arithmetic.
            // 给指定的迭代器 增加 n 步长
            std::advance(block_end, block_size);
            
            threads[i] = std::thread(
                accumulate_block<Iterator,T>(),
                block_start, block_end, std::ref(results[i]));

            block_start = block_end;
        }
        // 计算最后的数值
        accumulate_block<Iterator,T>()(block_start,last,results[num_threads-1]);

        std::for_each(threads.begin(), threads.end(), std::mem_fun(&std::thread::join));
        return std::accumulate(results.begin(),results.end(), init);
    }

    struct empty_stack: std::exception{
        // throw() 表示不抛出异常
        const char *what() const throw(){
            return "empty stack!";
        }
    };

    // 线程安全栈
    template<typename T>
    class threadsafe_stack{
    private:
        std::stack<T> data;
        mutable std::mutex m_mutex;
    public:
        threadsafe_stack():data(std::stack<T>())
        {}
        // 复制构造函数
        threadsafe_stack(const threadsafe_stack& other){
            std::lock_guard<std::mutex> lock(other.m_mutex);
            data = other.data;
        }
        // 删除 赋值构造函数
        threadsafe_stack& operator=(const threadsafe_stack&) = delete;
        void push(T new_value){
            std::lock_guard<std::mutex> lock(m_mutex);
            data.push(new_value);
        }
        std::shared_ptr<T> pop(){
            std::lock_guard<std::mutex> lock(m_mutex);
            if(data.empty())
                throw empty_stack();        // 抛出异常
            std::shared_ptr<T> const result( std::make_shared<T>(data.top()) );
            data.pop();
            return result;
        }
        void pop(T& value){
            std::lock_guard<std::mutex> lock(m_mutex);
            if(data.empty())
                throw empty_stack();        // 抛出异常
            value = data.top();
            data.pop();
        }
        bool empty() const{
            std::lock_guard<std::mutex> lock(m_mutex);
            return data.empty();            
        }
    };

    // 层次锁
    class hierarchical_mutex{
    public:
        explicit hierarchical_mutex(unsigned long value);
        void lock();
        void unlock();
        bool try_lock();
    private:
        std::mutex internal_mutex;
        unsigned long const hierarchy_value;
        unsigned long previous_hierarchy_value;
        // thread_local 线程周期函数
        static thread_local unsigned long this_thread_hierarchy_value;
        void check_for_hierarchy_violation();
        void updata_hierarchy_value();
    };

};


#endif // !WANJUN_TOOL_H