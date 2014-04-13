/*
题目2 : K-th string
时间限制:10000ms
单点时限:1000ms
内存限制:256MB
Description
Consider a string set that each of them consists of {0, 1} only. All strings in the set have the same number of 0s and 1s. Write a program to find and output the K-th string according to the dictionary order. If such a string doesn’t exist, or the input is not valid, please output “Impossible”. For example, if we have two ‘0’s and two ‘1’s, we will have a set with 6 different strings, {0011, 0101, 0110, 1001, 1010, 1100}, and the 4th string is 1001.

Input
The first line of the input file contains a single integer t (1 ≤ t ≤ 10000), the number of test cases, followed by the input data for each test case.
Each test case is 3 integers separated by blank space: N, M(2 <= N + M <= 33 and N , M >= 0), K(1 <= K <= 1000000000). N stands for the number of ‘0’s, M stands for the number of ‘1’s, and K stands for the K-th of string in the set that needs to be printed as output.

Output
For each case, print exactly one line. If the string exists, please print it, otherwise print “Impossible”.

样例输入
3
2 2 2
2 2 7
4 7 47
样例输出
0101
Impossible
01010111011
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>

using namespace std;

char num[70];

int main()
{
    int T;
    cin >> T;
    for (int i = 0; i < T; ++i) {
        int N, M, K;
        cin >> N >> M >> K;
        int total = N + M;
        
        string zero(N, '0');
        string one(M, '1');
        string number = zero+one;

        map<string, int, less<string> > dic;
        do {
            if (dic.find(number) == dic.end())
                dic[number] = 1;
        } while (next_permutation(number.begin(), number.end()));
         
        int k = 1;
        map<string, int, less<string> >::iterator iter = dic.begin();
        for (; iter != dic.end(); ++iter) {
            if (k == K)
                break;
            k++;
            
        }
        // k--;

        if (k < K || iter == dic.end())
            cout << "Impossible" << endl;
        else
            cout << iter->first << endl;
    }
}