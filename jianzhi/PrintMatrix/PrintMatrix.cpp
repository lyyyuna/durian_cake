#include <iostream>

using namespace::std;

void print_circle(int**, int, int, int);

void
print_matrix(int** array, int columns, int rows)
{
    if (array == NULL || columns <= 0 || rows <= 0)
        return ;

    int start = 0;
    while (columns>start*2 && rows>start*2) {
        print_circle(array, columns, rows, start);

        ++ start;
    }
}

void 
print_circle(int** array, int columns, int rows, int start)
{
    int end_x = columns - 1 - start;
    int end_y = rows - 1 - start;

    // left 2 right
    for (int i = start; i <= end_x; ++i) {
        int num = array[start][i];
        cout << num << " ";
    }

    // up 2 down
    if (start < end_y) {
        for (int i = start+1; i <= end_y; ++i) {
            int num = array[i][end_x];
            cout << num << " ";
        }
    }

    // right 2 left
    if (start < end_y && start < end_x) {
        for (int i = end_x-1; i >= start; --i) {
            int num = array[end_y][i];
            cout << num << " ";
        }
    }

    // down 2 up
    if (start < end_x && start < end_y-1) {
        for (int i = end_y-1; i >= start+1; --i) {
            int num = array[i][start];
            cout << num << " ";
        }
    }
}

void
test(int columns, int rows)
{
    cout << "test begins: ";

    if (columns < 1 || rows < 1)
        return ;

    int** numbers = new int*[rows];
    for (int i = 0; i < rows; ++i) {
        numbers[i] = new int[columns];
        for (int j = 0; j < columns; ++j)
            numbers[i][j] = i*columns + j + 1;
    }

    print_matrix(numbers, columns, rows);
    cout << endl;

    for(int i = 0; i < rows; ++i)
        delete[] (int*)numbers[i];

    delete[] numbers;    
}

int
main()
{
    test(4, 4);

    return 0;
}
