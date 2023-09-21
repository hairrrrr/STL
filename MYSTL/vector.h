//
// Created by 78172 on 2023/4/2.
//

#ifndef MEMORY_MANAGE_VECTOR_H
#define MEMORY_MANAGE_VECTOR_H

#include"iterator.h"
#include"allocator.h"
#include"uninitialized.h"
#include"type_traits.h"

namespace MYSTL
{

    template<typename Tp, typename Alloc>
    class _Vector_base
    {
    public:
        typedef Alloc allocator_type;
        allocator_type get_allocator() const { return allocator_type(); }

        _Vector_base(const Alloc& = allocator<Tp>())
            :_M_start(nullptr), _M_finish(nullptr), _M_end_of_storage(nullptr) {}

        _Vector_base(size_t n,  const Alloc& = allocator<Tp>())
                :_M_start(nullptr), _M_finish(nullptr), _M_end_of_storage(nullptr)
        {
            _M_start  = _M_allocate(n);
            _M_finish = _M_start;
            _M_end_of_storage = _M_start + n;
        }

        ~_Vector_base() { _M_deallocate(_M_start, _M_end_of_storage - _M_start); }


    protected:
        Tp* _M_start;
        Tp* _M_finish;
        Tp* _M_end_of_storage;

        typedef allocator<Tp, Alloc> _M_data_allocator;

        Tp*  _M_allocate(size_t n)  { return _M_data_allocator().allocate(n); }
        void _M_deallocate(Tp* ptr, size_t n) {  _M_data_allocator().deallocate(ptr, n); }
    };


    template<typename Tp, typename Alloc = allocator<Tp>>
    class vector : protected _Vector_base<Tp, Alloc>
    {
    private:
        typedef _Vector_base<Tp, Alloc> Base;
    public:
        typedef       Tp           value_type;
        typedef       value_type*  pointer;
        typedef const value_type*  const_pointer;
        typedef       value_type*  iterator;
        typedef const value_type*  const_iterator;
        typedef       value_type&  reference;
        typedef const value_type&  const_reference;
        typedef       size_t       size_type;
        typedef       ptrdiff_t    difference_type;

        typedef typename Base::allocator_type allocator_type;
        allocator_type get_allocator() { return Base::get_allocator(); }

        typedef reverse_iterator<const_iterator>  const_reverse_iterator;
        typedef reverse_iterator<iterator>        reverse_iterator;

    protected:
        using Base::_M_start;
        using Base::_M_finish;
        using Base::_M_end_of_storage;
        using Base::_M_allocate;
        using Base::_M_deallocate;

    public:

        /****************************
         * 迭代器
         */
        iterator       begin()       { return _M_start; }
        const_iterator begin() const { return _M_start; }
        iterator         end()       { return _M_finish; }
        const_iterator   end() const { return _M_finish; }

