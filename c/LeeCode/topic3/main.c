#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int lengthOfLongestSubstring(char * s)
{
    unsigned int i = 0;
    unsigned int unLen = 0;
    unsigned int unMaxLen = 0;
    unsigned int unFastPos = 0;
    unsigned int unSlowPos = 0;
    unsigned char Hash[128] = {0};
    
    if(NULL == s)
    {
        return -1;
    }

    unLen = strlen(s);

    while(unSlowPos < unLen && unFastPos < unLen)
    {
        if(0 == Hash[s[unFastPos]])
        {
            Hash[s[unFastPos]] = 1;
            unFastPos++;

            if(unFastPos - unSlowPos > unMaxLen)
            {
                unMaxLen = unFastPos - unSlowPos;
            }
        }
        else
        {
            Hash[s[unSlowPos]] = 0;
            unSlowPos++;
        }
    }
    
    return unMaxLen;
}

int main(void)
{
    int nMaxLen = 0;
    char *longstr = "abcbed"; 

    nMaxLen = lengthOfLongestSubstring(longstr);

    printf("test string is %s\n",longstr);
    printf("get max len is %d\n",nMaxLen);

    return 0;
}
