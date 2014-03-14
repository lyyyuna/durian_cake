#include <iostream>

using namespace::std;

int
number_one_1(unsigned int n)
{
    int count = 0;
    while (n) {
        if (n&1)
            count ++;

        n = n >> 1;
        n = n & 0x7fffffff;
        // cout << n << " ";
    }

    return count;
}

int
number_one_2(unsigned int n)
{
    int count = 0;

    while (n) {
        count ++;
        n = (n-1) & n;
    }
    return count;
}

void
test(unsigned int number, int expected)
{
    cout << "test " << number << endl;
    cout << "solution 1: " << endl;
    int actual1 = number_one_1(number);
    if (actual1 == expected) 
        cout << "passed" << endl;
    else
        cout << "failed" << endl;

    cout << "solution 2: " << endl;
    int actual2 = number_one_2(number);
    if (actual2 == expected)
        cout << "passed" << endl;
    else
        cout << "failed" << endl;

    cout << endl;

}

int 
main()
{
    test(0, 0);
    test(1, 1);
    test(10, 2);
    test(2, 1);
    test(0x7fffffff, 31);
    test(0xffffffff, 32);
    test(0x80000000, 1);

    return 0;
}
