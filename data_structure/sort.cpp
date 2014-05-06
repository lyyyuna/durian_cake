#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <limits>
#include <iomanip>
#include <iterator>

using namespace std;

void swap(int & a, int & b)
{
    int temp;
    temp = a;
    a = b;
    b = temp;
}

/////////////////////////////////////////////////////////////////////
/*
冒泡排序算法的运作如下：

比较相邻的元素。如果第一个比第二个大，就交换他们两个。
对每一对相邻元素作同样的工作，从开始第一对到结尾的最后一对。在这一点，最后的元素应该会是最大的数。
针对所有的元素重复以上的步骤，除了最后一个。
持续每次对越来越少的元素重复上面的步骤，直到没有任何一对数字需要比较。
*/
void bubble_sort(int * arr, int len)
{
    for (int i = 0; i < len; i++)
        for (int j = len-1; j > i; --j) {
            if (arr[j] < arr[j-1])
                swap(arr[j], arr[j-1]);
        }
}

//////////////////////////////////////////////////////////////////////
/* 选择排序 */
void selection_sort(int * arr, int len)
{
    for (int i = 0; i < len; ++i) {
        int min = i;
        for (int j = i; j < len; ++j) {
            if (arr[min] > arr[j])
                min = j;
        }
        if (min != i)
            swap(arr[min], arr[i]);
    }
}

////////////////////////////////////////////////////////////////
/*
插入排序
每一次取一个元素，都要将该元素与之前已经排序好的元素进行比较。
*/
void insertion_sort(int * arr, int len)
{
    for (int i = 1; i < len; ++i)
    {
        int key  = arr[i];
        int j = i-1;
        while(j>=0 && arr[j]>key)
        {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;

    }
}

//////////////////////////////////////////////////////////
/*
归并排序
*/
void merge(int * array, int p, int q, int r)
{
    int arr_l[q-p+1];
    int arr_r[r-q+1];

    int n1 = q-p+1;
    int n2 = r-q;

    for (int i = 0; i < n1; i++)
        arr_l[i] = array[p+i];
    for (int i = 0; i < n2; i++)
        arr_r[i] = array[q+i+1];


    int i = 0;
    int j = 0;
    for (int k = p; k < r+1; k++)
    {
        if (arr_l[i] < arr_r[j])
            array[k] = arr_l[i++];
        else
            array[k] = arr_r[j++];
    }
}

void merge_sort(int * arr, int p, int r)
{
    if (p < r)
    {
        int mid = (p+r)/2;
        merge_sort(arr, p, mid);
        merge_sort(arr, mid+1, r);
        merge(arr, p, mid, r);
    }
}

/////////////////////////////////////////////////////////////////////////
/*
堆排序
*/
#define LEFT(i) 2*(i)+1
#define RIGHT(i) 2*(i)+2

void max_heapify(int * arr, int heap_size, int i)
{
    int largest = i;
    if ((arr[LEFT(i)] >= arr[i]) && (LEFT(i) < heap_size))
        largest = LEFT(i);
    if ((arr[RIGHT(i)] >= arr[largest]) && (RIGHT(i) < heap_size))
        largest = RIGHT(i);

    if (largest != i)
    {
        swap(arr[i], arr[largest]);
        max_heapify(arr, heap_size, largest);
    }
}

void build_max_heap(int * arr, int size)
{
    for (int i = (size/2-1); i >= 0; i--)
    {
        max_heapify(arr, size, i);
    }
}

void heap_sort(int * arr, int size)
{
    build_max_heap(arr, size);

    for (int i = size-1; i >= 0; --i)
    {
        swap(arr[0], arr[i]);
        max_heapify(arr, i, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
/*
快速排序
*/
int partition(int * arr, int p, int r)
{
    int small = p-1;
    for (int i = p; i < r; ++i)
    {
        if (arr[i] < arr[r])
        {
            ++ small;
            if (i != small)
                swap(arr[small], arr[i]);
        }
    }
    ++small;
    swap(arr[small], arr[r]);
    return small;
}

void quick_sort(int * arr, int p, int r)
{
    if (p < r)
    {
        int mid = partition(arr, p, r);
        quick_sort(arr, p, mid-1);
        quick_sort(arr, mid, r);
    }
}

int main()
{
    ostream_iterator<int> output(cout, " ");

    /* test1 */
    int arr1[] = {3, 4, 5, 1, 6, -1};
    copy(arr1, arr1+6, output);
    cout << endl;
    bubble_sort(arr1, 6);
    copy(arr1, arr1+6, output);
    cout << endl << endl;

    /* test for selection sort */
    int arr2[] = {3, 4, 5, 1, 6, -1};
    copy(arr2, arr2+6, output);
    cout << endl;
    selection_sort(arr2, 6);
    copy(arr2, arr2+6, output);
    cout << endl << endl;

    /* test for insertion sort */
    int arr3[] = {3, 4, 5, 1, 6, -1};
    copy(arr3, arr3+6, output);
    cout << endl;
    insertion_sort(arr3, 6);
    copy(arr3, arr3+6, output);
    cout << endl << endl;

    /* test for insertion sort */
    int arr4[] = {3, 4, 5, 1, 6, -1};
    copy(arr4, arr4+6, output);
    cout << endl;
    merge_sort(arr4, 0, 6-1);
    copy(arr4, arr4+6, output);
    cout << endl << endl;

    /* test for quick_sort */
    int arr5[] = {3, 4, 5, 1, 6, -1};
    copy(arr5, arr5+6, output);
    cout << endl;
    quick_sort(arr5, 0, 6-1);
    copy(arr5, arr5+6, output);
    cout << endl << endl;

    /* test for heap_sort */
    int arr6[] = {3, 4, 5, 1, 6, -1};
    copy(arr6, arr6+6, output);
    cout << endl;
    heap_sort(arr6, 6);
    copy(arr6, arr6+6, output);
    cout << endl << endl;
}