        reverse_iterator       rbegin()       { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator         rend()       { return reverse_iterator(begin()); }
        const_reverse_iterator   rend() const { return const_reverse_iterator(begin()); }


        /**********************
         * 小功能函数
         */
        size_type size()     const { return end() - begin(); }
        size_type max_size() const { return sizeof(-1) / sizeof(Tp); }
        size_type capacity() const { return size_type(_M_end_of_storage - begin()); }
        size_type empty()    const { return begin() == end(); }

        /*********************
         * 元素获取
         */
         reference       front()       { return *begin(); }
         const_reference front() const { return *begin(); }
         reference        back()       { return *(end() - 1); }
         const_reference  back() const { return *(end() - 1); }

         reference       operator[](size_type pos)       { return *(begin() + pos); }
         const_reference operator[](size_type pos) const { return *(begin() + pos); }


         /*********************
          *  Big Five
          */
         vector() = default;

         explicit vector(allocator_type& a) : Base(a) {}

        // 父类申请内存，子类调用构造
         vector(size_type n, const Tp& value = Tp())
            : Base(n)
         { _M_finish = uninitialized_fill_n(_M_start, n, value); }

         vector(const Tp* first, const Tp* last)
            : Base(last - first)
         { _M_finish = uninitialized_copy(first, last, _M_start); }


         template<typename InputIterator>
         vector(InputIterator first, InputIterator last)
         {
             typedef typename _Is_integer<InputIterator>::_Integral Integral;
             _M_initialize_aux(first, last, Integral());
         }



        vector(const vector& v)
            : Base(v.size())
        { _M_finish = uninitialized_copy(v.begin(), v.end(), _M_start); }

        vector(vector&& v)
        { this->swap(v); }


        vector& operator=(vector v)
        {
             this->swap(v);
             return *this;
        }

        // 子类调用析构，父类释放内存
        ~vector() { MYSTL::destroy(_M_start, _M_finish); }


        /********************
         * size，capacity 调整函数
         */

        void reserve(size_type n)
        {
            if(capacity() >= n) return;
            size_type sz = size();
            iterator tmp = _M_allocate_and_copy(n, _M_start, _M_finish);
            _M_destroy_and_deallocate(_M_start, _M_finish, capacity());
            _M_start  = tmp;
            _M_finish = _M_start + sz;
            _M_end_of_storage = _M_start + n;
        }

        void resize(size_type n, const Tp& val)
        {
            if(n < size())
                erase(begin() + n, end());
            else if(n > size())
            {
                reserve(n);
                size_type cnt = n - size();
                while(cnt--)
                    push_back(val);
            }
        }

        void clear() { erase(begin(), end()); }

        /*************************
         * 元素插入
         */
        void assign(size_type n, const Tp& val) { _M_fill_assign(n, val); }

        template<typename InputIterator>
        void assign(InputIterator first, InputIterator last)
        {
            typedef typename _Is_integer<InputIterator>::_Integral integral;
            _M_assign_dispatch(first, last, integral());
        }


        void push_back(const Tp& val)
        {
            if(_M_finish != _M_end_of_storage)
            {
                construct(_M_finish, val);
                ++_M_finish;
            }
            else
                _M_insert_aux(end(), val);
        }

        iterator insert(iterator pos, const Tp& val = Tp())
        {
            size_type n = pos - begin();
            if(pos == end())
                push_back(val);
            else
                _M_insert_aux(pos, val);
            return begin() + n;
        }

        /*************************
         * 元素删除
         */
        void pop_back()
        {
            destroy(--_M_finish);
        }

        iterator erase(iterator pos)
        {
            if(pos != end() - 1)
                copy(pos + 1, end(), pos);
            destroy(--_M_finish);
            return pos;
        }

        iterator erase(iterator first, iterator last)
        {
            iterator new_finish = std::copy(last, end(), first);
            destroy(new_finish, end());
            _M_finish = new_finish;
            return first;
        }


        void swap(vector& rhs)
        {
             std::swap(_M_start, rhs._M_start);
             std::swap(_M_finish, rhs._M_finish);
             std::swap(_M_end_of_storage, rhs._M_end_of_storage);
        }

    private:

        template<typename InputIterator>
        void _M_initialize_aux(InputIterator first, InputIterator last, __false_type)
        {
            _M_range_initialize(first, last, iterator_category(first));
        }

        template<typename Integer>
        void _M_initialize_aux(Integer n, Integer val, __true_type)
        {
            _M_start  = _M_allocate(n);
            _M_end_of_storage = _M_start + n;
            _M_finish = uninitialized_fill_n(_M_start, n, val);
        }

        // 此函数只被用于构造函数
        template<typename InputIterator>
        void _M_range_initialize(InputIterator first, InputIterator last,
                                 forward_iterator_tag)
        {
            size_type n = 0;
            distance(first, last, n);
            _M_start  = _M_allocate(n);
            _M_finish = uninitialized_copy(first, last, _M_start);
            _M_end_of_storage = _M_finish;
        }

        iterator _M_allocate_and_copy(size_type n, const_iterator first,
                                               const_iterator last)
        {
            iterator res = _M_allocate(n);
            try{
                std::uninitialized_copy(first, last, res);
                return res;
            } catch(...){
                _M_deallocate(res, n);
                return iterator();
            }
        }

        void _M_destroy_and_deallocate(iterator first, iterator last, size_type n)
        {
            destroy(first, last);
            _M_deallocate(first, n);
        }

        void _M_fill_assign(size_type n, const Tp& val);

        template<typename Integer>
        void _M_assign_dispatch(Integer n, Integer val, __true_type)
        { _M_fill_assign(n, val); }

        template<typename InputIterator>
        void _M_assign_dispatch(InputIterator first, InputIterator last, __false_type)
        { _M_assign_aux(first, last, iterator_category(first)); }

        template<typename InputIterator>
        void _M_assign_aux(InputIterator first, InputIterator last, input_iterator_tag);

        template<typename ForwardIterator>
        void _M_assign_aux(ForwardIterator first, ForwardIterator last, forward_iterator_tag);

        void _M_insert_aux(iterator pos, const Tp& val);
    };


