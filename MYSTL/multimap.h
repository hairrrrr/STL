//
// Created by 78172 on 2023/4/8.
//

#ifndef MEMORY_MANAGE_MULTIMAP_H
#define MEMORY_MANAGE_MULTIMAP_H

#include"rb_tree.h"
#include"function.h"

namespace MYSTL
{

    template <class _Key, class _Tp, class _Compare = MYSTL::less<_Key>,
              class _Alloc = std::allocator<_Key> >
    class multimap {

    public:
        // typedefs:
        typedef _Key                  key_type;
        typedef _Tp                   data_type;
        typedef _Tp                   mapped_type;
        typedef pair<const _Key, _Tp> value_type;
        typedef _Compare              key_compare;

        class value_compare : public binary_function<value_type, value_type, bool> {
            friend class multimap<_Key,_Tp,_Compare>;
        protected:
            _Compare comp;
            value_compare(_Compare __c) : comp(__c) {}
        public:
            bool operator()(const value_type& __x, const value_type& __y) const {
                return comp(__x.first, __y.first);
            }
        };

    private:
        typedef _Rb_tree<key_type, value_type,
                select1st<value_type>, key_compare> _Rep_type;
        _Rep_type _M_t;  // red-black tree representing multimap

    public:
        typedef typename _Rep_type::pointer pointer;
        typedef typename _Rep_type::const_pointer const_pointer;
        typedef typename _Rep_type::reference reference;
        typedef typename _Rep_type::const_reference const_reference;
        typedef typename _Rep_type::iterator iterator;
        typedef typename _Rep_type::const_iterator const_iterator;
        typedef typename _Rep_type::reverse_iterator reverse_iterator;
        typedef typename _Rep_type::const_reverse_iterator const_reverse_iterator;
        typedef typename _Rep_type::size_type size_type;
        typedef typename _Rep_type::difference_type difference_type;
        typedef typename _Rep_type::allocator_type allocator_type;

// allocation/deallocation

        multimap() : _M_t(_Compare()) { }

        explicit multimap(const _Compare& __comp)
                : _M_t(__comp) { }


        multimap(const multimap<_Key,_Tp,_Compare,_Alloc>& __x)
            : _M_t(__x._M_t) { }

        multimap<_Key,_Tp,_Compare,_Alloc>&
        operator=(const multimap<_Key,_Tp,_Compare,_Alloc>& __x) {
            _M_t = __x._M_t;
            return *this;
        }


        // accessors:
        key_compare key_comp() const { return _M_t.key_comp(); }
        value_compare value_comp() const { return value_compare(_M_t.key_comp()); }
        allocator_type get_allocator() const { return _M_t.get_allocator(); }

        iterator begin() { return _M_t.begin(); }
        const_iterator begin() const { return _M_t.begin(); }
        iterator end() { return _M_t.end(); }
        const_iterator end() const { return _M_t.end(); }
        reverse_iterator rbegin() { return _M_t.rbegin(); }
        const_reverse_iterator rbegin() const { return _M_t.rbegin(); }
        reverse_iterator rend() { return _M_t.rend(); }
        const_reverse_iterator rend() const { return _M_t.rend(); }
        bool empty() const { return _M_t.empty(); }
        size_type size() const { return _M_t.size(); }
        size_type max_size() const { return _M_t.max_size(); }
        void swap(multimap<_Key,_Tp,_Compare,_Alloc>& __x) { _M_t.swap(__x._M_t); }

        // insert/erase

        iterator insert(const value_type& __x)
        { return _M_t.insert_equal(__x); }

        void erase(iterator __position)
        { _M_t.erase(__position); }

        size_type erase(const key_type& __x)
        { return _M_t.erase(__x); }

        void clear() { _M_t.clear(); }

        // multimap operations:

        iterator find(const key_type& __x) { return _M_t.find(__x); }
        const_iterator find(const key_type& __x) const { return _M_t.find(__x); }
        size_type count(const key_type& __x) const { return _M_t.count(__x); }
        iterator lower_bound(const key_type& __x) {return _M_t.lower_bound(__x); }
        const_iterator lower_bound(const key_type& __x) const {
            return _M_t.lower_bound(__x);
        }
        iterator upper_bound(const key_type& __x) {return _M_t.upper_bound(__x); }
        const_iterator upper_bound(const key_type& __x) const {
            return _M_t.upper_bound(__x);
        }
        pair<iterator,iterator> equal_range(const key_type& __x) {
            return _M_t.equal_range(__x);
        }
        pair<const_iterator,const_iterator> equal_range(const key_type& __x) const {
            return _M_t.equal_range(__x);
        }

        template <class _K1, class _T1, class _C1, class _A1>
        friend bool operator== (const multimap<_K1, _T1, _C1, _A1>&,
                                const multimap<_K1, _T1, _C1, _A1>&);

        template <class _K1, class _T1, class _C1, class _A1>
        friend bool operator< (const multimap<_K1, _T1, _C1, _A1>&,
                               const multimap<_K1, _T1, _C1, _A1>&);

    };

    template <class _Key, class _Tp, class _Compare, class _Alloc>
    inline bool operator==(const multimap<_Key,_Tp,_Compare,_Alloc>& __x,
                           const multimap<_Key,_Tp,_Compare,_Alloc>& __y) {
        return __x._M_t == __y._M_t;
    }

    template <class _Key, class _Tp, class _Compare, class _Alloc>
    inline bool operator<(const multimap<_Key,_Tp,_Compare,_Alloc>& __x,
                          const multimap<_Key,_Tp,_Compare,_Alloc>& __y) {
        return __x._M_t < __y._M_t;
    }

}


#endif //MEMORY_MANAGE_MULTIMAP_H
