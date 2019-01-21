#ifndef WANJUN_TIME
#define WANJUN_TIME

#include <string>
#include <chrono>

namespace Time{
    using namespace std::chrono;
    
    std::string getCurrentSystemTime();

    // 每次调用  point()  返回距离上次调用或初始化的  时间
    class Timer{
    public:
        Timer():pre(steady_clock::now()){ }
        inline double point(){
            auto tmp = steady_clock::now();
            auto time_span = duration_cast<duration<double>>( tmp - pre);
            pre = tmp;
            return time_span.count();
        }
    private:
        steady_clock::time_point pre;
    };

};

#endif // !WANJUN_TIME
