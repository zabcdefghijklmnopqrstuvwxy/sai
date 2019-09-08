#include<stdio.h>

struct ListNode {
      int val;
      struct ListNode *next;
};

struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2)
{
    int nSum = 0,nL1 = 0,nL2 = 0;
    int nNodeCnt = 0;
    struct ListNode *result = NULL;    
    struct _STACK
    {
        int val;
        struct _STACK *pre;
    };
    
    struct ListNode *tmpl1 = l1;
    struct ListNode *tmpl2 = l2;
    struct _STACK *tmps1 = NULL;
    struct _STACK *tmps2 = NULL;


    if(NULL == l1 || NULL == l2)
    {
        return NULL;
    }

    while(NULL != tmpl1)
    {
        static nL1Cnt = 0;
        struct _STACK *sl1 = (struct _STACK*)malloc(sizeof(struct _STACK));
        memset(sl1,0,sizeof(struct _STACK));
        sl1->val = tmpl1->val;
        if(nL1Cnt)
        {
            sl1->pre = tmps1;
        }
        tmpl1 = tmpl1->next;
        tmps1 = sl1;
        nL1Cnt++;
    }

    while(NULL != tmpl2)
    {
        static nL2Cnt = 0;
        struct _STACK *sl2 = (struct _STACK*)malloc(sizeof(struct _STACK));
        memset(sl2,0,sizeof(struct _STACK));
        sl2->val = tmpl2->val;
        if(nL2Cnt)
        {
            sl2->pre = tmps2;
        }
        tmpl2 = tmpl2->next;
        tmps2 = sl2;
        nL2Cnt++;
    }

    while(NULL != tmps1)
    {
        struct _STACK *tmp = NULL;
        nL1 = nL1*10 + tmps1->val;
        tmp = tmps1;
        tmps1 = tmps1->pre;
        free(tmp);
    }
    
    while(NULL != tmps2)
    {
        struct _STACK *tmp = NULL;
        nL2 = nL2*10 + tmps2->val;
        tmp = tmps2;
        tmps2 = tmps2->pre;
        free(tmp);
    }

    nSum = nL1 + nL2;

    while(nSum)
    {
        static struct ListNode *tmp = NULL;
        int val = 0;
        val = nSum % 10;
        struct ListNode *newnode = (struct ListNode*)malloc(sizeof(struct ListNode));
        memset(newnode,0,sizeof(struct ListNode));

        newnode->val = val;
        newnode->next = NULL;

        if(nNodeCnt)
        {
            tmp->next = newnode;
        }
        else
        {
            result = newnode;
        }
        
        tmp = newnode;
        nSum = nSum / 10;
        nNodeCnt++;
    }

    return result;
}


int main(void)
{
    int i = 0;
    struct ListNode l1[6] = {0};
    struct ListNode l2[6] = {0};
    struct ListNode* ret = NULL;

    for(i = 0; i < 5; i++)
    {
        l1[i].val = i+1;
        l1[i].next = &l1[i+1];
        l2[i].val = i+1;
        l2[i].next = &l2[i+1];
    }

    ret = addTwoNumbers(l1,l2);

    printf("sum list node is:");
    while(ret)
    {
        printf("%d\t",ret->val);
        ret = ret->next;
    }

    return 0;
}

