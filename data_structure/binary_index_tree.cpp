#include <stdio.h>
#include <string.h>

#define MAX 17

int C[MAX];
int A[MAX];
int tmp;

int LowBit(int a)
{
    return (a&(-a));
}

void Modify(int index,int num)//一次修改树状数组中相关数字的信息
{
    while(index<=MAX)
    {
        C[index]+=num;
        index+=LowBit(index);
    }
}

int GetResult(int index)//求得前index项和
{
    int sum=0;
    while(index>0)
    {
        sum+=C[index];
        index-=LowBit(index);
    }
    return sum;
}

int main()
{
    while(1)
    {
        memset(C,0,sizeof(C));
        memset(A,0,sizeof(A));

        printf("请输入16个整数：");
        for(int i=1;i<MAX;i++)
        {
            scanf("%d",&A[i]);
            Modify(i,A[i]);
        }

        printf("请输入对每个数值增加的量：");
        for(int i=1;i<MAX;i++)
        {
            scanf("%d",&tmp);
            Modify(i,tmp);
        }

        for(int i=1;i<MAX;i++)
        {
            printf("修改后前%d项和为%d\n",i,GetResult(i));
        }
    }
    return 0;
}