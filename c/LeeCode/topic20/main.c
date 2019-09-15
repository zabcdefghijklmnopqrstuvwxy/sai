#include<stdio.h>

int isValid(char *s)
{
    int nLen = 0;
    int i = 0;
    int Hash[256] = {0};
    int nCnt = 0;

    struct _STACK{
        char val;
        struct _STACK *pre;
        struct _STACK *pop;
    }stack_t;

    nLen = strlen(s);
    struct _STACK *popnode = NULL;

    if(0 == nLen)
    {
        return 1;
    }

    if(nLen % 2)
    {
        return 0;
    }

    Hash['('] = 1;
    Hash[')'] = 2;
    Hash['['] = 3;
    Hash[']'] = 4;
    Hash['{'] = 5;
    Hash['}'] = 6;

    for(i = 0; i < nLen; i++)
    {
        if(Hash[s[i]] % 2)
        {
            struct _STACK *newnode = (struct _STACK*)malloc(sizeof(struct _STACK));
            newnode->val = s[i];

            if(NULL == popnode)
            {
                popnode = newnode;
                newnode->pre = NULL;
            }
            else
            {
                newnode->pre = popnode;
                popnode = newnode;
            }
            nCnt++;
        }
        else
        {
            if(NULL == popnode)
            {
                return 0;
            }

            if(Hash[s[i]] != (Hash[popnode->val] + 1))
            {
                return 0;
            }
            else
            {
                nCnt--;
                if(popnode->pre)
                {
                    struct _STACK  *tmpnode = popnode;
                    popnode = popnode->pre;
                    free(tmpnode);
                }
            }
        }
    }

    if(0 != nCnt)
    {
        return 0;
    }

    return 1;
}


int main(void)
{
    //char *test = "{[]([])}";   
    //char *test = "()";   
    char *test = "((";    
    int nResult = 0;

    nResult = isValid(test);

    if(nResult)
    {
        printf("test result is success\n");
    }
    else
    {
        printf("test result is failed\n");
    }

    return 0;
}
