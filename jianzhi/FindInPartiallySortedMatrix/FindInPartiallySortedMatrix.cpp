#include <iostream>

using namespace::std;

bool 
find_in_matrix(int *matrix, int rows, int columns, int number)
{
    bool found = false;

    if (matrix != NULL && rows > 0 && columns > 0) {
        int row = 0;
        int column = columns - 1;

        while (row < rows && columns >= 0) {
            if (matrix[row*columns + column] == number) {
                found = true;
                break;
            }
            if (matrix[row*columns + column] > number)
                column --;
            else
                row ++;
        }
    }

    return found;
}

void
test(char *testname, int *matrix, int rows, int columns, int number, bool expected)
{
    if (testname != NULL)
        cout << testname << " begins: " << endl;

    bool result = find_in_matrix(matrix, rows, columns, number);
    if (result == expected)
        cout << "passed. " << endl;
    else
        cout << "failed. " << endl;
}

void
test1()
{   
    int matrix[][4] = {{1, 2, 8, 9}, {2, 4, 9, 12}, {4, 7, 10, 13}, {6, 8, 11, 15}};
    test("test1", (int *)matrix, 4, 4, 7, true);
}

void 
test2()
{
    int matrix[][4] = {{1, 2, 8, 9}, {2, 4,9 ,12}, {4, 7, 10, 13}, {6, 8, 11, 15}};
    test("test2", (int *)matrix, 4, 4, 5, false);
}

int
main()
{
    test1();
    test2();
}
