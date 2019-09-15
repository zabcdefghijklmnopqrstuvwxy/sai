#include<stdio.h>

double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size)
{
    int k = 0;
    int nNum1Pos = 0,nNum2Pos = 0;
    int nCnt = 0;
    int nLen = nums1Size + nums2Size;
    int sort[nLen] = {0};

    if(NULL == nums1 && NULL == nums2)
    {
        return -1;
    }

    while(nCnt >= nLen)
    {
        if(nNum1Pos < nums1Size)
        {
            
        }
    
    }
}

int main(void)
{

    return 0;
}

