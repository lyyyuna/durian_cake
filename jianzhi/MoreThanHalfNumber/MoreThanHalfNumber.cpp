#include <iostream>
#include "Array.h"

using namespace std;

bool
check_morethanhalf(int* numbers, int len, int num)
{
    int times = 0;
    for (int i = 0; i < len; ++i) {
        if (numbers[i] == num)
            times ++;
    }
    bool is_morethanhalf = true;
    if (times*2 <= len) 
        is_morethanhalf = false;

    return is_morethanhalf;
}

int 
solution(int* numbers, int len) 
{
    int mid = len >> 1;
    int start = 0;
    int end = len - 1;

    int i = partition(numbers, len, start, end);
    while (i != mid) {
        if (i < mid) {
            start = i + 1;
            i = partition(numbers, len, start, end);
        } else {
            end = i - 1;
            i = partition(numbers, len, start, end);
        }
    }

    int result = numbers[mid];
    if (!check_morethanhalf(numbers, len, result))
        result = 0;

    return result;
}

void
test(string testname, int* numbers, int len, int expected_value)
{
    cout << testname << " begins: " << endl;

    int* copy = new int[len];
    for (int i = 0; i < len; ++i)
        copy[i] = numbers[i];

    int result = solution(numbers, len);
    if (result == expected_value)
        cout << "passed" << endl;
    else
        cout << "failed" << endl;

    cout << endl;
    delete[] copy;

}


void
test1()
{
    int numbers[] = {1, 2, 3, 4, 2, 2, 2, 5, 2, 2};
    test("test1", numbers, sizeof(numbers)/sizeof(int), 2);
}

void
test2()
{
    int numbers[] = {1, 2, 3, 4, 5, 6, 7};
    test("test2", numbers, sizeof(numbers)/sizeof(int), 2);
}

int 
main()
{
    test1();
    test2();

    return 0;
}

