//
// Created by 78172 on 2023/4/3.
//

#ifndef MEMORY_MANAGE_DEQUE_H
#define MEMORY_MANAGE_DEQUE_H

#include "iterator.h"
#include "allocator.h"
#include "algo.h"

namespace MYSTL
{

    inline size_t __deque_buf_size(size_t size)
    {
        return size < 512 ? 512 / size : size_t(1);
    }

    template<typename Tp, typename Ref, typename Ptr>
    struct _Deque_iterator
    {
        typedef _Deque_iterator<Tp, Tp&, Tp*> iterator;
        typedef _Deque_iterator<Tp, const Tp&, const Tp*> const_iterator;

        static size_t _S_buffer_size() { return __deque_buf_size(sizeof(Tp)); }

        typedef random_access_iterator_tag iterator_category;
        typedef Tp                         value_type;
        typedef Ref                        reference;
        typedef Ptr                        pointer;
        typedef ptrdiff_t                  difference_type;
        typedef size_t                     size_type;
        typedef Tp**                       Map_pointer;

        typedef _Deque_iterator            Self;

        Tp*         _M_cur;   // 迭代器指向缓冲区的当前元素
        Tp*         _M_first; // 迭代器指向缓冲区的头部
        Tp*         _M_last;  // 迭代器指向缓冲区的尾部
        Map_pointer _M_node;  // 迭代器指向 map 的 node

        _Deque_iterator() : _M_cur(nullptr), _M_first(nullptr)
                          ,_M_last(nullptr), _M_node(nullptr) {}

        _Deque_iterator(const Tp* x, const Map_pointer y)
            : _M_cur(x), _M_first(*y)
            ,_M_last(*y + _S_buffer_size()), _M_node(y) {}

        _Deque_iterator(const _Deque_iterator& x)
            : _M_cur(x._M_cur)  , _M_first(x._M_first)
            , _M_last(x._M_last), _M_node(x._M_node) {}

        reference operator*() const  { return *_M_cur; }
        pointer   operator->() const { return &(operator*()); }

        difference_type operator-(const Self& x)
        {
            return (_M_node - x._M_node - 1) * difference_type(_S_buffer_size()) +
                   (_M_cur - _M_first) + (x._M_last - x._M_cur);
        }

        Self& operator++()
        {
            ++_M_cur;
            // 如果已到达所在缓冲区的尾端
            if(_M_cur == _M_last)
            {
                // 就切换到下一节点(下一个缓冲区)
                _M_set_node(_M_node + 1);
                // 指向下一个缓冲区的第一个元素
                _M_cur = _M_first;
            }
            return *this;
        }

        Self operator++(int)
        {
            Self tmp(*this);
            ++(*this);
            return tmp;
        }

        Self& operator--()
        {
            if(_M_cur == _M_first)
            {
                _M_set_node(_M_node - 1);
                _M_cur = _M_first;
            }
            --_M_cur;
            return *this;
        }

        Self operator--(int)
        {
            Self tmp(*this);
            --(*this);
            return tmp;
        }

        Self& operator+=(difference_type n)
        {
            difference_type offset = _M_cur - _M_first + n;
            // 目标位置在同一缓冲区内
            if( offset >= 0 && offset < _S_buffer_size() )
                _M_cur += offset;
            else
            {
                difference_type node_offset =  offset >= 0 ?
                        offset / difference_type(_S_buffer_size()) :
                        -difference_type((-offset - 1) / _S_buffer_size()) - 1;
                // 切换到正确的节点(缓冲区)
                _M_set_node(_M_node + node_offset);
                // 切换到正确的元素
                _M_cur = _M_first + ( offset - node_offset * difference_type(_S_buffer_size()) );
            }
            return *this;
        }

        Self& operator-=(difference_type n) { return *this += -n; }

        Self operator+(difference_type n)
        {
            Self tmp(*this);
            return tmp += n;
        }


        Self operator-(difference_type n)
        {
            Self tmp(*this);
            return tmp -= n;
        }

        reference operator[](difference_type n) { return *(*this + n); }

        bool operator==(const Self& rhs) const { return _M_cur == rhs._M_cur; }
        bool operator!=(const Self& rhs) const { return !(*this == rhs); }

        bool operator<(const Self& rhs) const
        {
            return _M_node == rhs._M_node ?
                   _M_cur < rhs._M_cur :
                   _M_node < rhs._M_node;
        }
        bool operator>(const Self& rhs) const { return rhs < *this; }

        bool operator<=(const Self& rhs) const { return !(rhs < *this); }
        bool operator>=(const Self& rhs) const { return !(*this < rhs); }

