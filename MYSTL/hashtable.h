//
// Created by 78172 on 2023/4/9.
//

#ifndef MEMORY_MANAGE_HASHTABLE_H
#define MEMORY_MANAGE_HASHTABLE_H

#include"vector.h"
#include"iterator.h"
#include"algo.h"
#include"uninitialized.h"
#include"function.h"
#include"allocator.h"
#include"pair.h"

namespace MYSTL {

    template<typename Val>
    struct _Hashtable_node
    {
        _Hashtable_node *_M_next;
        Val _M_val;
    };

    template<class _Val, class _Key, class _HashFcn,
             class _ExtractKey, class _EqualKey,
             class _Alloc = std::allocator<_Hashtable_node<_Val> > >
    class hashtable;

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_iterator;

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_const_iterator;


    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_iterator {
        typedef hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                _Hashtable;

        typedef _Hashtable_iterator<_Val, _Key, _HashFcn,
                _ExtractKey, _EqualKey, _Alloc> iterator;

        typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn,
                _ExtractKey, _EqualKey, _Alloc> const_iterator;

        typedef _Hashtable_node<_Val> _Node;

        typedef forward_iterator_tag iterator_category; // 迭代器类型

        typedef _Val value_type;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        typedef _Val &reference;
        typedef _Val *pointer;

        _Node *_M_cur;     // 迭代器目前所指的节点
        _Hashtable *_M_ht;      // 保持对容器的连接关系，bucket

        _Hashtable_iterator(_Node *__n, _Hashtable *__tab)
                : _M_cur(__n), _M_ht(__tab) {}

        _Hashtable_iterator() {}

        reference operator*() const { return _M_cur->_M_val; }

        pointer operator->() const { return &(operator*()); }

        iterator &operator++(); // 实现
        iterator operator++(int);

        bool operator==(const iterator &__it) const { return _M_cur == __it._M_cur; }

        bool operator!=(const iterator &__it) const { return _M_cur != __it._M_cur; }
    };

    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    struct _Hashtable_const_iterator {
        typedef hashtable<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                _Hashtable;

        typedef _Hashtable_iterator<_Val, _Key, _HashFcn,
                _ExtractKey, _EqualKey, _Alloc> iterator;

        typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn,
                _ExtractKey, _EqualKey, _Alloc> const_iterator;

        typedef _Hashtable_node<_Val> _Node;

        typedef forward_iterator_tag iterator_category; // 迭代器类型

        typedef _Val value_type;
        typedef ptrdiff_t difference_type;
        typedef size_t size_type;
        typedef _Val &reference;
        typedef _Val *pointer;

        _Node *_M_cur;     // 迭代器目前所指的节点
        _Hashtable *_M_ht;      // 保持对容器的连接关系，bucket

        _Hashtable_const_iterator(_Node *__n, _Hashtable *__tab)
                : _M_cur(__n), _M_ht(__tab) {}

        _Hashtable_const_iterator() {}

        _Hashtable_const_iterator(const iterator &__it)
                : _M_cur(__it._M_cur), _M_ht(__it._M_ht) {}

        reference operator*() const { return _M_cur->_M_val; }

        pointer operator->() const { return &(operator*()); }

        const_iterator &operator++(); // 实现
        const_iterator operator++(int);

        bool operator==(const iterator &__it) const { return _M_cur == __it._M_cur; }

        bool operator!=(const iterator &__it) const { return _M_cur != __it._M_cur; }
    };


    // Note: assumes long is at least 32 bits.
    enum {
        __stl_num_primes = 28
    };
    // SGI STL 提供 28 个质数，用质数大小来设计 buckets
    static const unsigned long __stl_prime_list[__stl_num_primes] =
    {
            53ul, 97ul, 193ul, 389ul, 769ul,
            1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
            49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
            1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
            50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
            1610612741ul, 3221225473ul, 4294967291ul
    };

    // 质数表内找到第一个大于等于 n 的数
    inline unsigned long __stl_next_prime(unsigned long __n) {
        const unsigned long *__first = __stl_prime_list;
        const unsigned long *__last = __stl_prime_list + (int) __stl_num_primes;
        const unsigned long *pos = lower_bound(__first, __last, __n);
        return pos == __last ? *(__last - 1) : *pos;
    }

