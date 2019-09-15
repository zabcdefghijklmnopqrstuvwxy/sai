#include<stdio.h>

int isPalindrome(int x)
{
	int nPalind[64] = {0};
    int nCnt = 0;
    int i = 0;
    
    if(x < 0)
    {
        return 0;
    }
    
    while(x)
    {
        nPalind[nCnt++] = x % 10;
        x = x / 10;    
    }
    
    for(i = 0; i < nCnt / 2; i++)
    {
        if(nPalind[i] != nPalind[nCnt - i - 1])
        {
            break;
        }        
    }
    
    if(i >= nCnt / 2)
    {
        return 1;
    }
    
    return 0;
}

int main(void)
{
    int test = 1234321;
    
    if(isPalindrome(test))
    {
        printf("The test integer is Palindrome %d\n",test);
        return 0;
    }

    printf("The test integer is not Palindrome %d\n",test);
    return 0;
}