        //  将 _M_node 移动到下一个缓冲区
        void _M_set_node(Map_pointer new_node)
        {
            _M_node  = new_node;
            _M_first = *new_node;
            _M_last  = _M_first + _S_buffer_size();
        }
    };

    template<typename Tp, typename Alloc>
    class _Deque_base
    {
    public:
        typedef _Deque_iterator<Tp, Tp&, Tp*> iterator;
        typedef _Deque_iterator<Tp, const Tp&, const Tp*> const_iterator;

        typedef Alloc allocator_type;
        allocator_type get_allocator() const { return allocator_type(); }

        _Deque_base() : _M_map(nullptr), _M_map_size(0),
                        _M_start(), _M_finish() {}

        _Deque_base(size_t num_element) : _M_map(nullptr), _M_map_size(0),
                                          _M_start(), _M_finish()
        { _M_initialize_map(num_element); }

        ~_Deque_base()
        {
            // 注意 nfinish 需要 +1，因为是前闭后开区间，而 deque 的 end 迭代器并不是真的开区间
            _M_destroy_nodes(_M_start._M_node, _M_finish._M_node + 1);
            _M_deallocate_map(_M_map, _M_map_size);
        }

    protected:
        void _M_initialize_map(size_t);
        void _M_create_nodes (Tp** nstart, Tp** nfinish);
        void _M_destroy_nodes(Tp** nstart, Tp** nfinish);
        enum { _S_initialize_map_size = 8 };

    protected:
        Tp** _M_map;
        size_t _M_map_size;
        iterator _M_start;
        iterator _M_finish;

        typedef MYSTL::allocator<Tp*, Alloc> map_alloc;
        typedef MYSTL::allocator<Tp , Alloc> node_alloc;

        Tp* _M_allocate_node()
        { return node_alloc().allocate(__deque_buf_size(sizeof(Tp))); }
        void _M_deallocate_node(Tp* ptr)
        { return node_alloc().deallocate(ptr, __deque_buf_size(sizeof(Tp))); }

        Tp** _M_allocate_map(size_t n)
        { return map_alloc().allocate(n); }
        void _M_deallocate_map(Tp** ptr, size_t n)
        { return map_alloc().deallocate(ptr, n); }
    };

    template<typename Tp, typename Alloc>
    void  _Deque_base<Tp, Alloc>::
    _M_initialize_map(size_t num_elements)
    {
        // 需要 buffer 数量
        size_t num_nodes = num_elements / __deque_buf_size(sizeof(Tp)) + 1;
        // map 的最小大小为 8，最大为所需 buffer + 2
        _M_map_size = max(num_nodes + 2, _S_initialize_map_size);
        _M_map = _M_allocate_map(_M_map_size);

        // nstart 指向 _M_map 中间位置
        Tp** nstart  = _M_map + (_M_map_size - num_nodes) / 2;
        Tp** nfinish = nstart + num_nodes;

        try {
            _M_create_nodes(nstart, nfinish);
        } catch(...){
            _M_deallocate_map(_M_map, _M_map_size);
            _M_map = nullptr;
            _M_map_size = 0;
        }

        _M_start._M_set_node(nstart);
        _M_finish._M_set_node(nfinish);
        _M_start._M_cur  = _M_start._M_first;
        _M_finish._M_cur = _M_finish._M_first +
                num_nodes % __deque_buf_size(sizeof(Tp));
    }

    template<typename Tp, typename Alloc>
    void _Deque_base<Tp, Alloc>::
    _M_create_nodes (Tp** nstart, Tp** nfinish)
    {
        Tp** cur;
        try {
            for(cur = nstart; cur != nfinish; ++cur)
                *cur = _M_allocate_node();
        } catch(...) {
            _M_destroy_nodes(nstart, cur);
        }
    }

    template<typename Tp, typename Alloc>
    void _Deque_base<Tp, Alloc>::
    _M_destroy_nodes(Tp** nstart, Tp** nfinish)
    {
        for(Tp** i = nstart; i != nfinish; ++i)
            _M_deallocate_node(*i);
    }


    template<typename Tp, typename Alloc = std::allocator<Tp>>
    class deque : protected _Deque_base<Tp, Alloc>
    {
        typedef _Deque_base<Tp, Alloc> _Base;

    public:                         // Basic types
        typedef     Tp                  value_type;
        typedef     value_type*         pointer;
        typedef     const value_type*   const_pointer;
        typedef     value_type&         reference;
        typedef     const value_type&   const_reference;
        typedef     size_t              size_type;
        typedef     ptrdiff_t           difference_type;

