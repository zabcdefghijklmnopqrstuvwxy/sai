#include<stdio.h>

char * addBinary(char * a, char * b){
    int nLen1 = 0;
    int nLen2 = 0;
    int nLenRet = 0;
    int i = 0;
    int nOv = 0;

    nLen1 = strlen(a);
    nLen2 = strlen(b);

    if(nLen1 == nLen2)
    {
        nLenRet = nLen1;
    }
    else 
    {
        if(nLen1 > nLen2)
        {
            nLenRet = nLen1 + 1;
        }
        else
        {
            nLenRet = nLen2 + 1;
        }
    }

    char *sRet = (char*)malloc(nLenRet);
    memset(sRet,0,nLenRet);

    if(NULL == sRet)
    {
        return NULL;
    }

    while(i < nLen1 || i < nLen2)
    {
        if(i < nLen1 && i < nLen2)
        {
            if(a[nLen1 - i - 1] - 48 + b[nLen2 - i - 1] - 48 + nOv == 2)
            {
                sRet[nLenRet - i -1] = '0';
                nOv = 1;
            }
            else if(a[nLen1 - i - 1] - 48 + b[nLen2 - i - 1] - 48 + nOv == 1)
            {
                sRet[nLenRet - i -1] = '1';
                nOv = 0;
            }
            else if(a[nLen1 - i - 1] - 48 + b[nLen2 - i - 1] - 48 + nOv == 0)
            {
                sRet[nLenRet - i -1] = '0';
                nOv = 0;
            }
            else
            {

            }
        }

        if(i >= nLen1 && i < nLen2)
        {
            if(b[nLen2 - i - 1] - 48 + nOv == 2)
            {
                sRet[nLenRet - i -1] = '0';
                nOv = 1;
            }
            else if(b[nLen2 - i - 1] - 48 + nOv == 1)
            {
                sRet[nLenRet - i -1] = '1';
                nOv = 0;
            }
            else if(b[nLen2 - i - 1] - 48 + nOv == 0)
            {
                sRet[nLenRet - i -1] = '0';
                nOv = 0;
            }
            else
            {

            }
        }
        if(i >= nLen2 && i < nLen1)
        {
            if(a[nLen1 - i - 1] - 48 + nOv == 2)
            {
                sRet[nLenRet - i -1] = '0';
                nOv = 1;
            }
            else if(a[nLen1 - i - 1] - 48 + nOv == 1)
            {
                sRet[nLenRet - i -1] = '1';
                nOv = 0;
            }
            else if(a[nLen1 - i - 1] - 48 + nOv == 0)
            {
                sRet[nLenRet - i -1] = '0';
                nOv = 0;
            }
            else
            {

            }
        }

        ++i;
    }
    
    if(nOv)
    {
        sRet[nLenRet - i - 1] = '1';
        nOv = 0;
    }

//    sRet[nLenRet - 1] = '\0';

    return sRet;
}

int main(void)
{
    char *a = "11";
    char *b = "1";
    char *ret = NULL;   
    
    ret = addBinary(a, b);

    printf("%s + %s = %s\n",a,b,ret);

    return 0;
}

