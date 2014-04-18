#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

int count;

void Merge(int * a, int first, int mid, int last, int * tmp)
{
    int i = first;
    int j = mid+1;
    int m = mid;
    int n = last;
    int k = 0;
    while (i <= m && j <= n) {
        if (a[i] > a[j]) {
            tmp[k++] = a[j++];
            count += mid-i+1;
        }
        else {
            tmp[k++] = a[i++];
        }
    }

    while (i <= m)
        tmp[k++] = a[i++];
    while (j <= n)
        tmp[k++] = a[j++];

    for (int i = 0; i < k; i++)
        a[first+i] = tmp[i];
}

void MergeSort(int * a, int first, int last, int * tmp)
{
    if (first < last) {
        int mid = first/2 + last/2;
        MergeSort(a, first, mid, tmp);
        MergeSort(a, mid+1, last, tmp);
        Merge(a, first, mid, last, tmp);
    }
}

int main()
{
    int data[] = {7, 5, 6, 4};
    int tmp[4];

    MergeSort(data, 0, 3, tmp);
    cout << count << endl;

    return 0;
}
