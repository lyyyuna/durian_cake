#include <iostream>
#include <cmath>

using namespace::std;

bool is_equal(double num1, double num2);
double power_core(double base, int exp);

double
power(double base, int exponent)
{
    if (is_equal(base, 0) == true && exponent < 0)    
        return 0;

    int abs_exp = exponent;
    if (abs_exp < 0)
        abs_exp = -abs_exp;

    double res = power_core(base, abs_exp);
    if (exponent < 0)
        res = 1.0 / res;

    return res;
}

/*
double
power_core(double base, int exp)
{
    double res = 1;
    for (int i = 0; i < exp; ++i) {
        res *= base;
   }

    return res;
}
*/

double
power_core(double base, int exp)
{
    if (exp == 0)
        return 1;
    if (exp == 1)
        return base;

    int res = power(base, exp>>1);
    res *= res;
    if (exp & 1 == 1)
        res *= base;

    return res;
}

bool
is_equal(double num1, double num2)
{
    if ((num1-num2 > -0.0000001) && (num1-num2 < 0.0000001))
        return true;
    else
        return false;
}


void
test(double base, int exp, double expected)
{
    double res1 = power(base, exp);
    if (is_equal(res1, expected) )
        cout << "passed" << endl;
    else 
        cout << "failed" << endl;
}

int 
main()
{
    cout << "test 1" << endl;
    test(2, 3, 8);

    cout << "test 2" << endl;
    test(-2, 3, -8);

    cout << "test 3" << endl;
    test(2, -3, 0.125);

    return 0;
}   

