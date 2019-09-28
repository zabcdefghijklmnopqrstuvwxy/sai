#include<stdio.h>
    
int longestValidParentheses(char * s)
{    
    int nLen = 0;
    int nSum = 0;
    int nValid = 0;
    int nValidLen = 0;
    int nMax = 0;
    int i = 0;
    
    nLen = strlen(s);

    for(i = 0; i < nLen; i++)
    {
        nSum += s[i] == '(' ?  1 : -1;
        
        nValid++;
        if(nSum < 0)
        {
            nMax = nMax > nValidLen ? nMax : nValidLen;
            nSum = 0;
            nValid = 0;
        }
        else if(nSum == 0)
        {
            nValidLen = nValid; 
        }
    }

    nMax = nMax > nValidLen ? nMax : nValidLen;

    nSum = 0;
    nValid = 0;
    nValidLen = 0;

    for(i = nLen - 1; i > 0; i--)
    {
        nSum += s[i] == ')' ?  1 : -1;
        
        nValid++;
        if(nSum < 0)
        {
            nMax = nMax > nValidLen ? nMax : nValidLen;
            nSum = 0;
            nValid = 0;
        }
        else if(nSum == 0)
        {
            nValidLen = nValid; 
        }
    }

    return nMax = nMax > nValidLen ? nMax : nValidLen;
}

int main(void)
{
  char *test = "(()";
 // char *test = "()(())";
 // char *test = ")()";
 // char *test = "(()()(";
//    char *test = "(()())";
 // char *test = ")(((((()())()))";
    int nLen = 0;

    nLen = longestValidParentheses(test);

    printf("longest string is %s,length is %d\n",test,nLen);

    return 0;
}