        typedef typename _Base::allocator_type allocator_type;
        allocator_type get_allocator() const { return _Base::get_allocator(); }

    public:
        // Iterators
        typedef typename _Base::iterator       iterator;
        typedef typename _Base::const_iterator const_iterator;

        typedef reverse_iterator<const_iterator> const_reverse_iterator;
        typedef reverse_iterator<iterator> reverse_iterator;

    protected:
        // Internal typedefs
        typedef pointer* Map_pointer;
        static size_t _S_buffer_size() { return __deque_buf_size(sizeof(Tp)); }

    protected:
        using _Base::_M_initialize_map;
        using _Base::_M_create_nodes;
        using _Base::_M_destroy_nodes;
        using _Base::_M_allocate_node;
        using _Base::_M_deallocate_node;
        using _Base::_M_allocate_map;
        using _Base::_M_deallocate_map;

        using _Base::_M_map;
        using _Base::_M_map_size;
        using _Base::_M_start;
        using _Base::_M_finish;

    public:
        // Basic accessors
        iterator begin() { return _M_start; }
        iterator   end() { return _M_finish; }
        const_iterator begin() const { return _M_start; }
        const_iterator   end() const { return _M_finish; }

        reverse_iterator rbegin() { return reverse_iterator(_M_finish); }
        reverse_iterator   rend() { return reverse_iterator(_M_start); }
        const_reverse_iterator rbegin() const
        { return const_reverse_iterator(_M_finish); }
        const_reverse_iterator rend() const
        { return const_reverse_iterator(_M_start); }

        reference operator[](size_type __n)
        { return _M_start[difference_type(__n)]; }
        const_reference operator[](size_type __n) const
        { return _M_start[difference_type(__n)]; }

        void _M_range_check(size_type __n) const {
            if (__n >= this->size())
                std::__throw_range_error("deque");
        }
        // at 实现
        reference at(size_type __n)
        { _M_range_check(__n); return (*this)[__n]; }
        const_reference at(size_type __n) const
        { _M_range_check(__n); return (*this)[__n]; }

        reference front() { return *_M_start; }
        reference  back() {
            iterator __tmp = _M_finish;
            --__tmp;
            return *__tmp;
        }

        const_reference front() const { return *_M_start; }
        const_reference  back() const {
            const_iterator __tmp = _M_finish;
            --__tmp;
            return *__tmp;
        }

        size_type size() const { return _M_finish - _M_start; }
        size_type max_size() const { return size_type(-1); }
        bool empty() const { return _M_finish == _M_start; }

    public:
        deque() = default;

        deque(const deque& __x) : _Base(__x.size())
        { uninitialized_copy(__x.begin(), __x.end(), _M_start); }

        deque(size_type __n, const value_type& __value = value_type()) : _Base(__n)
        { _M_fill_initialize(__value); }

        template <class _InputIterator>
        deque(_InputIterator __first, _InputIterator __last)
        {
            typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
            _M_initialize_dispatch(__first, __last, _Integral());
        }

        ~deque() { destroy(_M_start, _M_finish); }

        deque& operator= (const deque& __x) {
            const size_type __len = size();
            if (&__x != this) {
                if (__len >= __x.size())
                    erase(copy(__x.begin(), __x.end(), _M_start), _M_finish);
                else {
                    const_iterator __mid = __x.begin() + difference_type(__len);
                    copy(__x.begin(), __mid, _M_start);
                    insert(_M_finish, __mid, __x.end());
                }
            }
            return *this;
        }

        void swap(deque& __x) {
            std::swap(_M_start, __x._M_start);
            std::swap(_M_finish, __x._M_finish);
            std::swap(_M_map, __x._M_map);
            std::swap(_M_map_size, __x._M_map_size);
        }

        /***********************
         * 插入元素
         */
    public:
        // push_back
        void push_back(const value_type& __t = value_type()) {
            if (_M_finish._M_cur != _M_finish._M_last - 1) {
                construct(_M_finish._M_cur, __t);
                ++_M_finish._M_cur;
            }
            else
                _M_push_back_aux(__t);
        }
        // push_front
        void push_front(const value_type& __t = value_type()) {
            if (_M_start._M_cur != _M_start._M_first) {
                construct(_M_start._M_cur - 1, __t);
                --_M_start._M_cur;
            }
            else
                _M_push_front_aux(__t);
        }

