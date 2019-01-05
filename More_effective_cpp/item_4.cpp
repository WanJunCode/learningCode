#include <stdio.h>
#include <iostream>
#include <string>

#include "EquipmentPiece.h"

using namespace std;

int main(int argc, char const *argv[])
{
    // placement new
    void* rawmemory = operator new[](10 * sizeof(EquipmentPiece));

    // void * ==> EquipmentPiece *
    EquipmentPiece* basepiece = static_cast<EquipmentPiece*>(rawmemory);

    for(int i=0;i<10;++i){
        new (&basepiece[i]) EquipmentPiece(i);
    }

    // 完成数组的分配

    std::cout<<"use EquipmentPiece"<<std::endl;

    // 清理内存
    for(int i=9;i>=0;--i){
        // 手动调用 析构函数
        basepiece[i].~EquipmentPiece();
    }
    operator delete [](rawmemory);

    return 0;
}