    // hash table 数据结构
    // _Val 节点的实值类型，_Key 节点的键值类型，_HashFcn 哈希函数的类型，
    // _ExtractKey 从节点中取出键值的方法，_EqualKey 判断键值是否相同的方法
    template<class _Val, class _Key, class _HashFcn,
            class _ExtractKey, class _EqualKey, class _Alloc>
    class hashtable {
    public:
        typedef _Key key_type;
        typedef _Val value_type;
        typedef _HashFcn hasher;
        typedef _EqualKey key_equal;

        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef value_type *pointer;
        typedef value_type &reference;

        typedef const value_type *const_pointer;
        typedef const value_type &const_reference;

        hasher hash_funct() const { return _M_hash; }

        key_equal key_eq() const { return _M_equals; }

    private:
        typedef _Hashtable_node<_Val> _Node;

    public:
        typedef _Alloc allocator_type;

        allocator_type get_allocator() const { return allocator_type(); }

    private:
        typedef MYSTL::allocator<_Node, _Alloc> _M_node_allocator_type;

        _Node *_M_get_node() { return _M_node_allocator_type().allocate(1); }

        void _M_put_node(_Node *__p) { _M_node_allocator_type().deallocate(__p, 1); }

    private:
        hasher      _M_hash;
        key_equal   _M_equals;
        _ExtractKey _M_get_key;
        std::vector<_Node *> _M_buckets; // vector 容器
        size_type   _M_num_elements;

    public:
        typedef _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>
                iterator;
        typedef _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey,
                _Alloc>
                const_iterator;

        // 方便获取 _M_buckets
        friend struct
                _Hashtable_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>;
        friend struct
                _Hashtable_const_iterator<_Val, _Key, _HashFcn, _ExtractKey, _EqualKey, _Alloc>;


    public:
        hashtable(size_type __n,
                  const _HashFcn &__hf,
                  const _EqualKey &__eql,
                  const _ExtractKey &__ext)
                : _M_hash(__hf),
                  _M_equals(__eql),
                  _M_get_key(__ext),
                  _M_buckets(),
                  _M_num_elements(0) {
            _M_initialize_buckets(__n); // 初始构造 __n 个节点，选取符合的质数
        }

        hashtable(size_type __n,
                  const _HashFcn &__hf,
                  const _EqualKey &__eql)
                : _M_hash(__hf),
                  _M_equals(__eql),
                  _M_get_key(_ExtractKey()),
                  _M_buckets(),
                  _M_num_elements(0) {
            _M_initialize_buckets(__n);
        }

        hashtable(const hashtable &__ht)
                : _M_hash(__ht._M_hash),
                  _M_equals(__ht._M_equals),
                  _M_get_key(__ht._M_get_key),
                  _M_buckets(__ht.get_allocator()),
                  _M_num_elements(0) {
            _M_copy_from(__ht);
        }


        hashtable &operator=(const hashtable &__ht) {
            if (&__ht != this) {
                clear();
                _M_hash = __ht._M_hash;
                _M_equals = __ht._M_equals;
                _M_get_key = __ht._M_get_key;
                _M_copy_from(__ht);
            }
            return *this;
        }

        ~hashtable() { clear(); }

        size_type size() const { return _M_num_elements; }

        size_type max_size() const { return size_type(-1); }

        bool empty() const { return size() == 0; }

        void swap(hashtable &__ht) {
            std::swap(_M_hash, __ht._M_hash);
            std::swap(_M_equals, __ht._M_equals);
            std::swap(_M_get_key, __ht._M_get_key);
            _M_buckets.swap(__ht._M_buckets);
            std::swap(_M_num_elements, __ht._M_num_elements);
        }

        // 迭代器指向首尾 buckets
        iterator begin() {
            for (size_type __n = 0; __n < _M_buckets.size(); ++__n)
                if (_M_buckets[__n])
                    return iterator(_M_buckets[__n], this);
            return end();
        }

