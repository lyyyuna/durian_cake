#include <iostream>
#include <string>
#include <cstdio>
#include <algorithm>

using namespace std;



void Permutation(char * pStr, char * pBegin)
{
    if (*pBegin == '\0')
        cout << pStr << endl;
    else {
        for (char * pIndex = pBegin; *pIndex != '\0'; ++pIndex) {
            char temp;
            temp = *pIndex;
            *pIndex = *pBegin;
            *pBegin = temp;

            Permutation(pStr, pBegin+1);

            temp = *pIndex;
            *pIndex = *pBegin;
            *pBegin = temp;

        }
    }
}

void Permutation(char * pStr)
{
    if (pStr == NULL)
        return ;
    Permutation(pStr, pStr);
}

void Test1(char * pStr)
{
    Permutation(pStr);

    cout << endl;
}

int main()
{
    char str[] = "abc";
    Test1(str);

    do {
        cout << str << endl;
    } while (next_permutation(str, str+3));

    return 0;
}
