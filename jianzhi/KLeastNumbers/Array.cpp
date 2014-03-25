#include "Array.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int
RandomInRange(int min, int max)
{
    int random = rand() % (max-min+1) + min;
    return random;
}

void
swap(int* num1, int* num2)
{
    int temp = *num1;
    *num1 = *num2;
    *num2 = temp;
}

int 
partition(int* data, int length, int start, int end)
{
    // if (data == NULL || length < 1 || start < 0 || end >= length)
        
    int index = RandomInRange(start, end);
    swap(&data[index], &data[end]);

    int small = start-1;
    for (int i = start; i < end; ++i) {
        if (data[i] < data[end]) {
            ++small;
            if (small != i)
                swap(&data[i], &data[small]);
        }
    }

    ++small;
    swap(&data[small], &data[end]);

    return small;
}


