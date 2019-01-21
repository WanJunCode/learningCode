#ifndef WANJUN_QUEUE
#define WANJUN_QUEUE

#include <queue>
#include <list>
#include <functional>   // std::less
#include <algorithm>    // std::sort, std::includes
#include <mutex>
#include <condition_variable>

// base         std::queue
// derived      Queue_s

template <class T, class Sequence = std::list<T> >
class Queue_s : public std::queue < T, Sequence > {
public:
    explicit Queue_s();
    ~Queue_s();
public:
    void push(const T& val);
    T pop_front(bool &bstat, double seconds = 0);
    size_t size() const;
    void swap(Queue_s& t);
    void wake_all();
protected:
    void pop();
    std::mutex mutex_;
    std::condition_variable condition_;
};

template <class T, class Sequence>
Queue_s<T, Sequence>::Queue_s(){

}

template <class T, class Sequence>
Queue_s<T, Sequence>::~Queue_s() {
}

template <class T, class Sequence>
void Queue_s<T, Sequence>::push(const T& val) {
    std::lock_guard<std::mutex> locker(mutex_);
    std::queue<T, Sequence>::push(val);
    condition_.notify_one();
}

template <class T, class Sequence>
void Queue_s<T, Sequence>::pop() {
    std::lock_guard<std::mutex> locker(mutex_);
    std::queue<T, Sequence>::pop();
}

template <class T, class Sequence>
T Queue_s<T, Sequence>::pop_front(bool &bstat, double seconds) {
    std::unique_lock<std::mutex> locker(mutex_);
    if (std::queue<T, Sequence>::empty())
        condition_.wait(locker);

    if (!std::queue<T, Sequence>::empty()) {
        T tmp(std::queue<T, Sequence>::front());
        std::queue<T, Sequence>::pop();
        bstat = true;
        return tmp;
    }
    bstat = false;
    return T();
}

template <class T, class Sequence>
size_t Queue_s<T, Sequence>::size() const {
    std::lock_guard<std::mutex> locker(mutex_);
    return std::queue<T, Sequence>::size();
}

template <class T, class Sequence>
void Queue_s<T, Sequence>::wake_all() {
    condition_.notify_all();
}

template <class T, class Sequence>
void Queue_s<T, Sequence>::swap(Queue_s& t) {
    std::lock_guard<std::mutex> locker(mutex_);    
    std::queue<T, Sequence>::swap(t);
}

#endif // !WANJUN_QUEUE