    public:
        iterator insert(iterator position, const value_type& __x = value_type()) {
            if (position._M_cur == _M_start._M_cur) {
                push_front(__x);
                return _M_start;
            }
            else if (position._M_cur == _M_finish._M_cur) {
                push_back(__x);
                iterator __tmp = _M_finish;
                --__tmp;
                return __tmp;
            }
            else {
                return _M_insert_aux(position, __x);
            }
        }

        void insert(iterator __pos, size_type __n, const value_type& __x)
        { _M_fill_insert(__pos, __n, __x); }

        template <class _InputIterator>
        void insert(iterator __pos, _InputIterator __first, _InputIterator __last)
        {
            typedef typename _Is_integer<_InputIterator>::_Integral _Integral;
            _M_insert_dispatch(__pos, __first, __last, _Integral());
        }

        template <class _InputIterator>
        void insert(iterator __pos, _InputIterator __first, _InputIterator __last,
                    input_iterator_tag);

        template <class _ForwardIterator>
        void insert(iterator __pos,
                    _ForwardIterator __first, _ForwardIterator __last,
                    forward_iterator_tag);


        /*************
         * 删除元素
         */
    public:
        // pop_back
        void pop_back() {
            if (_M_finish._M_cur != _M_finish._M_first) {
                --_M_finish._M_cur;
                destroy(_M_finish._M_cur);
            }
            else
                _M_pop_back_aux();
        }

        // pop_front
        void pop_front() {
            if (_M_start._M_cur != _M_start._M_last - 1) {
                destroy(_M_start._M_cur);
                ++_M_start._M_cur;
            }
            else
                _M_pop_front_aux();
        }

        // erase(pos)
        iterator erase(iterator __pos) {
            iterator __next = __pos;
            ++__next;
            difference_type __index = __pos - _M_start;
            if (size_type(__index) < (this->size() >> 1)) {
                copy_backward(_M_start, __pos, __next);
                pop_front();
            }
            else {
                copy(__next, _M_finish, __pos);
                pop_back();
            }
            return _M_start + __index;
        }

        iterator erase(iterator __first, iterator __last);
        void clear();

        /**************************
         * 初始化相关的函数
         */
    protected:
        template <class _Integer>
        void _M_initialize_dispatch(_Integer __n, _Integer __x, __true_type) {
            _M_initialize_map(__n);
            _M_fill_initialize(__x);
        }

        template <class _InputIter>
        void _M_initialize_dispatch(_InputIter __first, _InputIter __last,
                                    __false_type) {
            _M_range_initialize(__first, __last, __ITERATOR_CATEGORY(__first));
        }

        void _M_fill_initialize(const value_type& __value);

        template <class _InputIterator>
        void _M_range_initialize(_InputIterator __first, _InputIterator __last,
                                 input_iterator_tag);

        template <class _ForwardIterator>
        void _M_range_initialize(_ForwardIterator __first, _ForwardIterator __last,
                                 forward_iterator_tag);

        void _M_fill_insert(iterator __pos, size_type __n, const value_type& __x);


        // Internal push_* and pop_*
    protected:
        void _M_push_back_aux(const value_type&);
        void _M_push_front_aux(const value_type&);
        void _M_pop_back_aux();
        void _M_pop_front_aux();


        /************************
        * 插入相关函数
        */
    protected:
        template <class _Integer>
        void _M_insert_dispatch(iterator __pos, _Integer __n, _Integer __x,
                                __true_type) {
            _M_fill_insert(__pos, (size_type) __n, (value_type) __x);
        }

        template <class _InputIterator>
        void _M_insert_dispatch(iterator __pos,
                                _InputIterator __first, _InputIterator __last,
                                __false_type) {
            insert(__pos, __first, __last, __ITERATOR_CATEGORY(__first));
        }

        iterator _M_insert_aux(iterator __pos, const value_type& __x = value_type());

        void _M_insert_aux(iterator __pos, size_type __n,
                           const value_type& __x);

        template <class _ForwardIterator>
        void _M_insert_aux(iterator __pos, _ForwardIterator __first,
                           _ForwardIterator __last, size_type __n);

    protected:
        iterator _M_reserve_elements_at_front(size_type __n) {
            size_type __vacancies = _M_start._M_cur - _M_start._M_first;
            if (__n > __vacancies)
                _M_new_elements_at_front(__n - __vacancies);
            return _M_start - difference_type(__n);
        }

        iterator _M_reserve_elements_at_back(size_type __n) {
            size_type __vacancies = (_M_finish._M_last - _M_finish._M_cur) - 1;
            if (__n > __vacancies)
                _M_new_elements_at_back(__n - __vacancies);
            return _M_finish + difference_type(__n);
        }

