unsigned long get_sp()
{
__asm__("movl %esp, %eax");
}

void fuc(char * argv[])
{
    char l_arr[256];
    printf("address : 0x%x \n", get_sp());
    strcpy(l_arr, argv[1]);
}

int main(int argc, char * argv[])
{
    //char l_arr[512] = {0};
    printf("address : 0x%x \n", get_sp());
    fuc(argv);
    // if (argc > 1)
       // strcpy(l_arr, argv[1]);
}
