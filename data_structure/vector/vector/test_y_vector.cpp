#include "StdAfx.h"

#include "y_vector.h"

using namespace std;

void test_y_vector()
{
    cout << endl;
    cout << "test for y_vector" << endl;
    y_vector<int> yvc(4, 1);

    cout << "size = " << yvc.size() << endl;
    cout << "capacity = " << yvc.capacity() << endl;
    cout << yvc[3] << endl;

    yvc.push_back(4);
    cout << "size = " << yvc.size() << endl;
    cout << "capacity = " << yvc.capacity() << endl;
    cout << yvc[4] << endl;

    yvc.insert(yvc.begin(), 2, 9);
    cout << "size = " << yvc.size() << endl;
    cout << "capacity = " << yvc.capacity() << endl;
    cout << yvc[4] << endl;
}