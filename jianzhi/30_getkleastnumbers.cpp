#include <iostream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <iterator>

using namespace std;


int RandomInRange(int small, int big)
{
    int random = rand() % (big-small+1) + small;
    return random;
}

int Swap(int * a, int * b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

int Partition(int * numbers, int len, int _start, int _end)
{
    int index = RandomInRange(_start, _end);
    Swap(&numbers[_end], &numbers[index]);

    int small = _start-1;
    for (int i = _start; i < _end; ++i) {
        if (numbers[i] < numbers[_end]) {
            ++small;
            if (small != i)
                Swap(&numbers[small], &numbers[i]);
        }
    }
    ++small;
    Swap(&numbers[_end], &numbers[small]);

    return small;
}

void GetLeastNumbers(int * input, int n, int * output, int k)
{
    if (input == NULL || output == NULL || k>n || n <= 0 || k <= 0) {
        return;
    }

    int _start = 0;
    int _end = n-1;
    int index = Partition(input, n, _start, _end);
    while (index != k-1) {
        if (index > k-1) {
            _end = index-1;
            index = Partition(input, n, _start, _end);
        }
        else {
            _start = index+1;
            index = Partition(input, n, _start, _end);
        }
    }

    for (int i = 0; i < k; ++i)
        output[i] = input[i];

    return;
}

#include <set>
typedef multiset<int, greater<int> >    intSet;
typedef multiset<int, greater<int> >::iterator setIterator;

void GetLeastNumbers2(int * input, int len, intSet & leastNumber, int k)
{
    for (int i = 0; i < len; ++i) {
        if (leastNumber.size() < k)
            leastNumber.insert(input[i]);
        else {
            setIterator iter = leastNumber.begin();

            if (*iter > input[i]) {
                leastNumber.erase(iter);
                leastNumber.insert(input[i]);
            }
        }
    }
}

int main()
{
    int input[] = {4, 5, 1, 6, 2, 7, 3, 8};
    int output[8];

    GetLeastNumbers(input, 8, output, 4);

    copy(output, output+4, ostream_iterator<int>(cout));

    intSet outputset;
    GetLeastNumbers2(input, 8, outputset, 4);
    for (setIterator iter = outputset.begin(); iter != outputset.end(); ++iter) {
        cout << *iter << " ";
    }
}
