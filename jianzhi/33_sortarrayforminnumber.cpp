#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <iterator>

using namespace std;

int cmp(const string & a, const
        string & b)
{
    string tmp1 = a + b;
    string tmp2 = b + a;

    return tmp1 < tmp2;
}

void PrintMinNumber(int * numbers, int len)
{
    vector<string> num;
    char buf[50];
    for (int i = 0; i < len; ++i) {
        sprintf(buf, "%d", numbers[i]);
        num.push_back(string(buf));
    }

    sort(begin(num), end(num), cmp);

    copy(begin(num), end(num), ostream_iterator<string>(cout));
}


int main()
{
    int arr[] = {3, 32, 321};

    PrintMinNumber(arr, 3);

    return 0;
}
