/*时间限制:10000ms
单点时限:1000ms
内存限制:256MB
描述
You are given a sequence of integers, A = a1, a2, ... an. A consecutive subsequence of A (say ai, ai+1 ... aj) is called a 

"repeated sequence" if it appears more than once in A (there exists some positive k that ai+k = ai, ai+k+1 = ai+1, ... aj+k = 

aj) and its appearances are not intersected (i + k > j).

Can you find the longest repeated sequence in A?

输入
Line 1: n (1 <= n <= 300), the length of A.
Line 2: the sequence, a1 a2 ... an (0 <= ai <= 100).

输出
The length of the longest repeated sequence.

样例输入
5
2 3 2 3 2
样例输出
2*/

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <map>
#include <functional>

using namespace std;

char str[300];
int maxlen;
int maxindex;

int comlen(char * p, char * q)
{
    int len = 0;
    char* temp = q;
    while(*p && *q && temp!=p && *p++ == *q++ )
    {
        ++len;
    }
    return len;
}

void LRS_base(char * arr, int size)
{
    for(int i = 0; i < size; ++i)
    {
        for(int j = i+1; j < size; ++j)
        {
            int len = comlen(&arr[i],&arr[j]);
            if(len > maxlen)
            {
                maxlen = len;
                maxindex = i;
            }
        }
    }
    //outputLRS(arr);
}

int main()
{
    int len;
    int index = 0;
    cin >> len;
    while (len--) {
        int ch;
        cin >> ch;
        str[index++] = ch;
    }
    str[index] = '\0';

    LRS_base(str, index);

    cout << maxlen << endl;
}

