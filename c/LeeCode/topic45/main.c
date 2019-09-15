#include<stdio.h>

int jump(int* nums, int numsSize)
{
    int nMaxStep = 0;
    int nNextStep = 0;
    int nCurStep = 0;
    int nTmpStep = 0;
    int nStepCnt = 0;
    int nFlag = 0;
    int nSuitNext = 0;

    if(numsSize < 2)
    {
        return 0;
    }

    while(nMaxStep <= numsSize)
    {
        nNextStep = nums[nCurStep];
        while(nNextStep)
        {
            if(nNextStep < numsSize)
            {
                nTmpStep = nums[nCurStep + nNextStep];
                if(nNextStep + nTmpStep > nMaxStep)
                {
                    nSuitNext = nNextStep;
                    nMaxStep = nNextStep + nTmpStep;
                }
            }
            else
            {
                nFlag = 1;
                break;
            }
            nNextStep--;
        }

        nCurStep = nSuitNext + nCurStep; 
        nStepCnt = nStepCnt + 1;
        if(nFlag)
        {
            break;
        }
     }

    return nStepCnt;

}

int main(void)
{
//    int nums[5] = {2,3,1,1,4};
    int nums[3] = {1,2,3};
//    int nums[2] = {2,1};
    int nJump = 0;
    int i = 0;
    int nLen = sizeof(nums)/sizeof(int);

    nJump = jump(nums,nLen);

    printf("game input is:\n");
    for(i = 0; i < nLen; i++)
    {
        printf("%d\t",nums[i]);
    }

    printf("\njump least step is %d\n",nJump);

    return 0;
}