    template<typename Tp, typename Alloc>
    void  vector<Tp, Alloc>::
    _M_fill_assign(size_type n, const Tp& val)
    {
        if( n > capacity() )
        {
            vector tmp(n, val);
            tmp.swap(*this);
        }
        else if( n > size() )
        {
            fill(begin(), end(), val);
            _M_finish = uninitialized_fill_n(end(), n - size(), val);
        }
        else
            erase(fill_n(begin(), n, val), end());
    }


    // 输入型迭代器只能遍历一遍，所以不能先用 distance 计算长度
    template<typename Tp, typename Alloc>
    template<typename InputIterator>
    void  vector<Tp, Alloc>::
    _M_assign_aux(InputIterator first, InputIterator last, input_iterator_tag)
    {
        iterator cur = begin();
        for( ; cur != end() && first != last; ++first, ++cur)
            *cur = *first;
        // size() > n
        if(first == last)
            erase(cur, end());
        // n > size()，可能引发扩容
        else if(cur == end())
            insert(end(), first, last);
    }

    template<typename Tp, typename Alloc>
    template<typename ForwardIterator>
    void  vector<Tp, Alloc>::
    _M_assign_aux(ForwardIterator first, ForwardIterator last, forward_iterator_tag)
    {
        size_type n = 0;
        distance(first, last, n);

        if(n > capacity())
        {
            vector tmp(first, last);
            tmp.swap(*this);
        }
        else if(n >= size())
        {
            ForwardIterator end = first;
            advance(end, n);
            copy(first, end, _M_start);
            uninitialized_copy(end, last, _M_finish);
        }
        else
        {
            iterator new_finish = copy(first, last, _M_start);
            destroy(new_finish, _M_finish);
            _M_finish = new_finish;
        }
    }


    /**************
     * push_back 调用, insert(position, x)调用
     * 所有增加 capacity 的调用最终到来到这里
     */
    template<typename Tp, typename Alloc>
    void  vector<Tp, Alloc>::
    _M_insert_aux(iterator pos, const Tp& val)
    {
        if(_M_finish != _M_end_of_storage)
        {
            construct(_M_finish, *(_M_finish - 1));
            ++_M_finish;
            std::copy_backward(pos, _M_finish - 2, _M_finish - 1);
            *pos = val;
        }
        else
        {
            size_type  old_size   = size();
            size_type  new_size   = old_size ? 2 * old_size : 1;
            iterator   new_start  = _M_allocate(new_size);
            iterator   new_finish = new_start;
            try {
                new_finish = std::uninitialized_copy(_M_start, pos, new_start);
                construct(new_finish, val);
                new_finish = std::uninitialized_copy(pos, _M_finish, ++new_finish);
            } catch(...){
                // 被这里接受的异常一定不是 uninitialized_copy 内部的。
                // 因为 uninitialized_copy 内会处理异常
                destroy(new_start, new_finish);
                _M_deallocate(new_start, new_size);
            }
            destroy(_M_start, _M_finish);
            _M_deallocate(_M_start, _M_end_of_storage - _M_start);
            _M_start          = new_start;
            _M_finish         = new_finish;
            _M_end_of_storage = new_start + new_size;
        }
    }

} // namespace MYSTL

#endif //MEMORY_MANAGE_VECTOR_H
