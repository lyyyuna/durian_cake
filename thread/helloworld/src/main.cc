#include <iostream>
#include <thread>
#include <cstdio>

void update(int &data)  //expects a reference to int
{
    data = 15;
    printf("hello\n");
}
int main()
{
    int data = 10;

    // This doesn't compile as the data value is copied when its reference is expected.
    //std::thread t1(update, data);         

    std::thread t1(update, std::ref(data));  // works
    auto t2 = std::move(t1);

    t2.join();

    printf("number of threads: %d\n", std::thread::hardware_concurrency());
    return 0;
}