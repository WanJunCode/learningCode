#ifndef WJ_THREAD_MANAGER
#define WJ_THREAD_MANAGER

#include <thread>

class Thread;
class Runnable{
public:
    virtual ~Runnable() {}

    virtual void run() = 0;

    virtual void Thread(Thread *thread);

private:
    // Thread *thread_;
};

class Thread{
public:
    using thread_id=std::thread::id;

public:
    static thread_id Current_id(){
        return std::this_thread::get_id();
    }

    static bool is_current(thread_id id){
        return std::this_thread::get_id()==id;
    }

protected:

private:
    Runnable *runnable;
};

#endif