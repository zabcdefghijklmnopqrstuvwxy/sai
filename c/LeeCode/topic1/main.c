#include<stdio.h>
#include<limits.h>

int* twoSum(int* nums, int numsSize, int target, int* returnSize)
{   
    int i = 0;
    int max = INT_MIN;
    int min = INT_MAX;
    int nLen = 0;
    int diff = 0;
    int *nIndex = (int*)malloc(sizeof(int)*2);
    
    if(NULL == nIndex)
    {
        return NULL;
    }

    for(i = 0; i < numsSize; i++)
    {
        if(nums[i] > max)
        {
            max = nums[i];
        }

        if(nums[i] < min)
        {
            min = nums[i];
        }
    }
    
    nLen = max - min + 1;
    int *hash = (int*)malloc(sizeof(int)*nLen);

    for(i = 0; i < nLen; i++)
    {
        hash[i] = -1;
    }

    for(i = 0; i < numsSize; i++)
    {
        hash[nums[i] - min] = i;
    }
    
    for(i = 0; i < numsSize; i++)
    {
        diff = target - nums[i] - min;

        if(diff > 0 && diff < nLen && hash[diff] != -1 && i != hash[diff])
        {
            nIndex[0] = i;
            nIndex[1] = hash[diff];

            *returnSize = 2;

            return nIndex;
        }

    
    }

    return NULL;
}

int main(void)
{
    int testnum[10] = {1,7,2,8,3,5,8,1,4,11};   	
    int nLen = 0;
    int *pIndex = NULL;
    int i = 0;

    pIndex = twoSum(testnum,10,10,&nLen);

    if(NULL == pIndex || nLen ==0)
    {
        printf("get twosum is failed\n");
        return -1;
    }


    for(i = 0; i < nLen; i = i + 2)
    {
        printf("success index1=%d value1=%d,index2=%d value2=%d\n",pIndex[i],testnum[pIndex[i]],pIndex[i+1],testnum[pIndex[i+1]]);
    }

    return 0;
}