        iterator end() { return iterator(0, this); }

        const_iterator begin() const {
            for (size_type __n = 0; __n < _M_buckets.size(); ++__n)
                if (_M_buckets[__n])
                    return const_iterator(_M_buckets[__n], this);
            return end();
        }

        const_iterator end() const { return const_iterator(0, this); }

        template<class _Vl, class _Ky, class _HF, class _Ex, class _Eq, class _Al>
        friend bool operator==(const hashtable<_Vl, _Ky, _HF, _Ex, _Eq, _Al> &,
                               const hashtable<_Vl, _Ky, _HF, _Ex, _Eq, _Al> &);


    public:

        size_type bucket_count() const { return _M_buckets.size(); } // buckets 大小

        size_type max_bucket_count() const
        { return __stl_prime_list[(int) __stl_num_primes - 1]; }

        // 求某个bucket节点下的节点个数
        size_type elems_in_bucket(size_type __bucket) const {
            size_type __result = 0;
            for (_Node *__cur = _M_buckets[__bucket];
                 __cur; __cur = __cur->_M_next)
                __result += 1;
            return __result;
        }

        // 不允许有重复的节点
        pair<iterator, bool> insert_unique(const value_type &__obj)
        {
            resize(_M_num_elements + 1); // 判断是否需要重置 buckets
            return insert_unique_noresize(__obj);
        }

        // 允许有重复的节点
        iterator insert_equal(const value_type &__obj)
        {
            resize(_M_num_elements + 1);
            return insert_equal_noresize(__obj);
        }

        pair<iterator, bool> insert_unique_noresize(const value_type &__obj);

        iterator insert_equal_noresize(const value_type &__obj);


        iterator find( const _Key& k )
        {
            size_type pos = _M_bkt_num_key(k);
            _Node* first;
            for( first = _M_buckets[pos];
                 first && !_M_equals(k, _M_get_key(first->_M_val));
                 first = first->_M_next)
            {}

            return iterator(first, this);
        }

        const_iterator find(const key_type& __key) const
        {
            size_type __n = _M_bkt_num_key(__key);
            const _Node* __first;
            for ( __first = _M_buckets[__n];
                  __first && !_M_equals(_M_get_key(__first->_M_val), __key);
                  __first = __first->_M_next)
            {}
            return const_iterator(__first, this);
        }

        size_type count(const key_type& __key) const
        {
            const size_type __n = _M_bkt_num_key(__key);
            size_type __result = 0;

            for (const _Node* __cur = _M_buckets[__n]; __cur; __cur = __cur->_M_next)
                if (_M_equals(_M_get_key(__cur->_M_val), __key))
                    ++__result;
            return __result;
        }

        pair<iterator, iterator>
        equal_range(const key_type &__key);

        pair<const_iterator, const_iterator>
        equal_range(const key_type &__key) const;

        size_type erase(const key_type &__key);

        void erase(const iterator &__it);

        void resize(size_type __num_elements_hint);

        void clear();

    // Debug
    public:
        void show_hash_elem() const
        {
            using std::cout;

            cout << "All Elems: ---------------------\n";
            for(size_t i = 0; i < _M_buckets.size(); ++i)
                if( _M_buckets[i] )
                {
                    cout << "[ " << i << " ]: ";
                    _Node* cur = _M_buckets[i];
                    while( cur )
                    {
                        cout << (cur->_M_val).second << " ";
                        cur = cur->_M_next;
                    }
                    cout << std::endl;
                }
            cout << "--------------------- finish\n";
        }

    private:
        size_type _M_next_size(size_type __n) const { return __stl_next_prime(__n); }

        // 初始化 buckets vector
        void _M_initialize_buckets(size_type __n) {
            const size_type __n_buckets = _M_next_size(__n);
            _M_buckets.reserve(__n_buckets);
            _M_buckets.resize(__n_buckets, nullptr);
            _M_num_elements = 0;
        }

        // 只接受键值
        size_type _M_bkt_num_key(const key_type &__key) const {
            return _M_bkt_num_key(__key, _M_buckets.size());
        }

