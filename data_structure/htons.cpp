#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <limits>
#include <iomanip>

using namespace std;

uint32_t htons(uint32_t a)
{
    return (((uint32_t)a & 0xff000000) >> 24) |
    (((uint32_t)a & 0x00ff0000) >> 8) |
    (((uint32_t)a & 0x0000ff00) << 8) |
    (((uint32_t)a & 0x000000ff) << 24);
}

int main()
{
    uint32_t x = 0x12345678;
    cout << hex << x << endl;
    cout << htons(x) << endl;
}
