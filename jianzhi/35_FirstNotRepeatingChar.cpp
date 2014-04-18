#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

int hashtable[400];

char FirstNotRepeatingChar(char * pStr)
{
    if (pStr == NULL)
        return 0;
    char * pIndex = pStr;
    while (*pIndex != '\0') {
        hashtable[*pIndex++] ++;
    }
    pIndex = pStr;
    while (*pIndex != '\0') {
        if (hashtable[*pIndex] == 1)
            return *pIndex;
        pIndex ++;
    }

    return '\0';
}

int main()
{
    cout << FirstNotRepeatingChar("abaccdeff") << endl;

    return 0;
}