        void _M_new_elements_at_front(size_type __new_elements);
        void _M_new_elements_at_back(size_type __new_elements);

    protected:                      // Allocation of _M_map and nodes

        // Makes sure the _M_map has space for new nodes.  Does not actually
        //  add the nodes.  Can invalidate _M_map pointers.  (And consequently,
        //  deque iterators.)

        void _M_reserve_map_at_back (size_type __nodes_to_add = 1)
        {
            if (__nodes_to_add + 1 > _M_map_size - (_M_finish._M_node - _M_map))
                _M_reallocate_map(__nodes_to_add, false);
        }

        void _M_reserve_map_at_front (size_type __nodes_to_add = 1)
        {
            if (__nodes_to_add > size_type(_M_start._M_node - _M_map))
                _M_reallocate_map(__nodes_to_add, true);
        }

        void _M_reallocate_map(size_type __nodes_to_add, bool __add_at_front);
    };

    template <class Tp, class Alloc>
    void deque<Tp,Alloc>::_M_fill_initialize(const value_type& __value) {
        Map_pointer __cur;
        try {
            for (__cur = _M_start._M_node; __cur < _M_finish._M_node; ++__cur)
                uninitialized_fill(*__cur, *__cur + _S_buffer_size(), __value);
            uninitialized_fill(_M_finish._M_first, _M_finish._M_cur, __value);
        } catch (...) {
            destroy(_M_start, iterator(*__cur, __cur));
        }
    }

    template <class _Tp, class _Alloc> template <class _InputIterator>
    void deque<_Tp,_Alloc>::_M_range_initialize(_InputIterator __first,
                                                _InputIterator __last,
                                                input_iterator_tag)
    {
        _M_initialize_map(0);
        try {
                for ( ; __first != __last; ++__first)
                push_back(*__first);
        }
        catch(...) {
            clear();
        };
    }

    template <class _Tp, class _Alloc> template <class _ForwardIterator>
    void deque<_Tp,_Alloc>::_M_range_initialize(_ForwardIterator __first,
                                                _ForwardIterator __last,
                                                forward_iterator_tag)
    {
        size_type __n = 0;
        distance(__first, __last, __n);
        _M_initialize_map(__n);

        Map_pointer __cur_node;
        try {
                for (__cur_node = _M_start._M_node;
                __cur_node < _M_finish._M_node; ++__cur_node)
                {
                    _ForwardIterator __mid = __first;
                    advance(__mid, _S_buffer_size());
                    uninitialized_copy(__first, __mid, *__cur_node);
                    __first = __mid;
                }
                uninitialized_copy(__first, __last, _M_finish._M_first);
        }
        catch (...) {
            destroy(_M_start, iterator(*__cur_node, __cur_node));
        }
    }


    // Called only if _M_finish._M_cur == _M_finish._M_last - 1.
    template <class _Tp, class _Alloc>
    void deque<_Tp,_Alloc>::_M_push_back_aux(const value_type& __t)
    {
        value_type __t_copy = __t;
        _M_reserve_map_at_back();
        *(_M_finish._M_node + 1) = _M_allocate_node();
        try {
                construct(_M_finish._M_cur, __t_copy);
                _M_finish._M_set_node(_M_finish._M_node + 1);
                _M_finish._M_cur = _M_finish._M_first;
        }
        catch (...) {
            _M_deallocate_node(*(_M_finish._M_node + 1));
        }
    }

// Called only if _M_start._M_cur == _M_start._M_first.
    template <class _Tp, class _Alloc>
    void  deque<_Tp,_Alloc>::_M_push_front_aux(const value_type& __t)
    {
        value_type __t_copy = __t;
        _M_reserve_map_at_front();
        *(_M_start._M_node - 1) = _M_allocate_node();
        try {
                _M_start._M_set_node(_M_start._M_node - 1);
                _M_start._M_cur = _M_start._M_last - 1;
                construct(_M_start._M_cur, __t_copy);
        }
        catch(...) {
            ++_M_start;
            _M_deallocate_node(*(_M_start._M_node - 1));
        }
    }



    template <class _Tp, class _Alloc> template <class _InputIterator>
    void deque<_Tp,_Alloc>::insert(iterator __pos,
                                   _InputIterator __first, _InputIterator __last,
                                   input_iterator_tag)
    {
        copy(__first, __last, inserter(*this, __pos));
    }

