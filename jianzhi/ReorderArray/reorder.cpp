#include <iostream>

using namespace::std;

void
reorder(int* array, int len)
{
    if (array == NULL || len <= 0)
        return ;

    int* p1 = array;
    int* p2 = array+len-1;

    while (p1 < p2) {
        while (p1 < p2 && ((*p1 & 1) == 1))
            p1 ++;

        while (p1 < p2 && ((*p2 & 1) == 0))
            p2 --;

        if (p1 < p2) {
            int tmp;
            tmp = *p1;
            *p1 = *p2;
            *p2 = tmp;
        }
    }
}

void
print_array(int* array, int len)
{
    if (array == NULL || len <= 0)
        return;

    for (int i = 0; i < len; ++i) 
        cout << array[i] << " ";

    cout << endl;
}

void 
test(char* testname, int* array, int len)
{
    if(testname != NULL)
        cout << testname << " begins: " << endl;

    cout << "original array: " << endl;
    print_array(array, len);
    reorder(array, len);
    cout << "reordered array: " << endl;
    print_array(array, len);

    cout << endl;
}

void test1()
{
    int numbers[] = {1, 2, 3, 4, 5, 6, 7};
    test("test1", numbers, sizeof(numbers)/sizeof(int));
}

int 
main()
{
    test1();

    return 0;
}
