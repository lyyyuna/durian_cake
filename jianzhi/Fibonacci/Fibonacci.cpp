#include <iostream>

using namespace::std;

long long
Fibonacci_s1(unsigned int n)
{
    if (n <= 0)
        return 0;
    if (n == 1)
        return 1;

    return Fibonacci_s1(n-1) + Fibonacci_s1(n-2);
}

long long
Fibonacci_s2(unsigned int n)
{
    if (n <= 0)
        return 0;
    if (1 == n)
        return 1;

    long long N_1 = 1;
    long long N_2 = 0;
    long long res = 0;
    for (int i = 2; i <= n; ++i) {
        res = N_1 + N_2;
        N_2 = N_1;
        N_1 = res;
    }

    return res;
}

void
test(int n, int expected)
{
    cout << "test " << n << endl;
    cout << "solution1" << endl;
    if (Fibonacci_s1(n) == expected)
        cout << "passed" << endl;
    else 
        cout << "failed" << endl;

    cout << "solution2" << endl;
    if (Fibonacci_s2(n) == expected)
        cout << "passed" << endl;
    else
        cout << "failed" << endl;
}

int 
main()
{
    test(0, 0);
    test(3, 2);

    test(40, 102334155);
    return 0;
}
