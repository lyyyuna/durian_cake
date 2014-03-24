#include <iostream>

using namespace::std;

void
dfs(char* p_str, char* p_begin);

void
permutation(char* p_str)
{
    if (p_str == NULL)
        return ;
    dfs (p_str, p_str);
}

void
dfs(char* p_str, char* p_index)
{
    if (*p_index == '\0') {
        cout << p_str << endl;
        return ;
    }

    for (char* p_ch = p_index; *p_ch != '\0'; ++p_ch) {
        char tmp;
        tmp = *p_ch;
        *p_ch = *p_index;
        *p_index = tmp;

        dfs(p_str, p_index+1);

        tmp = *p_ch;
        *p_ch = *p_index;
        *p_index = tmp;
    }
}

void
test(char *p_str)
{
    cout << "test begins: " << endl;

    permutation(p_str);

    cout << endl;
}

int 
main()
{
    char str1[] = "abc";
    test(str1);

    return 0;
}
