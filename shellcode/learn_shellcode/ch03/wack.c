char sc[] = "\x31\xdb"
            "\x31\xc0"
            "\xb0\x01"
            "\xcd\x80";

int main()
{
    int *ret;
    ret = (int *)&ret + 2;
    (*ret) = (int)sc;
}    
