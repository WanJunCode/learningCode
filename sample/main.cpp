#include <thread>
#include <iostream>
#include <mutex>

using namespace std;

class TA{
public:
    TA(int i){
        cout<<"struct"<<endl;
    }
    TA(const TA &t){
        cout<<"copy struct"<<endl;
    }
    TA(const TA &&t){
        cout<<"right reference struct"<<endl;
    }
    ~TA(){
        cout<<"~destruct"<<endl;
    }

    void operator()(){
        cout<<"operator ()"<<endl;
    }

};



int main(int argc, char const *argv[])
{
    int x = 12;
    TA ta(x);
    thread myobj(ta);

    myobj.join();
    cout<<"main stop"<<endl;
    return 0;
}