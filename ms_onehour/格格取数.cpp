/*
题目3 : 格格取数
时间限制:2000ms
单点时限:1000ms
内存限制:256MB
描述
给你一个m x n (1 <= m, n <= 100)的矩阵A (0<=aij<=10000)，要求在矩阵中选择一些数，要求每一行，每一列都至少选到了一个数，使得选出的数的和尽量的小。



输入
多组测试数据。首先是数据组数T

对于每组测试数据，第1行是两个正整数m, n，分别表示矩阵的行数和列数。

接下来的m行，每行n个整数，之间用一个空格分隔，表示矩阵A的元素。



输出
每组数据输出一行，表示选出的数的和的最小值。



数据范围
小数据：1 <= m, n <= 5

大数据：1 <= m, n <= 100





样例输入
2
3 3
1 2 3
3 1 2
2 3 1
5 5
1 2 3 4 5
5 1 2 3 4
4 5 1 2 3
3 4 5 1 2
2 3 4 5 1
样例输出
Case 1: 3
Case 2: 5
*/

//source here
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

struct Point
{
    int x;
    int y;
    int value;
};

const int MAXN = 104;

int flagrow[MAXN];
int flagcol[MAXN];
vector<Point> matrix;

int cmp(const Point & a, const Point & b)
{
    return a.value < b.value ;
}

int cmp2(const Point & a, const Point & b)
{
    return a.value > b.value ;
}

int main()
{
    int T;
    cin >> T;
    for (int index = 0; index < T; ++index) {
        int M, N;
        cin >> M >> N;
        for (int i = 0; i < M; ++i)
            for (int j = 0; j < N; ++j) {
                Point tmp;
                tmp.x = i;
                tmp.y = j;
                cin >> tmp.value;
                matrix.push_back(tmp);
                flagrow[i] = 0;
                flagcol[j] = 0;
            }

        sort(matrix.begin(), matrix.end(), cmp);

        vector<Point> minarr;
        int total = M + N;
        int i(0);
        while (total>0) {
            int x = matrix[i].x;
            int y = matrix[i].y;
            if (flagrow[x] == 0) 
                total --;
            if (flagcol[y] == 0)
                total --;
            flagcol[y]++;
            flagrow[x]++;

            minarr.push_back(matrix[i]);
            i++;
        }

        int min = 0;
        sort(minarr.begin(), minarr.end(), cmp2);
        for (int i = 0; i < minarr.size() ; ++i) {
            int x = minarr[i].x;
            int y = minarr[i].y;
            if (flagrow[x] > 1 && flagcol[y] > 1) {
                flagrow[x]--;
                flagcol[y]--;
            } else
                min += minarr[i].value;
        }

        minarr.clear();
        matrix.clear();
        cout << "Case " << index+1 << ": " << min << endl;
    }
}