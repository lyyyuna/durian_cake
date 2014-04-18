#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cmath>
#include <cstdio>

using namespace std;

bool IsBit1(int num, unsigned int index)
{
    num = num >> index;
    return (num & 1);
}

unsigned int FindFirstBitIs1(int num)
{
    int indexBit = 0;
    while (((num & 1) == 0) && (indexBit < 8 * sizeof(int)))
    {
        num = num >> 1;
        ++ indexBit;
    }

    return indexBit;
}

void FindNumsAppearOnce(int * data, int len, int * num1, int * num2)
{
    int resultOR = 0;
    for (int i = 0; i < len; ++i)
        resultOR ^= data[i];

    unsigned int indexOf1 = resultOR & (-resultOR);

    *num1 = *num2 = 0;
    for (int i = 0; i < len; ++i) {
        if (data[i] & indexOf1)
            *num1 ^= data[i];
        else
            *num2 ^= data[i];
    }
}

int main()
{
    int data[] = {2, 4, 3, 6, 3, 2, 5, 5};
    int result1, result2;
    FindNumsAppearOnce(data, 8, &result1, &result2);
    cout << result1 << endl
        << result2 << endl;


    return 0;
}
