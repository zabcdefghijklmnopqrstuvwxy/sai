#include<stdio.h>

int reverse(int x){
    int nIntMax = 2147483647;
    int nIntMin = -2147483648;
    int nRecord[32] = {0};
    int nIndex = 0;
    int i = 0;
    int nResult = 0;
    int nInput = x;

    while(nInput)
    {
        nRecord[nIndex] = nInput % 10;
        ++nIndex;
        nInput = nInput / 10;
    }
    
    if(nIndex <= 1)
    {
        return x;
    }


    for(i = 0; i < nIndex - 1; i++)
    {
        nResult = nResult*10 + nRecord[i];
    }
    
    if((nResult > nIntMax/10) || (nResult < nIntMin / 10))
    {
        return 0;
    } 
    else if((nResult == nIntMax/10) || (nResult == nIntMin / 10))
    {
         if((nRecord[nIndex - 1] > nIntMax % 10) || (nRecord[nIndex - 1] < nIntMin %10))
         {
             return 0;
         }
         else
         {
            nResult = nResult*10 + nRecord[nIndex - 1];   
         }       
    }
    else
    {
        nResult = nResult*10 + nRecord[nIndex - 1];  
    }
    
    return nResult;
}

int main(void)
{
    int nInput = 0;
    int nResult = 0;

    nResult = reverse(nInput);

    printf("%d reverse result is %d\n",nInput,nResult);

    return 0;
}
