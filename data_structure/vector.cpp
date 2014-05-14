#include <iostream>
#include <cstring>

using namespace std;

class vector_int
{
public:
    typedef int value_type;
    typedef value_type * iterator;
    typedef value_type * pointer;
    typedef value_type & reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

private:
    iterator start;
    iterator finish;
    iterator end_of_storage;

public:
    iterator begin() { return start; }
    iterator end() { return finish; }
    size_type size() { return size_type(end() - begin()); }
    size_type capacity() {return size_type(end_of_storage - begin());}
    bool empty() { return begin() == end(); }
    reference operator[] (size_type n) { return *(begin()+n); }

    // ctor
    vector_int(): start(0), finish(0), end_of_storage(0) {}
    vector_int(size_type n, const value_type & value ) { fill_initialize(n, value); }
    // vector_int()

    // dtor
    ~vector_int()
    {
        delete [] start;
    }

private:
    iterator allocate_and_fill(size_type n, const value_type & x)
    {
        iterator result = new value_type[n];
        for (size_type i = 0; i < n; ++i)
            result[i] = x;
        return result;
    }

    void fill_initialize(size_type n, const value_type & x)
    {
        start = allocate_and_fill(n, x);
        finish = start + n;
        end_of_storage = finish;
    }

    void insert_aux(const value_type & x)
    {
        const size_type old_size = size();
        const size_type len = old_size!=0 ? 2*old_size : 1;

        iterator new_start = new value_type[len];
        iterator new_finish = new_start;

        memcpy(new_start, start, old_size);
        new_finish += old_size;
        *new_finish = x;     // 复制新元素
        ++new_finish;

        // 销毁原数据区域
        delete [] start;

        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }

public:
    reference front() { return *begin(); }
    reference back() { return *(end()-1); }

    void push_back(const value_type & x)
    {
        if (finish != end_of_storage)
        {
            *finish = x;
            ++finish;
        }
        else
        {
            insert_aux(x);
        }
    }

    void pop_back()
    {
        --finish;
    }
};

int main()
{
    vector_int vec(2, 3);
    cout << vec[0] << endl;
    cout << "size: " << vec.size() << endl;
    cout << "capacity: " << vec.capacity() << endl;
    vec.push_back(5);
    cout << vec[2] << endl;
    cout << "size: " << vec.size() << endl;
    cout << "capacity: " << vec.capacity() << endl;
    vec.pop_back();
    cout << "size: " << vec.size() << endl;
    cout << "capacity: " << vec.capacity() << endl;
}