    template <class _Tp, class _Alloc> template <class _ForwardIterator>
    void
    deque<_Tp,_Alloc>::insert(iterator __pos,
                              _ForwardIterator __first, _ForwardIterator __last,
                              forward_iterator_tag) {
        size_type __n = 0;
        distance(__first, __last, __n);
        if (__pos._M_cur == _M_start._M_cur)
        {
            iterator __new_start = _M_reserve_elements_at_front(__n);
            try {
                    uninitialized_copy(__first, __last, __new_start);
                    _M_start = __new_start;
            } catch(...) {
                _M_destroy_nodes(__new_start._M_node, _M_start._M_node);
            }
        }
        else if (__pos._M_cur == _M_finish._M_cur) {
            iterator __new_finish = _M_reserve_elements_at_back(__n);
            try {
                uninitialized_copy(__first, __last, _M_finish);
                _M_finish = __new_finish;
            } catch (...) {
                (_M_destroy_nodes(_M_finish._M_node + 1,
                                  __new_finish._M_node + 1));
            }
        }
        else
            _M_insert_aux(__pos, __first, __last, __n);
    }

    template <class _Tp, class _Alloc>
    void deque<_Tp, _Alloc>::_M_fill_insert(iterator __pos,
                                            size_type __n, const value_type& __x)
    {
        if (__pos._M_cur == _M_start._M_cur) {
            iterator __new_start = _M_reserve_elements_at_front(__n);
            try {
                    uninitialized_fill(__new_start, _M_start, __x);
                    _M_start = __new_start;
            } catch(...) {
                _M_destroy_nodes(__new_start._M_node, _M_start._M_node);
            }
        }
        else if (__pos._M_cur == _M_finish._M_cur) {
            iterator __new_finish = _M_reserve_elements_at_back(__n);
            try {
                    uninitialized_fill(_M_finish, __new_finish, __x);
                    _M_finish = __new_finish;
            } catch(...) {
                _M_destroy_nodes(_M_finish._M_node + 1,
                                          __new_finish._M_node + 1);
            }
        }
        else
            _M_insert_aux(__pos, __n, __x);
    }

    template <class _Tp, class _Alloc>
    typename deque<_Tp, _Alloc>::iterator
    deque<_Tp,_Alloc>::_M_insert_aux(iterator __pos, const value_type& __x)
    {
        difference_type __index = __pos - _M_start;
        value_type __x_copy = __x;
        // pos 位于偏靠前的位置
        if (size_type(__index) < this->size() / 2) {
            push_front(front());
            iterator __front1 = _M_start;
            ++__front1;
            iterator __front2 = __front1;
            ++__front2;
            __pos = _M_start + __index;
            iterator __pos1 = __pos;
            ++__pos1;
            copy(__front2, __pos1, __front1);
        }
        // pos 位于偏 靠后的位置
        else {
            push_back(back());
            iterator __back1 = _M_finish;
            --__back1;
            iterator __back2 = __back1;
            --__back2;
            __pos = _M_start + __index;
            copy_backward(__pos, __back2, __back1);
        }
        *__pos = __x_copy;
        return __pos;
    }


    template <class _Tp, class _Alloc>
    void deque<_Tp,_Alloc>::_M_insert_aux(iterator __pos,
                                          size_type __n,
                                          const value_type& __x)
    {
        const difference_type __elems_before = __pos - _M_start;
        size_type __length = this->size();
        value_type __x_copy = __x;
        if (__elems_before < difference_type(__length / 2)) {
            iterator __new_start = _M_reserve_elements_at_front(__n);
            iterator __old_start = _M_start;
            __pos = _M_start + __elems_before;
            try {
                    if (__elems_before >= difference_type(__n)) {
                        iterator __start_n = _M_start + difference_type(__n);
                        uninitialized_copy(_M_start, __start_n, __new_start);
                        _M_start = __new_start;
                        copy(__start_n, __pos, __old_start);
                        fill(__pos - difference_type(__n), __pos, __x_copy);
                    }
                    else {
                        __uninitialized_copy_fill(_M_start, __pos, __new_start,
                                                  _M_start, __x_copy);
                        _M_start = __new_start;
                        fill(__old_start, __pos, __x_copy);
                    }
            }
            catch (...) {
                    _M_destroy_nodes(__new_start._M_node, _M_start._M_node);
            }

        }
        else {
            iterator __new_finish = _M_reserve_elements_at_back(__n);
            iterator __old_finish = _M_finish;
            const difference_type __elems_after =
                    difference_type(__length) - __elems_before;
            __pos = _M_finish - __elems_after;
            try {
                    if (__elems_after > difference_type(__n)) {
                        iterator __finish_n = _M_finish - difference_type(__n);
                        uninitialized_copy(__finish_n, _M_finish, _M_finish);
                        _M_finish = __new_finish;
                        copy_backward(__pos, __finish_n, __old_finish);
                        fill(__pos, __pos + difference_type(__n), __x_copy);
                    }
                    else {
                        __uninitialized_fill_copy(_M_finish, __pos + difference_type(__n),
                                                  __x_copy, __pos, _M_finish);
                        _M_finish = __new_finish;
                        fill(__pos, __old_finish, __x_copy);
                    }
            }
            catch (...) {
                (_M_destroy_nodes(_M_finish._M_node + 1,
                                  __new_finish._M_node + 1));
            }
        }
    }

