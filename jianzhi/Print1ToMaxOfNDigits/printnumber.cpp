#include <iostream>
#include <cstring>


using namespace::std;

class BigNum
{
public:
    BigNum(): len(1) {
        memset(n, 0, sizeof(n));
        n[0] = 1;
    }

    BigNum(int num): len(num) {
        memset(n, 0, sizeof(n));
        for (int i = 0; i < len; ++i) {
            n[i] = 9;
        }
    }

    void operator += (const BigNum& rhs) {
        if (rhs.len > len)
            len = rhs.len;
        for (int i = 0; i < len; ++i) {
            n[i] += rhs.n[i];
            if (n[i] > 9) {
                ++ n[i+1];
                n[i] -= 10;
            }
        }

        if (n[len])
            len ++;
    }

    bool operator == (const BigNum& rhs) {
        bool flag = true;
        int length = len;
        if (len < rhs.len)
            length = rhs.len;

        for (int i = 0; i < len; ++i)
            if (n[i] != rhs.n[i]) {
                flag = false;
                break;
            }

        return flag;
    }

    void print() {
        for (int i = len-1; i >= 0; --i) {
            cout << n[i];
        }
    }

private:
    int n[500];
    int len;
    // int ndigits;
};

int
main()
{
    BigNum one;
    BigNum a(1);
    BigNum b(2);

    a += one;
    for (BigNum index; !(index==a); index+=one) {
        index.print();
        cout << " ";
    }

    cout << endl;
    b += one;
    for (BigNum index; !(index==b); index += one) {
        index.print();
        cout << " ";
    }


    return 0;
}
