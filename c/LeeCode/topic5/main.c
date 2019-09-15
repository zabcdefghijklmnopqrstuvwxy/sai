#include<stdio.h>

char result[128] = {0};

char * longestPalindrome(char * s)
{
    int Hash[128] = {0};
    char fix[26] = {0};
    char *pLongPos = NULL;
    int nCnt = 0;
    int nStrLen = 0;
    int nIndex = 0;
    int nPos = 0;
    int i = 0,j = 0;
    int nFlag = 0;
    int nLen = strlen(s);

    if(nLen <= 1)
    {
        return s;
    }
    
    while(nPos < nLen && i < nLen)
    {
        if(0 == Hash[s[i]])
        {
            Hash[s[i]] = ++nIndex;
            fix[j++] = nIndex;
            i++;
        }
        else
        {
            int nTemPos = Hash[s[i]];
            
            if(nTemPos == fix[--j])
            {
                if(j == 0)
                {
                    if(nIndex > nCnt)
                    {
                        nCnt = nIndex;
                        if(nFlag)
                        {
                            nStrLen = 2 * nCnt - 1;
                        }
                        else
                        {
                            if(1 == nCnt)
                            {
                                nStrLen = 1;
                            }
                            else
                            {
                                nStrLen = 2 * nCnt;
                            }
                        }

                        pLongPos = s + nPos;

                        nFlag = 0;
                        nIndex = 0;
                        memset(Hash,0,sizeof(Hash));
                        memset(fix,0,sizeof(fix));
                        ++nPos;
                        i = nPos;
                        j = 0;
                    }
                }
            }
            else
            {
                if(0 == nFlag)
                {
                    nFlag = 1;
                    if(nTemPos == fix[--j])
                    {
                        if(j == 0)
                        {
                            if(nIndex > nCnt)
                            {
                                nCnt = nIndex;
                                if(nFlag)
                                {
                                    nStrLen = 2 * nCnt - 1;
                                }
                                else
                                {
                                    nStrLen = 2 * nCnt;
                                }

                                pLongPos = s + nPos;

                                nFlag = 0;
                                nIndex = 0;
                                memset(Hash,0,sizeof(Hash));
                                memset(fix,0,sizeof(fix));
                                ++nPos;
                                i = nPos;
                                j = 0;
                            }
                       }
                    }
                }
                else
                {
                    nFlag = 0;
                    nIndex = 0;
                    memset(Hash,0,sizeof(Hash));
                    memset(fix,0,sizeof(fix));
                    ++nPos;
                    i = nPos;
                    j = 0;
                }
            }
        
        }
    
    }
    
    memcpy(result,pLongPos,nStrLen);
    result[nStrLen] = '\0';

    return result;
}

int main(void)
{
    char *test = "c";
    char *pdata = NULL;

    pdata = longestPalindrome(test);

    printf("longest string is %s\n",pdata);

    return 0;
}
