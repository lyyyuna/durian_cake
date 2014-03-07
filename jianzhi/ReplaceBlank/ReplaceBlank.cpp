#include <iostream>
#include <string>

using namespace::std;

void
replace_blank(char *str, int length)
{
    if (str == NULL && length <= 0)
        return;

    int i = 0;
    int number_blank = 0;
    while (str[i] != '\0') {
        if (str[i] == ' ')
            number_blank ++;
        i ++;
    }

    /* the length of new character */
    int new_length = 2*number_blank + i;
    if (new_length > length)
        return ;

    int ori_length = i;
    int new_index = new_length;
    int ori_index = ori_length;

    while (ori_index >= 0 && new_index > ori_index) {
        if (str[ori_index] != ' ') {
            str[new_index--] = str[ori_index];
        } else {
            str[new_index--] = '0';
            str[new_index--] = '2';
            str[new_index--] = '%';
        }

        ori_index --;
    }

    cout << str << endl;
}

void 
test(char *test_name, char *str, int length, const char *expected)
{
    if (test_name != NULL)
        cout << test_name << " begins: " << endl;

    replace_blank(str, length);

    if (expected == NULL && str == NULL)
        cout << "passed." << endl;
    else if (expected == NULL && str != NULL)
        cout << "failed." << endl;
    else if (strcmp(expected, str) == 0)
        cout << "passed." << endl;
    else
        cout << "failed." << endl;
}

void
test1()
{
    const int length = 100;

    char test_str[length] = "hello world";
    test("test1", test_str, length, "hello%20world");
}

int
main()
{
    test1();
}

void func()
{

}
