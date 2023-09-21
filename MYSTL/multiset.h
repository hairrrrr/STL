//
// Created by 78172 on 2023/4/8.
//

#ifndef MEMORY_MANAGE_MULTISET_H
#define MEMORY_MANAGE_MULTISET_H

#include"rb_tree.h"
#include"function.h"

namespace MYSTL
{

    template <class _Key, class _Compare = MYSTL::less<_Key>,
              class _Alloc = std::allocator<_Key> >
    class multiset
    {
    public:
        // typedefs:
        typedef _Key     key_type;
        typedef _Key     value_type;
        typedef _Compare key_compare;
        typedef _Compare value_compare;

    private:
        typedef _Rb_tree< key_type, value_type,
                         _Identity<value_type>, key_compare> _Rep_type;

        _Rep_type _M_t;  // red-black tree representing multiset

    public:
        typedef typename _Rep_type::const_pointer pointer;
        typedef typename _Rep_type::const_pointer const_pointer;
        typedef typename _Rep_type::const_reference reference;
        typedef typename _Rep_type::const_reference const_reference;
        typedef typename _Rep_type::const_iterator iterator;
        typedef typename _Rep_type::const_iterator const_iterator;
        typedef typename _Rep_type::const_reverse_iterator reverse_iterator;
        typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
        typedef typename _Rep_type::size_type size_type;
        typedef typename _Rep_type::difference_type difference_type;
        typedef typename _Rep_type::allocator_type allocator_type;

        // allocation/deallocation

        multiset() : _M_t(_Compare()) {}
        explicit multiset(const _Compare& __comp)
                : _M_t(__comp) {}


        multiset(const multiset<_Key,_Compare,_Alloc>& __x) : _M_t(__x._M_t) {}

        multiset<_Key,_Compare,_Alloc>&
        operator=(const multiset<_Key,_Compare,_Alloc>& __x) {
            _M_t = __x._M_t;
            return *this;
        }

        // accessors:

        key_compare key_comp() const { return _M_t.key_comp(); }
        value_compare value_comp() const { return _M_t.key_comp(); }
        allocator_type get_allocator() const { return _M_t.get_allocator(); }

        iterator begin() const { return _M_t.begin(); }
        iterator end() const { return _M_t.end(); }
        reverse_iterator rbegin() const { return _M_t.rbegin(); }
        reverse_iterator rend() const { return _M_t.rend(); }
        bool empty() const { return _M_t.empty(); }
        size_type size() const { return _M_t.size(); }
        size_type max_size() const { return _M_t.max_size(); }
        void swap(multiset<_Key,_Compare,_Alloc>& __x) { _M_t.swap(__x._M_t); }

        // insert/erase
        iterator insert(const value_type& __x) {
            return _M_t.insert_equal(__x);
        }

        void erase(iterator __position) {
            typedef typename _Rep_type::iterator _Rep_iterator;
            _M_t.erase((_Rep_iterator&)__position);
        }

        size_type erase(const key_type& __x) {
            return _M_t.erase(__x);
        }

        void clear() { _M_t.clear(); }


        // multiset operations:
        iterator find(const key_type& __x) const { return _M_t.find(__x); }
        size_type count(const key_type& __x) const { return _M_t.count(__x); }
        iterator lower_bound(const key_type& __x) const
        {
            return _M_t.lower_bound(__x);
        }
        iterator upper_bound(const key_type& __x) const
        {
            return _M_t.upper_bound(__x);
        }
        pair<iterator,iterator> equal_range(const key_type& __x) const
        {
            return _M_t.equal_range(__x);
        }


        template <class _K1, class _C1, class _A1>
        friend bool operator== (const multiset<_K1,_C1,_A1>&,
                                const multiset<_K1,_C1,_A1>&);

        template <class _K1, class _C1, class _A1>
        friend bool operator< (const multiset<_K1,_C1,_A1>&,
                               const multiset<_K1,_C1,_A1>&);

    };

    template <class _Key, class _Compare, class _Alloc>
    inline bool operator==(const multiset<_Key,_Compare,_Alloc>& __x,
                           const multiset<_Key,_Compare,_Alloc>& __y) {
        return __x._M_t == __y._M_t;
    }

    template <class _Key, class _Compare, class _Alloc>
    inline bool operator<(const multiset<_Key,_Compare,_Alloc>& __x,
                          const multiset<_Key,_Compare,_Alloc>& __y) {
        return __x._M_t < __y._M_t;
    }

}


#endif //MEMORY_MANAGE_MULTISET_H