        // 只接受实值
        size_type _M_bkt_num(const value_type &__obj) const {
            return _M_bkt_num_key(_M_get_key(__obj));
        }

        // 接受键值和 buckets 个数
        size_type _M_bkt_num_key(const key_type &__key, size_t __n) const {
            return _M_hash(__key) % __n;
        }

        // 接受实值和 buckets 个数
        size_type _M_bkt_num(const value_type &__obj, size_t __n) const {
            return _M_bkt_num_key(_M_get_key(__obj), __n);
        }

        // 节点配置函数
        _Node *_M_new_node(const value_type &__obj) {
            _Node *__n = _M_get_node();
            __n->_M_next = nullptr;
            try {
                construct(&__n->_M_val, __obj);
                return __n;
            }
            catch (...) {
                _M_put_node(__n);
            }
        }

        // 节点释放函数
        void _M_delete_node(_Node *__n) {
            destroy(&__n->_M_val);
            _M_put_node(__n);
        }

        void _M_copy_from(const hashtable &__ht);

    };


    // 迭代器前进操作
    template<class _Val, class _Key, class _HF, class _ExK, class _EqK, class _All>
    _Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All> &
    _Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>
    ::operator++() {
        _Node *old = _M_cur;
        _M_cur = _M_cur->_M_next;
        if (!_M_cur) {
            for (size_type i = _M_ht->_M_bkt_num(old->_M_val) + 1;
                 i < _M_ht->_M__M_num_elements; ++i) {
                _Node *cur = _M_ht->_M_buckets[i];
                if (cur)
                    _M_cur = cur;
            }
        }
        return *this;
    }

    // 迭代器前进操作
    template<class _Val, class _Key, class _HF, class _ExK, class _EqK, class _All>
    _Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>
    _Hashtable_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>
    ::operator++(int) {
        iterator tmp(*this);
        ++(*this);
        return tmp;
    }

    template<class _Val, class _Key, class _HF, class _ExK, class _EqK,
            class _All>
    _Hashtable_const_iterator<_Val, _Key, _HF, _ExK, _EqK, _All> &
    _Hashtable_const_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>::operator++() {
        const _Node *__old = _M_cur;
        _M_cur = _M_cur->_M_next;
        if (!_M_cur) {
            size_type __bucket = _M_ht->_M_bkt_num(__old->_M_val);
            while (!_M_cur && ++__bucket < _M_ht->_M_buckets.size())
                _M_cur = _M_ht->_M_buckets[__bucket];
        }
        return *this;
    }

    template<class _Val, class _Key, class _HF, class _ExK, class _EqK,
            class _All>
    inline _Hashtable_const_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>
    _Hashtable_const_iterator<_Val, _Key, _HF, _ExK, _EqK, _All>::operator++(int) {
        const_iterator __tmp = *this;
        ++*this;
        return __tmp;
    }


    // 清空所有节点
    template<class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::clear() {
        for (size_type i = 0; i < _M_num_elements; ++i) {
            _Node *cur = _M_buckets[i];
            if (cur) {
                _Node *nxt = cur->_M_next;
                _M_delete_node(cur);
                cur = nxt;
            }
            _M_buckets[i] = nullptr;
        }

        _M_num_elements = 0;
    }

    // 哈希表复制，第一：vector 复制，第二：linked list 复制
    template<class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>
    ::_M_copy_from(const hashtable &__ht) {
        _M_buckets.clear();
        _M_buckets.reserve(__ht._M_buckets.size());
        _M_buckets.resize(__ht._M_buckets.size(), nullptr);
        try {
            for (size_type __i = 0; __i < __ht._M_buckets.size(); ++__i) {
                const _Node *__cur = __ht._M_buckets[__i];
                if (__cur) {
                    _Node *__copy = _M_new_node(__cur->_M_val);
                    _M_buckets[__i] = __copy;

                    for (_Node *__next = __cur->_M_next; __next;
                         __cur = __next, __next = __cur->_M_next) {
                        __copy->_M_next = _M_new_node(__next->_M_val);
                        __copy = __copy->_M_next;
                    }
                }
            }
            _M_num_elements = __ht._M_num_elements;
        }
        catch (...) {
            clear();
        }
    }


