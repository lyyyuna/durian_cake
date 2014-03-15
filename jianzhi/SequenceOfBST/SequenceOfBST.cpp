#include <iostream>
#include <string>

using namespace::std;

bool 
dfs(int sequence[], int len)
{
    if (sequence == NULL || len <= 0)
        return false;

    int root = sequence[len-1];

    int i = 0;
    for (; i < len - 1; ++i) {
        if (sequence[i] > root)
            break;
    }

    int j = i;
    for (; j < len - 1; ++j) {
        if (sequence[j] < root)
            return false;
    }

    bool left_flag = true;
    bool right_flag = true;
    if (i != 0)
        left_flag = dfs(sequence, i);
    if (j != len-1)
        right_flag = dfs(sequence+i, len-i-1);

    return left_flag && right_flag;
}

void
test(string testname, int seq[], int len, bool expected)
{
    cout << testname << " begins:" << endl;

    if (dfs(seq, len) == expected)
        cout << "passed" << endl;
    else
        cout << "failed" << endl;
}

//            10
//         /      \
//        6        14
//       /\        /\
//      4  8     12  16
void 
test1()
{
    int data[] = {4, 8, 6, 12, 16, 14, 10};
    test("Test1", data, sizeof(data)/sizeof(int), true);    
}

void 
test2()
{
    int data[] = {5};
    test("Test2", data, sizeof(data)/sizeof(int), true);
}

void 
test3()
{
    int data[] = {7, 4, 6, 5};
    test("Test3", data, sizeof(data)/sizeof(int), false);
}

void
test4()
{
    test("test4", NULL, 0, false);
}

int
main()
{
    test1();
    test2();
    test3();

    test4();

    return 0;
}

