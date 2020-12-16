#include <iostream>
#include <thread>
#include <cstdio>

void hello()
{
    printf("hello")
}

int main() {
    std::thread t(hello);
    t.join();
}