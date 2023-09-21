//
// Created by 78172 on 2023/4/8.
//

#ifndef MEMORY_MANAGE_MAP_H
#define MEMORY_MANAGE_MAP_H

#include"rb_tree.h"
#include"function.h"
#include "pair.h"

namespace MYSTL
{

    template <class _Key, class _Tp, class _Compare = MYSTL::less<_Key>,
              class _Alloc = std::allocator<_Tp> >
    class map
    {
    public:
        typedef _Key                  key_type; // 键值类型
        typedef _Tp                   data_type; // 实值类型
        typedef _Tp                   mapped_type;
        typedef pair<const _Key, _Tp> value_type;  // 元素类型(键值/实值)
        typedef _Compare              key_compare; // 键值比较函数

        // 定义一个 functor，其作用就是调用“元素比较函数”
        class value_compare
                : public binary_function<value_type, value_type, bool>
        {
            friend class map<_Key,_Tp,_Compare,_Alloc>;
        protected :
            _Compare comp;
            value_compare(_Compare __c) : comp(__c) {}
        public:
            bool operator()(const value_type& __x, const value_type& __y) const
            { return comp(__x.first, __y.first); }
        };

    private:
        typedef _Rb_tree<key_type, value_type, select1st<value_type>,
                         key_compare> _Rep_type;
        _Rep_type _M_t;

    public:
        typedef typename _Rep_type::pointer pointer;
        typedef typename _Rep_type::const_pointer const_pointer;
        typedef typename _Rep_type::reference reference;
        typedef typename _Rep_type::const_reference const_reference;
        typedef typename _Rep_type::iterator iterator;  // map 迭代器
        typedef typename _Rep_type::const_iterator const_iterator;
        typedef typename _Rep_type::reverse_iterator reverse_iterator;
        typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
        typedef typename _Rep_type::size_type size_type;
        typedef typename _Rep_type::difference_type difference_type;
        typedef typename _Rep_type::allocator_type allocator_type;

        map() : _M_t(_Compare()) {}

        map(const map& m) : _M_t(m._M_t) {}

        map operator=(const map& m)
        {
            _M_t = m._M_t;
            return *this;
        }


        key_compare    key_comp()      const { return _M_t.key_comp(); }
        value_compare  value_comp()    const { return value_compare(_M_t.key_comp()); }
        allocator_type get_allocator() const { return _M_t.get_allocator(); }

        iterator       begin() { return _M_t.begin(); }
        iterator       end()   { return _M_t.end(); }

        const_iterator begin() const { return _M_t.begin(); }
        const_iterator end()   const { return _M_t.end(); }

        reverse_iterator rbegin() { return _M_t.rbegin(); }
        reverse_iterator rend()   { return _M_t.rend(); }

        const_reverse_iterator rbegin() const { return _M_t.rbegin(); }
        const_reverse_iterator rend()   const { return _M_t.rend(); }

        bool      empty()    const { return _M_t.empty(); }
        size_type size()     const { return _M_t.size();  }
        size_type max_size() const { return _M_t.max_size(); }

        void swap(map& m) { _M_t.swap(m._M_t); }

//        _Tp& operator[](const _Key& k)
//        {
//            return  (*(insert(value_type(k, _Tp())).first)).second;
//        }

        // 另一种写法：
        _Tp& operator[](const _Key& k)
        {
            iterator it = lower_bound(k);

            if( it == end() || key_comp()(k, (*it).first ) )
               it = insert(value_type(k, _Tp())).first;

            return (*it).second;
        }

        pair<iterator, bool> insert(const value_type& x)
        {
//            cout << x.first << " " << x.second << endl;
            return _M_t.insert_unique(x);
        }

        void erase(iterator pos)
        { _M_t.erase(pos); }

        size_type erase(const _Key& k)
        { return _M_t.erase(k); }

        void clear() { return _M_t.clear(); }

        // map operations:
        // 寻找带有特定键的元素
        iterator       find(const key_type& __x)
        { return _M_t.find(__x); }
        const_iterator find(const key_type& __x) const
        { return _M_t.find(__x); }

        // 直接调用 _M_t.count 也是可以的
        size_type count(const key_type& __x) const
        { return _M_t.find(__x) == _M_t.end() ? 0 : 1; }

        // 返回指向首个不小于给定关键的元素的迭代器
        iterator       lower_bound(const key_type& __x)
        { return _M_t.lower_bound(__x); }
        const_iterator lower_bound(const key_type& __x) const
        { return _M_t.lower_bound(__x); }

        // 返回指向首个大于给定关键的元素的迭代器
        iterator upper_bound(const key_type& __x)
        { return _M_t.upper_bound(__x); }
        const_iterator upper_bound(const key_type& __x) const
        { return _M_t.upper_bound(__x); }

        // 返回匹配特定键的元素范围
        pair<iterator,iterator> equal_range(const key_type& __x)
        { return _M_t.equal_range(__x); }
        pair<const_iterator,const_iterator> equal_range (const key_type& __x) const
        { return _M_t.equal_range(__x); }


        template <class _K1, class _T1, class _C1, class _A1>
        friend bool operator== (const map<_K1, _T1, _C1, _A1>&,
                                const map<_K1, _T1, _C1, _A1>&);
        template <class _K1, class _T1, class _C1, class _A1>
        friend bool operator< (const map<_K1, _T1, _C1, _A1>&,
                               const map<_K1, _T1, _C1, _A1>&);
    };

    template <class _Key, class _Tp, class _Compare, class _Alloc>
    inline bool operator==(const map<_Key,_Tp,_Compare,_Alloc>& __x,
                           const map<_Key,_Tp,_Compare,_Alloc>& __y) {
        return __x._M_t == __y._M_t;
    }

    template <class _Key, class _Tp, class _Compare, class _Alloc>
    inline bool operator<(const map<_Key,_Tp,_Compare,_Alloc>& __x,
                          const map<_Key,_Tp,_Compare,_Alloc>& __y) {
        return __x._M_t < __y._M_t;
    }


} // namespace MYSTL


#endif //MEMORY_MANAGE_MAP_H
