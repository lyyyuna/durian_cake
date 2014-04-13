/*
题目3 : Reduce inversion count
时间限制:10000ms
单点时限:1000ms
内存限制:256MB
Description
Find a pair in an integer array that swapping them would maximally decrease the inversion count of the array. If such a pair exists, return the new inversion count; otherwise returns the original inversion count.

Definition of Inversion: Let (A[0], A[1] ... A[n], n <= 50) be a sequence of n numbers. If i < j and A[i] > A[j], then the pair (i, j) is called inversion of A.

Example:
Count(Inversion({3, 1, 2})) = Count({3, 1}, {3, 2}) = 2
InversionCountOfSwap({3, 1, 2})=>
{
 InversionCount({1, 3, 2}) = 1 <-- swapping 1 with 3, decreases inversion count by 1
 InversionCount({2, 1, 3}) = 1 <-- swapping 2 with 3, decreases inversion count by 1
 InversionCount({3, 2, 1}) = 3 <-- swapping 1 with 2 , increases inversion count by 1
}



Input
Input consists of multiple cases, one case per line.Each case consists of a sequence of integers separated by comma. 

Output
For each case, print exactly one line with the new inversion count or the original inversion count if it cannot be reduced.


样例输入
3,1,2
1,2,3,4,5
样例输出
1
0
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

const int MAX = 100;
int number[MAX];
int total;

int count_inversion(int * num)
{
	int inversion = 0;
	for (int i = 0; i < total-1; ++i)
		for (int j = i+1; j < total; ++j) {
			if (num[i] > num[j])
				inversion ++;
		}

	return inversion;
}

int main()
{
	string line;
	while (getline(cin, line)) {
		total = 0;
		int last = 0;
		for (size_t pos = 0 ; pos < line.length() ; pos++ )
		{
			string temp ;

			while ( (pos < line.length()) && (line[pos] != ',') ) pos++;
			temp = line.substr(last , pos - last);
			last = pos + 1 ;
			number[total] = atoi(temp.c_str());
			total ++;
		}



		int min_inv = count_inversion(number);

		for (int i = 0; i < total-1; ++i)
			for (int j = i+1; j < total; ++j) {
				int temp;
				temp = number[i];
				number[i] = number[j];
				number[j] = temp;

				int new_inv = count_inversion(number);
				if (min_inv > new_inv)
					min_inv = new_inv;

				temp = number[i];
				number[i] = number[j];
				number[j] = temp;
			}

			cout << min_inv << endl;
	}
}
