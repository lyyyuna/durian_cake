#include "StdAfx.h"

#pragma once
#ifndef __Y_VECTOR
#define __Y_VECTOR

template <class T>
class y_vector
{
public:
    // vector 的嵌套型别定义
    typedef T   value_type;
    typedef value_type* pointer;
    typedef value_type* iterator;
    typedef value_type& reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

protected:
    std::allocator<value_type> data_allocator;   // memory allocator
    iterator start;         // start memory start used
    iterator finish;        // end memory start used
    iterator end_of_storage; // end memory haven't been used

    void insert_aux(iterator position, const T& x);
    
    void deallocate()
    {
        if(start)
        {
            data_allocator.deallocate(start, end_of_storage-start);
        }
    }
    void destroy(iterator s, iterator e)
    {
        iterator tmp = s;
        while (s != e)
        {
            s ++;
            data_allocator.destroy(tmp);
            tmp = s;
        }
    }

    void fill_initialize(size_type n, const T& value)
    {
        start = allocate_and_fill(n, value);
        finish = start + n;
        end_of_storage = finish;
    }

public:
    iterator begin() { return start; }
    iterator end() { return finish; }
    size_type size() { return size_type(end() - begin()); }
    size_type capacity() { return size_type(end_of_storage - begin());  }
    bool  empty() const { return begin() == end(); }
    reference operator[](size_type n) {  return *(begin()+n); }

    /* ctor */
    y_vector() : start(0), finish(0), end_of_storage(0) {}
    y_vector(size_type n, const T& value) { fill_initialize(n, value); }
    y_vector(int n, const T& value) { fill_initialize(n, value); }
    y_vector(long n, const T& value) { fill_intialize(n, value); }
    explicit y_vector(size_type n) {fill_initialize(n, T()); }

    /* dtor */
    ~y_vector()
    {
        destroy(start, finish);
        deallocate();
    }

    /*  */
    reference front() { return *begin(); }
    reference back() { return *(end()-1); }
    
    void push_back(const T& x)
    {
        if ( finish != end_of_storage )
        {
            data_allocator.construct(finish, x);
            ++finish;
        }
        else
            insert_aux(end(), x);
    }

    void pop_back()
    {
        --finish;
        data_allocator.destroy(finish);
    }

    /* erase */
    iterator erase(iterator first, iterator last)
    {
        iterator i = copy(last, finish, first);
        destroy(i, finish);
        finish = finish - (last - first);
        return first;
    }
    iterator erase(iterator position)
    {
        if (position + 1 != end())
            std::copy(posistion+1, finish, position);
        --finish;
        data_allocator.destroy(finish);
        return position;
    }

    void resize(size_type new_size, const T& x)
    {
        if (new_size < size())
            erase(begin() + new_size, end());
        else
            insert(end(), new_size-size(), x);
    }
    void resize(size_type new_size)
    {
        resize(new_size, T());
    }
    void clear()
    {
        erase(begin(), end());
    }

    void insert(iterator position, size_type n, const T & x);


protected:
    iterator allocate_and_fill(size_type n, const T& x)
    {
        iterator result = data_allocator.allocate(n);
        std::uninitialized_fill_n(result, n, x);
        return result;
    }
};


template <class T>
void y_vector<T>::insert_aux(iterator position, const T&x)
{
    if (finish != end_of_storage)
    {
        data_allocator.construct(finish, *(finish-1));
        ++finish;
        T x_copy = x;
        std::copy_backward(position, finish-2, stdext::make_checked_array_iterator(finish-1, finish-2-position));
        *position = x_copy;
    }
    else
    {
        const size_type old_size = size();
        const size_type len = old_size != 0 ? 2 * old_size : 1;

        iterator new_start = data_allocator.allocate(len);
        iterator new_finish = new_start;
        
        new_finish = std::uninitialized_copy(start, position, new_start);
        data_allocator.construct(new_finish, x); 
        ++new_finish;
        new_finish = std::uninitialized_copy(position, finish, new_finish);
        
        // destroy original vector
        destroy(begin(), end());
        deallocate();

        // ajust the iterator
        start = new_start;
        finish = new_finish;
        end_of_storage = new_start + len;
    }
}

template <class T>
void y_vector<T>::insert(iterator position, size_type n, const T & x)
{
    if (n != 0)
    {
        // 备用空间大于新增元素个数
        if (size_type(end_of_storage - finish) >= n)
        {
            T x_copy = x;
            // 计算插入点之后的元素个数
            const size_type elems_after = finish - position;
            iterator old_finish = finish;
            // 插之后的元素个数 大于 新增元素个数
            if (elems_after > n)
            {
                std::uninitialized_copy(finish-n, finish, finish);
                finish += n;
                std::copy_backward(position, old_finish-n, old_finish);
                std::fill(position, position+n, x_copy);
            }
            // 插入之后的现有元素个数 小于 新增元素个数
            else
            {
                std::uninitialized_fill_n(finish, n-elems_after, x_copy);
                finish += n-elems_after;
                std::uninitialized_copy(position, old_finish, finish);
                finish += elems_after;
                std::fill(position, old_finish, x_copy);
            }
        }
        // 备用空间不够
        else
        {
            // 新长度为2倍
            const size_type old_size = size();
            const size_type len = old_size + max(old_size, n);
            // 配置新的空间
            iterator new_start = data_allocator.allocate(len);
            iterator new_finish = new_start;
            // __TRY
            // 先将旧vector 插入点之前的数据搬移到 新 vector 中
            new_finish = std::uninitialized_copy(start, position, new_start);
            // 将插入数据放到新空间中
            std::uninitialized_fill_n(new_finish, n, x);
            new_finish += n;
            new_finish = std::uninitialized_copy(position, finish, new_finish);

            // 释放旧的 vector
            destroy(start, finish);
            deallocate();

            // 调整标记
            start = new_start;
            finish = new_finish;
            end_of_storage = new_start + len;
        }
    }
}

#endif