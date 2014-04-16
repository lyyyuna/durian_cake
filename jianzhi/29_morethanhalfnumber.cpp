#include <iostream>
#include <string>
#include <cstdio>
#include <algorithm>
#include <cstdlib>

using namespace std;

int RandomInRange(int min, int max)
{
    int random = rand() % (max-min+1) + min;
    return random;
}

void Swap(int * num1, int * num2)
{
    int temp = *num1;
    *num1 = *num2;
    *num2 = temp;
}

int Partition(int * numbers, int len, int _start, int _end)
{
    if (numbers == NULL || len <= 0 || _start < 0 || _end >= len)
        ;

    int index = RandomInRange(_start, _end);
    Swap(&numbers[_end], &numbers[index]);

    int small = _start-1;
    for (int i = _start; i < _end; ++i) {
        if (numbers[_end] < numbers[i]) {
            ++small;
            if (small != i)
                Swap(&numbers[i], &numbers[small]);
        }
    }

    ++small;
    Swap(&numbers[small], &numbers[_end]);

    return small;
}

int MoreThanHalfNum(int * numbers, int len)
{
    int mid = len >> 1;
    int start = 0;
    int end = len - 1;
    int index = Partition(numbers, len , start, end);
    while (index != mid) {
        if (index > mid) {
            end = index - 1;
            index = Partition(numbers, len, start, end);
        }
        else {
            start = index + 1;
            index = Partition(numbers, len, start, end);
        }
    }

    return numbers[mid];
}



int main()
{
    int arr[] = {1, 2, 3, 2, 2, 2, 5, 4, 2};
    cout << MoreThanHalfNum(arr, sizeof(arr)/sizeof(int));

    return 0;
}
