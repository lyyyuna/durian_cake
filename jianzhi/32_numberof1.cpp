#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <iterator>

using namespace std;

int CountNumOfDigits(int n)
{
    int res = 0;
    int tmp = n;
    while (tmp) {
        tmp /= 10;
        res ++;
    }

    return res;
}

int PowerOf_10(int n)
{
    if (n == 0)
        return 1;
    else
        return (PowerOf_10(n-1))*10;
}

int CountNumOf1(int n)
{
    if (n <= 9 && n >= 1)
        return 1;

    int ws = CountNumOfDigits(n);
    int gw = n / PowerOf_10(ws-1);

    int sum = 0;
    int tmp = n % PowerOf_10(ws-1);

    switch (gw) {
    case 1:
        sum += tmp + 1;
        sum += (ws-1) * PowerOf_10(ws-2);
        if (tmp != 0)
            sum += CountNumOf1(tmp);
        break;
    default:
        sum += PowerOf_10(ws-1);
        sum += gw * (ws-1) * PowerOf_10(ws-2);
        if (tmp != 0)
            sum += CountNumOf1(tmp);
        break;
    }

    return sum;
}

int main()
{
    int arr[] = {1, -2, 3, 10, -4, 7, 2, -5};

    cout << CountNumOf1(55) << endl;

    return 0;
}
