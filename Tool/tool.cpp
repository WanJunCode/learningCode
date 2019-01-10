#include "tool.h"

bool Tool::isBig()
{
    // 匿名联合体
    union{
        short value;
        char union_bytes[sizeof(short)];
    } test;
    test.value = 0x0102;
    if((test.union_bytes[0]==1)&&(test.union_bytes[1]==2)){
        return true;
    }else{
        return false;
    }
    // short
    //  01  [1]
    //  02  [0]
    // 数据大端  数据的低位保存在内存的高地址中
}