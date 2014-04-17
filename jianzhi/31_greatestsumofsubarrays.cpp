#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <iterator>

using namespace std;

int FindGreatestSumOfSubArray(int * pData, int len)
{
    int nCurSum = 0;
    int nGreatestSum = 0;
    for (int i = 0; i < len; ++i) {
        if (nCurSum <= 0)
            nCurSum = pData[i];
        else
            nCurSum += pData[i];

        if (nCurSum > nGreatestSum)
            nGreatestSum = nCurSum;
    }

    return nGreatestSum;
}

int main()
{
    int arr[] = {1, -2, 3, 10, -4, 7, 2, -5};

    cout << FindGreatestSumOfSubArray(arr, 8) << endl;

    return 0;
}
