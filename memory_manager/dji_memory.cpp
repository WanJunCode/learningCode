/** @file dji_memory.cpp
 *  @version 3.3
 *  @date Jun 15 2017
 *
 *  @brief
 *  // 内存管理 工具
 *  Implement memory management for OSDK library
 *
 *  @Copyright (c) 2016-2017 DJI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 *  @attention
 *  It is not necessary to include dji_memory.hpp in any custom code file.
 *  The functions in this file are not API functions.
 *  Do not modify this file if you are unsure about it.
 *
 */

#include "dji_memory.hpp"
#include <string.h>

using namespace DJI::OSDK;

MMU::MMU()
{
}

void
MMU::setupMMU()
{
  uint32_t i;
  // 初始化  memoryTable 头部
  memoryTable[0].tabIndex  = 0;
  memoryTable[0].usageFlag = 1;
  memoryTable[0].pmem      = memory;
  memoryTable[0].memSize   = 0;
  // 初始化 memoryTable 后续数组
  for (i = 1; i < (MMU_TABLE_NUM - 1); i++)
  {
    memoryTable[i].tabIndex  = i;
    memoryTable[i].usageFlag = 0;
  }
  // 初始化 memoryTable 末尾
  memoryTable[MMU_TABLE_NUM - 1].tabIndex  = MMU_TABLE_NUM - 1;
  memoryTable[MMU_TABLE_NUM - 1].usageFlag = 1;
  memoryTable[MMU_TABLE_NUM - 1].pmem      = memory + MEMORY_SIZE;
  memoryTable[MMU_TABLE_NUM - 1].memSize   = 0;
}

void
MMU::freeMemory(MMU_Tab* mmu_tab)
{
  if (mmu_tab == (MMU_Tab*)0)
  {
    return;
  }
  if (mmu_tab->tabIndex == 0 || mmu_tab->tabIndex == (MMU_TABLE_NUM - 1))
  {
    // head  and  tail  memoryTable 不释放
    return;
  }
  // 设置使用标记位 0，表示未被使用
  mmu_tab->usageFlag = 0;
}

