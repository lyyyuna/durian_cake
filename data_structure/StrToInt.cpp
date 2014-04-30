#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <limits>

using namespace std;

enum Status {kValid = 0, kInvalid};
int g_nStatus = kValid;

int StrToInt(const char * str)
{
    g_nStatus = kInvalid;
    long long num = 0;

    if (str != NULL) {
        const char * digit = str;

        /* tell if the first char is + or - or none */
        bool minus = false;
        if (*digit == '+')
            digit ++;
        else if (*digit == '-') {
            digit ++;
            minus = true;
        }

        while (*digit != '\0') {
            if (*digit >= '0' && *digit <= '9') {
                num = num*10 + (*digit-'0');

                if (num > std::numeric_limits<int>::max()){
                    g_nStatus = kInvalid;
                    break;
                }


                digit++;
            }
            else {
                g_nStatus = kInvalid;
                break;
            }
        }

        if (*digit == '\0')
        {
            g_nStatus = kValid;
            if (minus)
                num = 0 - num;
        }
    }

    return static_cast<int>(num);
}

int main()
{
    string num;
    cin >> num;
    cout << StrToInt(num.c_str()) << endl;
}
