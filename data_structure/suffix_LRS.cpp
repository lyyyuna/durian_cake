#include <iostream>
#include <cstring>
#include <iterator>
#include <algorithm>


using namespace std;

bool cmp(char * s1, char * s2)
{
    if (strcmp(s1, s2) < 0)
        return true;
    else
        return false;
}

int comlen(char * p, char * q)
{
    int len = 0;
    while (*p && *q && *p++==*q++)
        len ++;

    return len;
}

int maxIndex;
int maxLen;
void printLRS(char * str);

void LRS_suffix(char * arr, int size)
{
    int suffixIndex = maxLen = maxIndex = 0;
    char * suff[50];
    for (int i = 0; i < size; ++i)
        suff[i] = &arr[i];

    sort(suff, suff+size, cmp);
    //qsort(suff, size, sizeof(char *), pstrcmp);

    for (int i = 0; i < size-1; ++i)
    {
        int len = comlen(suff[i], suff[i+1]);
        if (len > maxLen)
        {
            maxLen = len;
            suffixIndex = i;
        }
    }

    printLRS(suff[suffixIndex]);
}

void printLRS(char * str)
{
    cout << "the length is " << maxLen << endl;
    int i = 0;
    while (maxLen--)
    {
        cout << str[i++];
    }
    cout << endl;
}

int main()
{
    char * test = "banana";
    LRS_suffix(test, strlen(test));
}