MMU_Tab*
MMU::allocMemory(uint16_t size)
{
  uint32_t mem_used = 0;
  uint8_t  i;
  uint8_t  j                = 0;
  uint8_t  mmu_tab_used_num = 0;
  // 记录 已经使用的 memory node index 数组
  uint8_t  mmu_tab_used_index[MMU_TABLE_NUM];

  uint32_t temp32;
  // 记录最合适的 table 的 index and size
  uint32_t temp_area[2] = { 0xFFFFFFFF, 0xFFFFFFFF };

  uint32_t record_temp32 = 0;
  uint8_t  magic_flag    = 0;

  if (size > PRO_PURE_DATA_MAX_SIZE || size > MEMORY_SIZE)
  {
    // size too big
    return (MMU_Tab *) 0;
  }

  // 遍历 memorytable
  for (i = 0; i < MMU_TABLE_NUM; i++)
  {
    // 如果 memory node 已经使用了
    if (memoryTable[i].usageFlag == 1)
    {
      // 添加mem 使用大小
      mem_used += memoryTable[i].memSize;
      mmu_tab_used_index[mmu_tab_used_num++] = memoryTable[i].tabIndex;
    }
  }

  if (MEMORY_SIZE < (mem_used + size))
  {
    // too big
    return (MMU_Tab *) 0;
  }

  if (mem_used == 0)
  {
    // 未使用过 mem
    memoryTable[1].pmem      = memoryTable[0].pmem;
    memoryTable[1].memSize   = size;
    memoryTable[1].usageFlag = 1;
    // 返回 node 1
    return &memoryTable[1];
  }

  // 调整 mmu_tab_used_num 的顺序
  for (i = 0; i < (mmu_tab_used_num - 1); i++)
  {
    for (j = 0; j < (mmu_tab_used_num - i - 1); j++)
    {
      // 如果 node x 的指针位置 大于 node x+1 的位置
      if (memoryTable[mmu_tab_used_index[j]].pmem >
          memoryTable[mmu_tab_used_index[j + 1]].pmem)
      {
        // 异或就是两个数的二进制形式，按位对比，相同取0，不同取一
        // 交换
        mmu_tab_used_index[j + 1] ^= mmu_tab_used_index[j];
        mmu_tab_used_index[j] ^= mmu_tab_used_index[j + 1];
        mmu_tab_used_index[j + 1] ^= mmu_tab_used_index[j];
      }
    }
  }

  // 遍历已经使用过的 memory table
  for (i = 0; i < (mmu_tab_used_num - 1); i++)
  {
    // node x+1 内存指针位置 减去  node x
    // 判断 node x 到 node x+1 的内存大小
    // temp32 表示两个 table 指针 相距的大小
    temp32 = static_cast<uint32_t>(memoryTable[mmu_tab_used_index[i + 1]].pmem -
                                   memoryTable[mmu_tab_used_index[i]].pmem);

    uint32_t rest_of_table_i = temp32 - memoryTable[mmu_tab_used_index[i]].memSize;
    
    // node i 剩余的内存空间可以开辟 size 大小
    if ((rest_of_table_i) >= size)
    {
      // node i 剩余可用的 mem大小 小于 32 bit的最大值 
      // 找到 合适的table ， 剩余 mem 最小
      if (temp_area[1] > (rest_of_table_i))
      {
        // 记录 node i 的 tabIndex
        temp_area[0] = memoryTable[mmu_tab_used_index[i]].tabIndex;
        // 记录 node i 剩余可用的mem大小
        temp_area[1] = rest_of_table_i;
      }
    }
    
    // 记录 node i 剩余可用的 mem 大小  累加
    record_temp32 += rest_of_table_i;
    
    // 如果 node i 剩余的空间大小可以开辟 size 空间，并且 magic_flag 为 0
    if (record_temp32 >= size && magic_flag == 0)
    {
      // j 记录下 可以开辟空间的 node index
      // 或者 node index 之前所有的 table 可用 memory 累加 大于 size
      j          = i;
      magic_flag = 1;
    }
  }

  if (temp_area[0] == 0xFFFFFFFF && temp_area[1] == 0xFFFFFFFF)
  {
    // 没有找到合适的 table， 则 紧凑前面的 table，空出足够的 memory
    for (i = 0; i < j; i++)
    {
      // 如果 node x+1 的指针位置 大于 node x指针位置 加 使用的内存大小
      if (memoryTable[mmu_tab_used_index[i + 1]].pmem >
          (memoryTable[mmu_tab_used_index[i]].pmem +
           memoryTable[mmu_tab_used_index[i]].memSize))
      {
        // 移动后面的 node 到前方
        // ||===================================||
        // ||^-----===^---------================||
        //        !   !         !
        memmove(memoryTable[mmu_tab_used_index[i]].pmem +
                  memoryTable[mmu_tab_used_index[i]].memSize,
                memoryTable[mmu_tab_used_index[i + 1]].pmem,
                memoryTable[mmu_tab_used_index[i + 1]].memSize);
        // 更改 node x+1 的指针位置
        memoryTable[mmu_tab_used_index[i + 1]].pmem =
          memoryTable[mmu_tab_used_index[i]].pmem +
          memoryTable[mmu_tab_used_index[i]].memSize;
      }
    }

    // 要么找到合适的 table 区域，或者 整理压缩出一块区域
    for (i = 1; i < (MMU_TABLE_NUM - 1); i++)
    {
      if (memoryTable[i].usageFlag == 0)
      {
      // 找到一个 没有使用的 memory table , 使其指针位置指向 可用空间的头部
        memoryTable[i].pmem = memoryTable[mmu_tab_used_index[j]].pmem +
                              memoryTable[mmu_tab_used_index[j]].memSize;

        memoryTable[i].memSize   = size;
        memoryTable[i].usageFlag = 1;
        // 返回  开辟的空间
        return &memoryTable[i];
      }
    }
    // 返回 空指针
    return (MMU_Tab*)0;
  }
  // end   没有找到合适的 table

  
  // 找到了合适的 j
  for (i = 1; i < (MMU_TABLE_NUM - 1); i++)
  {
    // 遍历得到一个 可用的 table
    if (memoryTable[i].usageFlag == 0)
    {
      memoryTable[i].pmem =
        memoryTable[temp_area[0]].pmem + memoryTable[temp_area[0]].memSize;

      memoryTable[i].memSize   = size;
      memoryTable[i].usageFlag = 1;
      return &memoryTable[i];
    }
  }

  return (MMU_Tab*)0;
}
