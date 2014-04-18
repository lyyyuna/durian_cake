#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

int Min(int a, int b, int c)
{
    int _min = min(a, b);
    _min = min(_min, c);

    return _min;
}

int GetUglyNumber(int k)
{
    if (k < 0)
        return 0;

    int * pUglyNumbers = new int [k];
    pUglyNumbers[0] = 1;
    int UglyIndex = 1;

    int * index2 = pUglyNumbers;
    int * index3 = pUglyNumbers;
    int * index5 = pUglyNumbers;

    while (UglyIndex < k) {
        int _min = Min(*index2 * 2, *index3 * 3, *index5 * 5);
        pUglyNumbers[UglyIndex] = _min;

        while (*index2 * 2 <= pUglyNumbers[UglyIndex])
            ++ index2;
        while (*index3 * 3 <= pUglyNumbers[UglyIndex])
            ++ index3;
        while (*index5 * 5 <= pUglyNumbers[UglyIndex])
            ++ index5;

        UglyIndex ++;
    }

    int res = pUglyNumbers[UglyIndex-1];
    delete[] pUglyNumbers;

    return res;
}

int main()
{
    cout << GetUglyNumber(1500) << endl;

    return 0;
}
