#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>

using namespace std;

bool FindNumbersWithSum(int * data, int len, int sum, int * num1, int * num2)
{
    int _start = 0;
    int _end = len-1;
    while (_end > _start) {
        int cursum = data[_start] + data[_end];
        if (cursum == sum) {
            *num1 = data[_start];
            *num2 = data[_end];
            return true;
        }
        else if (cursum > sum)
            _end --;
        else
            _start ++;
    }

    return false;
}

void PrintContinuousSequence(int small, int big)
{
    for(int i = small; i <= big; ++ i)
        printf("%d ", i);

    printf("\n");
}


void FindContinuousSequence(int sum)
{
    if (sum < 3)
        return ;

    int _small = 1;
    int _big = 2;
    int mid = (1+sum)/2;
    int cursum = 3;

    while (_small < mid) {
        if (cursum == sum)
            PrintContinuousSequence(_small, _big);

        while (cursum > sum && _small < mid) {
            cursum -= _small;
            _small ++;

            if (cursum == sum)
                PrintContinuousSequence(_small, _big);
        }

        _big ++;
        cursum += _big;
    }
}

int main()
{
    int data[] = {1, 2, 4, 7, 11, 15};
    int num1;
    int num2;
    cout << FindNumbersWithSum(data, 6, 15, &num1, &num2) << endl;
    cout << num1 << " " << num2 << endl;

    FindContinuousSequence(15);

    return 0;
}
