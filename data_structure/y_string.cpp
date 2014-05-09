#include <iostream>
#include <cstring>
#include <vector>

class y_string
{
public:
    y_string() : _data(new char[1])
    {
        *_data = '\0';
    }

    y_string(const char * str) : _data(new char[strlen(str)+1])
    {
        strcpy(_data, str);
    }

    y_string(const y_string & rhs) : _data(new char[rhs.size()+1])
    {
        strcpy(_data, rhs.c_str());
    }

    ~y_string()
    {
        delete [] _data;
    }

    // ½£Ö¸
    y_string & operator = (y_string rhs)
    {
        swap(rhs);
        return *this;
    }

    // c++11
    y_string(y_string && rhs) : _data(rhs._data)
    {
        rhs._data = nullptr;
    }
    // c++11
    y_string & operator= (y_string && rhs)
    {
        swap(rhs);
        return *this;
    }

    // accessors
    size_t size() const
    {
        return strlen(_data);
    }

    const char * c_str() const
    {
        return _data;
    }

    void swap(y_string & rhs)
    {
        std::swap(_data, rhs._data);
    }


private:
    char * _data;

};



// test
void foo(y_string x)
{

}

void bar(const y_string & x)
{

}

y_string baz()
{
    y_string ret("world");
    return ret;
}

int main()
{
    y_string s0;
    y_string s1("hello");
    y_string s2(s0);
    y_string s3 = s1;
    s2 = s1;

    foo(s1);
    bar(s2);
    foo("tem");
    bar("tem");
    y_string s4 = baz();

    std::vector<y_string> svec;
    svec.push_back(s0);
    svec.push_back(s1);
    svec.push_back(baz());
}
