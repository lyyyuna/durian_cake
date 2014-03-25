#include <iostream>
#include <set>
#include <vector>
#include <string>
#include "Array.h"

using namespace std;

void
solution1(int* numbers, int len, int k)
{
    if (numbers == NULL || len <= 0 || k <= 0 || k > len)
        return ;

    int start = 0;
    int end = len - 1;
    int index = partition(numbers, len, start, end);
    while (index != k-1) {
        if (index > k-1) {
            end = index-1;
            index = partition(numbers, len, start, end);
        } else {
            start = index+1;
            index = partition(numbers, len, start, end);
        }
    }
}

// =======================
typedef multiset<int, greater<int> > intset;
typedef multiset<int, greater<int> >::iterator setIterator;
void
solution2(const vector<int>& data, intset& leastNumbers, int k)
{
    leastNumbers.clear();

    if (k<1 && data.size()<k)
        return ;

    vector<int>::const_iterator iter = data.begin();
    for (; iter != data.end(); ++iter) {
        if ((leastNumbers.size()) < k)
            leastNumbers.insert(*iter);
        else {
            setIterator iterBig = leastNumbers.begin();

            if (*iter < *iterBig) {
                leastNumbers.erase(iterBig);  
                leastNumbers.insert(*iter);
            }
        }
    }
}

void
test(string testname, int* data, int len, int* expected, int k)
{
    cout << testname << " begins:" << endl;

    vector<int> vecData;
    for (int i = 0; i < len; ++i)
        vecData.push_back(data[i]);
    cout << "expected result" << endl;
    for (int i = 0; i < k; ++i)
        cout << expected[i] << " ";
    cout << endl;
    cout << "solution1: " << endl;
    solution1(data, len, k);
    for (int i = 0; i < k; ++i)
        cout << data[i] << " ";
    cout << endl;

    cout << "solution2: " << endl;
    intset leastNumbers;
    solution2(vecData, leastNumbers, k);
    for (setIterator iter = leastNumbers.begin(); iter != leastNumbers.end(); ++iter)
        cout << *iter << " ";
    cout << endl << endl;
}

void
test1()
{
    int data[] = {4, 5, 1 ,6, 2, 7, 3, 8};
    int expected[] = {1, 2 , 3 ,4};
    test("test1", data, sizeof(data)/sizeof(int), expected, sizeof(expected)/sizeof(int));
}

int
main()
{
    test1();

    return 0;
}
