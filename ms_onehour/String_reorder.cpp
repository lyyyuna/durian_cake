/*
题目1 : String reorder
时间限制:10000ms
单点时限:1000ms
内存限制:256MB
Description
For this question, your program is required to process an input string containing only ASCII characters between ‘0’ and ‘9’, or between ‘a’ and ‘z’ (including ‘0’, ‘9’, ‘a’, ‘z’).

Your program should reorder and split all input string characters into multiple segments, and output all segments as one concatenated string. The following requirements should also be met,
1. Characters in each segment should be in strictly increasing order. For ordering, ‘9’ is larger than ‘0’, ‘a’ is larger than ‘9’, and ‘z’ is larger than ‘a’ (basically following ASCII character order).
2. Characters in the second segment must be the same as or a subset of the first segment; and every following segment must be the same as or a subset of its previous segment.

Your program should output string “<invalid input string>” when the input contains any invalid characters (i.e., outside the '0'-'9' and 'a'-'z' range).



Input

Input consists of multiple cases, one case per line. Each case is one string consisting of ASCII characters.

Output

For each case, print exactly one line with the reordered string based on the criteria above.


样例输入
aabbccdd
007799aabbccddeeff113355zz
1234.89898
abcdefabcdefabcdefaaaaaaaaaaaaaabbbbbbbddddddee
样例输出
abcdabcd
013579abcdefz013579abcdefz
<invalid input string>
abcdefabcdefabcdefabdeabdeabdabdabdabdabaaaaaaa
*/

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

bool isvalid(char c){
    if (c>='0' && c<='9')
        return true;
    if (c >= 'a' && c <='z')
        return true;
    if (c >= 'A' && c <= 'Z')
        return true;

    return false;
}

int main()
{
    string inputstr;
    while (cin >> inputstr) {
        for (string::iterator iter = inputstr.begin(); iter != inputstr.end(); ++iter) {
            if (!isvalid(*iter)) {
                cout << "<invalid input string>" << endl;
                goto flag;
            }
        }

        sort(inputstr.begin(), inputstr.end());
        while (!inputstr.empty()) {
            char last = 0;
            for (string::iterator iter = inputstr.begin(); iter != inputstr.end();) {
                if (last != *iter) {
                    cout << *iter;
                    last = *iter;
                    inputstr.erase(iter);
                    // --iter;
                } 
                else {
                    last = *iter;
                     ++iter;
                }
                
            }
        }

        cout << endl;


    flag:
        ;
    }
}