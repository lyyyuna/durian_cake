#include <iostream>
#include <stack>
#include <string>

using namespace::std;

bool
is_poporder(const int* p_push, const int* p_pop, int len)
{
    if (p_push != NULL && p_pop != NULL && len > 0) {
        const int* p_nextpush = p_push;
        const int* p_nextpop = p_pop;

        stack<int> push_stack;

        while (p_nextpop - p_pop < len) {
            while (push_stack.empty() || push_stack.top() != *p_nextpop) {
                
                if (p_nextpush - p_push == len)
                    break;

                push_stack.push(*p_nextpush ++);
            }
            
            if (push_stack.top() != *p_nextpop) 
                break;

            push_stack.pop();
            p_nextpop ++;
        }

        if (push_stack.empty() && p_nextpop - p_pop == len)
            return true;
    }

    return false;
}

void 
test(string testname, const int* p_push, const int* p_pop, int len, bool expected)
{
    cout << testname << " begins: " << endl;

    if (is_poporder(p_push, p_pop, len) == expected)
        cout << "passed" << endl;
    else
        cout << "failed" << endl;

    cout << endl;
}

void
test1()
{
    int push[] = {1, 2, 3, 4, 5};
    int pop[] = {4, 5, 3, 2, 1};

    test("test1", push, pop, 5, true);
}

void
test2()
{
    int push[] = {1, 2, 3, 4, 5};
    int pop[] = {3, 5, 4, 1, 2};

    test("test2", push, pop, 5, false);
}   

void
test3()
{
    test("test3", NULL, NULL, 0, false);
}

int
main()
{
    test1();
    test2();
    test3();

    return 0;
}
