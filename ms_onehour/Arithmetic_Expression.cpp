/*时间限制:2000ms
单点时限:200ms
内存限制:256MB
描述
Given N arithmetic expressions, can you tell whose result is closest to 9?

输入
Line 1: N (1 <= N <= 50000).
Line 2..N+1: Each line contains an expression in the format of "a op b" where a, b are integers (-10000 <= a, b <= 10000) and op 

is one of addition (+), subtraction (-), multiplication (*) and division (/). There is no "divided by zero" expression.


输出
The index of expression whose result is closest to 9. If there are more than one such expressions, output the smallest index.


样例输入
4
901 / 100
3 * 3
2 + 6
8 - -1
样例输出
2*/

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <map>
#include <functional> 

using namespace std;

int main()
{
    int num;
    cin >> num;
    int index = 0;
    map<double, int, less<double> > res;
    while (num--) {
        index++;
        int x, y;
        string op;
        cin >> x >> op >> y;
        double result;
        if (op == "/")
            result = x*1.0 / y;
        if (op == "*")
            result = x * y;
        if (op == "+")
            result = x + y;
        if (op == "-")
            result = x - y;
        
        result = fabs(result-9);
        if (res.find(result) == res.end())
        res[result] = index;
    }
    map<double, int, less<double> > ::iterator iter = res.begin();
    cout << iter->second << endl;
}