    template <class _Tp, class _Alloc> template <class _ForwardIterator>
    void deque<_Tp,_Alloc>::_M_insert_aux(iterator __pos,
                                          _ForwardIterator __first,
                                          _ForwardIterator __last,
                                          size_type __n)
    {
        const difference_type __elemsbefore = __pos - _M_start;
        size_type __length = size();
        if (__elemsbefore < __length / 2) {
            iterator __new_start = _M_reserve_elements_at_front(__n);
            iterator __old_start = _M_start;
            __pos = _M_start + __elemsbefore;
            try {
                    if (__elemsbefore >= difference_type(__n)) {
                        iterator __start_n = _M_start + difference_type(__n);
                        uninitialized_copy(_M_start, __start_n, __new_start);
                        _M_start = __new_start;
                        copy(__start_n, __pos, __old_start);
                        copy(__first, __last, __pos - difference_type(__n));
                    }
                    else {
                        _ForwardIterator __mid = __first;
                        advance(__mid, difference_type(__n) - __elemsbefore);
                        __uninitialized_copy_copy(_M_start, __pos, __first, __mid,
                                                  __new_start);
                        _M_start = __new_start;
                        copy(__mid, __last, __old_start);
                    }
            } catch(...) {
                _M_destroy_nodes(__new_start._M_node, _M_start._M_node);
            }
        }
        else {
            iterator __new_finish = _M_reserve_elements_at_back(__n);
            iterator __old_finish = _M_finish;
            const difference_type __elemsafter =
                    difference_type(__length) - __elemsbefore;
            __pos = _M_finish - __elemsafter;
            try {
                    if (__elemsafter > difference_type(__n)) {
                        iterator __finish_n = _M_finish - difference_type(__n);
                        uninitialized_copy(__finish_n, _M_finish, _M_finish);
                        _M_finish = __new_finish;
                        copy_backward(__pos, __finish_n, __old_finish);
                        copy(__first, __last, __pos);
                    }
                    else {
                        _ForwardIterator __mid = __first;
                        advance(__mid, __elemsafter);
                        __uninitialized_copy_copy(__mid, __last, __pos, _M_finish, _M_finish);
                        _M_finish = __new_finish;
                        copy(__first, __mid, __pos);
                    }
            } catch(...) {
                (_M_destroy_nodes(_M_finish._M_node + 1,
                                  __new_finish._M_node + 1));
            }
        }
    }


    // Called only if _M_finish._M_cur == _M_finish._M_first.
    template <class _Tp, class _Alloc>
    void deque<_Tp,_Alloc>::_M_pop_back_aux()
    {
        _M_deallocate_node(_M_finish._M_first);
        _M_finish._M_set_node(_M_finish._M_node - 1);
        _M_finish._M_cur = _M_finish._M_last - 1;
        destroy(_M_finish._M_cur);
    }

    // Called only if _M_start._M_cur == _M_start._M_last - 1.  Note that
    // if the deque has at least one element (a precondition for this member
    // function), and if _M_start._M_cur == _M_start._M_last, then the deque
    // must have at least two nodes.
    template <class _Tp, class _Alloc>
    void deque<_Tp,_Alloc>::_M_pop_front_aux()
    {
        destroy(_M_start._M_cur);
        _M_deallocate_node(_M_start._M_first);
        _M_start._M_set_node(_M_start._M_node + 1);
        _M_start._M_cur = _M_start._M_first;
    }

