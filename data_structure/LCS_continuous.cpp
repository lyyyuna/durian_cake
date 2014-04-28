/*
    最长公共子字符串

*/

#include <iostream>
#include <string>
#include <vector>
#include <cstring>

using namespace std;

int main()
{
    string str1, str2;
    cin >> str1 >> str2;

    int32_t len1 = str1.length();
    int32_t len2 = str2.length();

    vector<vector<int32_t>> opt(len1+1, vector<int>(len2+1));

    for (int i = len1-1; i >= 0; i--) {
        for (int j = len2-1; j >= 0; j--) {
            if (str1[i] == str2[j])
                opt[i][j] = opt[i+1][j+1] + 1;
            else
                opt[i][j] = 0;
        }
    }

    int max = 0;
    for (int i = len1-1; i >= 0; i--) {
        for (int j = len2-1; j >= 0; j--) {
            if (max < opt[i][j])
                max = opt[i][j];
        }
    }
    cout << max << endl;
    //cout << opt[0][0] << endl;
/*
    int i = 0;
    int j = 0;
    while (i < len1 && j < len2) {
        if (str1[i] == str2[j]) {
            cout << str1[i];
            i ++;
            j ++;
        }
        else if (opt[i+1][j] >= opt[i][j+1])
            i ++;
        else
            j ++;
    }
*/
    return 0;
}
