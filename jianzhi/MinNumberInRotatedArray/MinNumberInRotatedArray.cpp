#include <iostream>

using namespace::std;

int
min_special(int* array, int index1, int index2)
{
    int min = array[index1];
    for (int i = index1; i <= index2; ++i) {
        if (array[i] < min)
            min = array[i];
    }

    return min;
}

int
min_in_array(int* array, int len)
{
    if (array == NULL || len < 0)
        return -1;

    int index1 = 0;
    int index2 = len - 1;
    // for if the array is already sorted
    int mid = index1;
    while (array[index1] >= array[index2]) {
        if (index1 + 1 == index2) {
            mid = index2;
            break;
        }
        
        mid = (index1+index2) / 2;
        /* special case: i1 = i2 = mid */
        if (array[index1] == array[index2] && array[index1] == array[mid]) {
            return min_special(array, index1, index2);
        }

        if (array[index1] >= array[mid]) {
            index2 = mid;
        } else if (array[index2] <= array[mid]) {
            index1 = mid;
        }
    }

    return array[mid];
}

void
test(int* array, int len, int expected)
{
    int result = 0;
    result = min_in_array(array, len);
    if (result == -1)
        cout << "invalid input" << endl;

    for (int i = 0; i < len; ++i)
        cout << array[i] << " ";
    cout << endl;

    if (result == expected)
        cout << "test passed" << endl;
    else
        cout << "test failed" << endl;
}

int 
main()
{
    int array1[] = {3, 4, 5, 1, 2};   
    test(array1, sizeof(array1)/sizeof(int), 1);

    int array2[] = {3, 4, 5, 1, 1, 2};
    test(array2, sizeof(array2)/sizeof(int), 1);

    int array3[] = {3, 4, 5, 1, 2, 2};
    test(array3, sizeof(array3)/sizeof(int), 1);

    int array4[] = {3};
    test(array4, sizeof(array4)/sizeof(int), 3);

    int array5[] = {1, 0, 1, 1, 1};
    test(array5, sizeof(array5)/sizeof(int), 0);

    int array6[] = {1, 2, 3, 4, 5};
    test(array6, sizeof(array6)/sizeof(int), 1);

    return 0;
}
