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
#include <condition_variable>
#include <stack>
#include <deque>
#include <list>
#include <queue>
#include <climits>
#include <future>
#include <type_traits>
#include <functional>

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

    template<typename T>
    class threadsafe_queue{
    private:
        mutable std::mutex m_mutex;
        std::queue<T> data_queue;
        std::condition_variable m_data_cond;
    public:
        threadsafe_queue(){}
        threadsafe_queue(threadsafe_queue const& other){
            std::lock_guard<std::mutex> lock(m_mutex);
            // ? 是否需要锁住 other 的 m_mutex
            data_queue = other.data_queue;
        }
        void push(T new_value){
            std::lock_guard<std::mutex> lock(m_mutex);
            data_queue.push(new_value);
            m_data_cond.notify_one();
        }
        void wait_and_pop(T& value){
            std::lock_guard<std::mutex> lock(m_mutex);
            m_data_cond.wait(lock,
                            [this]{ return !data_queue.empty(); } );
            value = data_queue.front();
            data_queue.pop();
        }
        std::shared_ptr<T> wait_and_pop(){
            std::unique_lock<std::mutex> lock(m_mutex);
            m_data_cond.wait(lock, 
                            [this]{ return !data_queue.empty();});
            std::shared_ptr<T> res(std::make_shared<T>(data_queue.front()));
            data_queue.pop();
            return res;
        }
        bool try_pop(T& value){
            std::lock_guard<std::mutex> lock(m_mutex);
            if(data_queue.empty()){
                return false;
            }
            value = data_queue.front();;
            data_queue.pop();
            return true;
        }
        std::shared_ptr<T> try_pop(){
            std::lock_guard<std::mutex> lock(m_mutex);
            if(data_queue.empty()){
                return std::shared_ptr<T>();
            }
            std::shared_ptr<T> result(std::make_shared<T>(data_queue.front()));
            data_queue.pop();
            return result;
        }
        bool empty() const{
            std::lock_guard<std::mutex> lock(m_mutex);
            return data_queue.empty();
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

    class move_only{
    public:
        move_only();
        // 移动复制构造函数
        move_only(move_only&&);
        move_only(move_only const&) = delete;
        // 移动赋值构造函数
        move_only& operator=(move_only&&);
        move_only& operator=(move_only const&) = delete;

        // 可调用对象
        void operator()();
    };

    template<typename F,typename... Args>
    inline auto real_async(F&& f,Args&&... args)
    ->std::future<typename std::result_of<F(Args...)>::type>{
        // std::launch::async  立刻开启线程执行
        return std::async(std::launch::async,std::forward<F>(f),std::forward<Args>(args)...);
    }

    // thread detach 执行，　使用　future 获得返回结果
    template<typename F,typename... Args>
    auto thread_async(F&& f,Args&&... args)
    ->std::future<typename std::result_of<F(Args...)>::type>{
        using return_type = typename std::result_of<F(Args...)>::type;
        auto func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        std::packaged_task<return_type()> task(std::move(func));
        auto future_ = task.get_future();
        std::thread thd(std::move(task));
        thd.detach();
        return future_;
    }

};

namespace Algorithm{
    // 模板函数
    template<typename T>
    std::list<T> sequential_quick_sort(std::list<T> input){
        if(input.empty()){
            return input;
        }
        std::list<T> result;
        result.splice(result.begin(),input,input.begin());
        T const& pivot=*result.begin();
        
        auto divide_point = std::partition(input.begin(),input.end(),[&](T const& t){return t<pivot;});

        std::list<T> lower_part;
        lower_part.splice(lower_part.end(),input,input.begin(),divide_point);

        // 递归
        auto new_lower(sequential_quick_sort(std::move(lower_part)));
        auto new_higher(sequential_quick_sort(std::move(input)));

        result.splice(result.end(),new_higher);
        result.splice(result.begin(),new_lower);
        return result;
    }
};


#endif // !WANJUN_TOOL_H