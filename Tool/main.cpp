#include "tool.h"

int main(int argc, char const *argv[])
{
    if(Tool::isBig()){
        printf("big endian\n");
    }else{
        printf("small endian\n");
    }
    return 0;
}