    template<class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    bool operator==(const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All> &__ht1,
                    const hashtable<_Val, _Key, _HF, _Ex, _Eq, _All> &__ht2) {
        typedef typename hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::_Node _Node;
        if (__ht1._M_buckets.size() != __ht2._M_buckets.size())
            return false;
        for (int __n = 0; __n < __ht1._M_buckets.size(); ++__n) {
            _Node *__cur1 = __ht1._M_buckets[__n];
            _Node *__cur2 = __ht2._M_buckets[__n];
            for (; __cur1 && __cur2 && __cur1->_M_val == __cur2->_M_val;
                   __cur1 = __cur1->_M_next, __cur2 = __cur2->_M_next) {}
            if (__cur1 || __cur2)
                return false;
        }
        return true;
    }


    // 判断重建 buckets 大小
    template<class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void hashtable<_Val, _Key, _HF, _Ex, _Eq, _All>::
    resize(size_type __num_elements_hint)
    {
        if( __num_elements_hint <= _M_buckets.size() ) return;

        size_type old_n = _M_buckets.size();
        size_type n = _M_next_size(__num_elements_hint);
        if( n > old_n )
        {
            std::vector<_Node*> tmp( n, nullptr );
            try {
                for(size_type i = 0; i < old_n; ++i)
                {
                    _Node* cur = _M_buckets[i];
                    while( cur )
                    {
                        size_type pos = _M_bkt_num(cur->_M_val, n);
                        _M_buckets[i] = cur->_M_next;
                        cur->_M_next = tmp[pos];
                        tmp[pos] = cur;
                        cur = _M_buckets[i];
                    }
                }

                _M_buckets.swap(tmp);

            } catch( ... ) {
                for(size_type i = 0; i < n; ++i)
                {
                    _Node* cur = tmp[i];
                    while( cur )
                    {
                        _Node* nxt = cur->_M_next;
                        _M_delete_node(cur);
                        cur = nxt;
                    }
                }
                throw;
            }
        }
    }

    // 在不需要重建 buckets 大小下，插入新节点，键值不能重复
    template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    pair<typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::iterator, bool>
    hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::
    insert_unique_noresize(const value_type& obj)
    {
        size_type pos = _M_bkt_num(obj);
        _Node* first  = _M_buckets[pos];

        for(_Node* cur = first; cur; cur = cur->_M_next)
            if( _M_equals( _M_get_key( obj ), _M_get_key( cur->_M_val ) ) )
                return MYSTL::make_pair( iterator(cur, this), false );

        // 头插
        _Node* new_node = _M_new_node(obj);
        new_node->_M_next = first;
        _M_buckets[pos] = new_node;
        ++_M_num_elements;
        return MYSTL::make_pair( iterator(new_node, this), true );
    }

    // 在不需要重建 buckets 大小下，插入新节点，键值可以重复
    template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::iterator
    hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
    ::insert_equal_noresize(const value_type& __obj)
    {
        const size_type __n = _M_bkt_num(__obj);
        _Node* __first = _M_buckets[__n];

        // key 相等元素放在一起
        for (_Node* __cur = __first; __cur; __cur = __cur->_M_next)
            if (_M_equals(_M_get_key(__cur->_M_val), _M_get_key(__obj))) {
                _Node* __tmp = _M_new_node(__obj); // 相等，插入后面
                __tmp->_M_next = __cur->_M_next;
                __cur->_M_next = __tmp;
                ++_M_num_elements;
                return iterator(__tmp, this);
            }

        _Node* __tmp = _M_new_node(__obj);
        __tmp->_M_next = __first;
        _M_buckets[__n] = __tmp;
        ++_M_num_elements;
        return iterator(__tmp, this);
    }