    template <class _Tp, class _Alloc>
    typename deque<_Tp,_Alloc>::iterator
    deque<_Tp,_Alloc>::erase(iterator __first, iterator __last)
    {
        if (__first == _M_start && __last == _M_finish) {
            clear();
            return _M_finish;
        }
        else {
            difference_type __n = __last - __first;
            difference_type __elems_before = __first - _M_start;
            if (__elems_before < difference_type((this->size() - __n) / 2)) {
                copy_backward(_M_start, __first, __last);
                iterator __new_start = _M_start + __n;
                destroy(_M_start, __new_start);
                _M_destroy_nodes(__new_start._M_node, _M_start._M_node);
                _M_start = __new_start;
            }
            else {
                copy(__last, _M_finish, __first);
                iterator __new_finish = _M_finish - __n;
                destroy(__new_finish, _M_finish);
                _M_destroy_nodes(__new_finish._M_node + 1, _M_finish._M_node + 1);
                _M_finish = __new_finish;
            }
            return _M_start + __elems_before;
        }
    }

    template <class _Tp, class _Alloc>
    void deque<_Tp,_Alloc>::clear()
    {
        // 删除 start 和 finish 之间的 node
        for (Map_pointer __node = _M_start._M_node + 1;
             __node < _M_finish._M_node;
             ++__node) {
            destroy(*__node, *__node + _S_buffer_size());
            _M_deallocate_node(*__node);
        }

        // 如果 start 和 finish 不同
        if (_M_start._M_node != _M_finish._M_node) {
            destroy(_M_start._M_cur, _M_start._M_last);
            destroy(_M_finish._M_first, _M_finish._M_cur);
            _M_deallocate_node(_M_finish._M_first);
        }
        // 否则，有效的元素为  _M_start._M_cur, _M_finish._M_cur
        else
            destroy(_M_start._M_cur, _M_finish._M_cur);

        _M_finish = _M_start;
    }



    template <class _Tp, class _Alloc>
    void deque<_Tp,_Alloc>::_M_new_elements_at_front(size_type __new_elems)
    {
        size_type __new_nodes
                = (__new_elems + _S_buffer_size() - 1) / _S_buffer_size();
        _M_reserve_map_at_front(__new_nodes);
        size_type __i;
        try {
                for (__i = 1; __i <= __new_nodes; ++__i)
                *(_M_start._M_node - __i) = _M_allocate_node();
        } catch(...) {
            for (size_type __j = 1; __j < __i; ++__j)
              _M_deallocate_node(*(_M_start._M_node - __j));
            throw;
        }

    }

    template <class _Tp, class _Alloc>
    void deque<_Tp,_Alloc>::_M_new_elements_at_back(size_type __new_elems)
    {
        size_type __new_nodes
                = (__new_elems + _S_buffer_size() - 1) / _S_buffer_size();
        _M_reserve_map_at_back(__new_nodes);
        size_type __i;
        try {
                for (__i = 1; __i <= __new_nodes; ++__i)
                *(_M_finish._M_node + __i) = _M_allocate_node();
        }
        catch(...) {
            for (size_type __j = 1; __j < __i; ++__j)
                _M_deallocate_node(*(_M_finish._M_node + __j));
            throw;
        }
    }

    // 重新分配 map 连续空间
    template <class _Tp, class _Alloc>
    void deque<_Tp,_Alloc>::_M_reallocate_map(size_type __nodes_to_add,
                                              bool __add_at_front)
    {
        size_type __old_num_nodes = _M_finish._M_node - _M_start._M_node + 1;
        size_type __new_num_nodes = __old_num_nodes + __nodes_to_add;

        Map_pointer __new_nstart;
        if (_M_map_size > 2 * __new_num_nodes) {
            __new_nstart = _M_map + (_M_map_size - __new_num_nodes) / 2
                           + (__add_at_front ? __nodes_to_add : 0);
            if (__new_nstart < _M_start._M_node)
                copy(_M_start._M_node, _M_finish._M_node + 1, __new_nstart);
            else
                copy_backward(_M_start._M_node, _M_finish._M_node + 1,
                              __new_nstart + __old_num_nodes);
        }
        else {
            size_type __new_map_size =
                    _M_map_size + max(_M_map_size, __nodes_to_add) + 2;

            Map_pointer __new_map = _M_allocate_map(__new_map_size);
            __new_nstart = __new_map + (__new_map_size - __new_num_nodes) / 2
                           + (__add_at_front ? __nodes_to_add : 0);
            copy(_M_start._M_node, _M_finish._M_node + 1, __new_nstart);
            _M_deallocate_map(_M_map, _M_map_size);

            _M_map = __new_map;
            _M_map_size = __new_map_size;
        }

        _M_start._M_set_node(__new_nstart);
        _M_finish._M_set_node(__new_nstart + __old_num_nodes - 1);
    }
}


#endif //MEMORY_MANAGE_DEQUE_H
