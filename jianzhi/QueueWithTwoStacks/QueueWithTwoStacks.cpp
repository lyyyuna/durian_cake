#include <iostream>
#include "CQueue.h"

using namespace::std;

void test(char actual, char expected)
{
    if (actual == expected)
        cout << "test passed" << endl;
    else
        cout << "test failed" << endl;
}

int main()
{
    CQueue<char>     queue;

    queue.appendTail('a');
    queue.appendTail('b');
    queue.appendTail('c');

    char head = queue.deleteHead();
    test(head, 'a');
    head = queue.deleteHead();
    test(head, 'c');

    return 0;
}
