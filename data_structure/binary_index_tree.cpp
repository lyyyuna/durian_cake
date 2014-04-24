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

void Modify(int index,int num)//һ���޸���״������������ֵ���Ϣ
{
    while(index<=MAX)
    {
        C[index]+=num;
        index+=LowBit(index);
    }
}

int GetResult(int index)//���ǰindex���
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

        printf("������16��������");
        for(int i=1;i<MAX;i++)
        {
            scanf("%d",&A[i]);
            Modify(i,A[i]);
        }

        printf("�������ÿ����ֵ���ӵ�����");
        for(int i=1;i<MAX;i++)
        {
            scanf("%d",&tmp);
            Modify(i,tmp);
        }

        for(int i=1;i<MAX;i++)
        {
            printf("�޸ĺ�ǰ%d���Ϊ%d\n",i,GetResult(i));
        }
    }
    return 0;
}