#include <iostream>
#include <string>
#include "StackWithMin.h"

using namespace::std;

void
test(string testname, const StackWithMin<int>& st, int expected)
{
    cout << testname << " begins: " << endl;

    if (st.min() == expected)
        cout << "passed" << endl;
    else
        cout << "failed" << endl;

    cout << endl;
}

int
main()
{
    StackWithMin<int> st;
    
    st.push(2);
    test("test1", st, 2);

    st.push(12);
    test("test2", st, 2);

    st.push(1);
    test("test3", st, 1);

    st.pop();
    test("test4", st, 2);
}