    template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    pair<typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::iterator,
            typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::iterator>
    hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::equal_range(const key_type& __key)
    {
        typedef pair<iterator, iterator> _Pii;
        const size_type __n = _M_bkt_num_key(__key);

        for (_Node* __first = _M_buckets[__n]; __first; __first = __first->_M_next)
            // 如果有等于 key 的元素存在
            if (_M_equals(_M_get_key(__first->_M_val), __key))
            {
                // 此循环在当前 bucket 内找到第一个与 key 不等的元素作为 pair 的第二参数
                for (_Node* __cur = __first->_M_next; __cur; __cur = __cur->_M_next)
                    if (!_M_equals(_M_get_key(__cur->_M_val), __key))
                        return _Pii(iterator(__first, this), iterator(__cur, this));
                // 程序执行到这里，说明当前 bucket 内从 first 到最后一个元素都等于 key
                // 此循环在当前 bucket 后面的 buckets 中找到一个元素作为 pair 的第二参数
                for (size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
                    if (_M_buckets[__m])
                        return _Pii(iterator(__first, this),
                                    iterator(_M_buckets[__m], this));

                // 当前 bucket 后面没有元素了，用 end 作为 pair 第二参数
                return _Pii(iterator(__first, this), end());
            }
        return _Pii(end(), end());
    }

    template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    pair<typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::const_iterator,
            typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::const_iterator>
    hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>
    ::equal_range(const key_type& __key) const
    {
        typedef pair<const_iterator, const_iterator> _Pii;
        const size_type __n = _M_bkt_num_key(__key);

        for (const _Node* __first = _M_buckets[__n] ;
             __first;
             __first = __first->_M_next) {
            if (_M_equals(_M_get_key(__first->_M_val), __key)) {
                for (const _Node* __cur = __first->_M_next;
                     __cur;
                     __cur = __cur->_M_next)
                    if (!_M_equals(_M_get_key(__cur->_M_val), __key))
                        return _Pii(const_iterator(__first, this),
                                    const_iterator(__cur, this));
                for (size_type __m = __n + 1; __m < _M_buckets.size(); ++__m)
                    if (_M_buckets[__m])
                        return _Pii(const_iterator(__first, this),
                                    const_iterator(_M_buckets[__m], this));
                return _Pii(const_iterator(__first, this), end());
            }
        }
        return _Pii(end(), end());
    }


    // 删除指定的 key
    template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    typename hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::size_type
    hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::erase(const key_type& key)
    {
        size_type pos = _M_bkt_num_key(key);
        _Node* first = _M_buckets[pos];
        size_type cnt = 0;

        if(first)
        {
            _Node* cur = first;
            _Node* next = first->_M_next;
            // 删除 first 后可能才能在的与 key 相同的元素
            while( next )
            {
                if( _M_equals( key, _M_get_key(next->_M_val) ) )
                {
                    cur->_M_next = next->_M_next;
                    _M_delete_node(next);
                    ++cnt;
                    --_M_num_elements;
                    next = cur->_M_next;
                }
                else
                {
                    cur = next;
                    next = next->_M_next;
                }
            }
            // 如果 first 和 key 相同，头删除
            if( _M_equals( key, _M_get_key(first->_M_val) ) )
            {
                _M_buckets[pos] = first->_M_next;
                _M_delete_node(first);
                ++cnt;
                --_M_num_elements;
            }

        }

        return cnt;
    }

    // 删除一个节点
    template <class _Val, class _Key, class _HF, class _Ex, class _Eq, class _All>
    void hashtable<_Val,_Key,_HF,_Ex,_Eq,_All>::erase(const iterator& __it) {
        _Node *__p = __it._M_cur;
        if (__p) {
            const size_type __n = _M_bkt_num(__p->_M_val);
            _Node *__cur = _M_buckets[__n];

            if (__cur == __p) {
                _M_buckets[__n] = __cur->_M_next;
                _M_delete_node(__cur);
                --_M_num_elements;
            } else {
                _Node *__next = __cur->_M_next;
                while (__next) {
                    if (__next == __p) {
                        __cur->_M_next = __next->_M_next;
                        _M_delete_node(__next);
                        --_M_num_elements;
                        break;
                    } else {
                        __cur = __next;
                        __next = __cur->_M_next;
                    }
                }
            }
        }
    }


}


#endif //MEMORY_MANAGE_HASHTABLE_H
