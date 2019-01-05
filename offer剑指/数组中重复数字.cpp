// 在一个长度为 n 的数组里的所有数字都在 0 到 n-1 的范围内。数组中某些数字是重复的，
// 但不知道有几个数字是重复的。也不知道每个数字重复几次。请找出数组中任意一个重复的数字。
// 例如，如果输入长度为 7 的数组 {2, 3, 1, 0, 2, 5, 3}，那么对应的输出是第一个重复的数字 2。

#include <stdio.h>
#include <stdint.h>

bool duplicate(int* nums, int length, int* duplication) {
    if (nums == NULL || length <= 0) 
        return false;
    
    for (int i = 0; i < length; i++) {
        // 将第 i 个位置上的书不断交换 获得值为 i
        while (nums[i] != i && nums[i] != nums[nums[i]]) {
            swap(nums, i, nums[i]);
        }
        if (nums[i] != i && nums[i] == nums[nums[i]]) {
            duplication[0] = nums[i];
            return true;
        }
    }
    return false;
}

void swap(int* nums, int i, int j) {
    int t = nums[i]; nums[i] = nums[j]; nums[j] = t;
}

int main(int argc, char const *argv[])
{
    
    return 0;
}