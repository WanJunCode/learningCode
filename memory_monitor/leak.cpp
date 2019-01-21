#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;
class big_data{
public:
    int data[1000];
};

int main(int argc, char const *argv[])
{
    while(true){
        string input;
        cin>>input;
        printf("input [%s]\n",input.c_str());

        if(input.npos!=input.find("a")){
            printf("find a\n");
            for(int i=0;i<20;++i){
                big_data *p=new big_data;
            }
        }else{
            printf("no a\n");
            break;
        }
    }
    return 0;
}
