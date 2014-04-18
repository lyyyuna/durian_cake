#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

int GetFirstK(int * data, int len, int k, int _start, int _end)
{
    if (_start > _end)
        return -1;

    int mid = (_start+_end)/2;
    int midData = data[mid];

    if (midData == k) {
        if ((mid > 0 && data[mid-1] != k) || mid == 0)
            return mid;
        else
            _end = mid-1;
    }
    else if (midData > k)
        _end = mid - 1;
    else
        _start = mid + 1;

    return GetFirstK(data, len, k, _start, _end);
}

int GetLastK(int * data, int len, int k, int _start, int _end)
{
    if (_start > _end)
        return -1;

    int mid = (_start + _end) / 2;
    int midData = data[mid];

    if (midData == k) {
        if ((mid < len-1 && data[mid+1] != k) || mid == len-1)
            return mid;
        else
            _start = mid + 1;
    }
    else if (mid < k)
        _start = mid + 1;
    else
        _end = mid - 1;
}

int GetNumberOfK(int * data, int len, int k)
{
    int num = 0;
    if (data != NULL && len > 0) {
        int first = GetFirstK(data, len, k, 0, len-1);
        int last = GetLastK(data, len, k, 0, len-1);

        num = last - first + 1;
    }

    return num;
}

int main()
{
    int data[] = {1, 2, 3, 3, 3, 3, 4, 5};
    cout << GetNumberOfK(data, 8, 3) << endl;

    return 0;
}
