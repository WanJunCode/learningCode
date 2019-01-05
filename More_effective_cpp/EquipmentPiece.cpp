
#include "EquipmentPiece.h"
#include <stdio.h>

EquipmentPiece::EquipmentPiece(int id)
{
    id_ = id;
    printf("construct\n");
}

EquipmentPiece::~EquipmentPiece(){
    printf("destruct\n");
}
