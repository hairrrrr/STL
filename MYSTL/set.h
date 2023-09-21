//
// Created by 78172 on 2023/4/8.
//

#ifndef MEMORY_MANAGE_SET_H
#define MEMORY_MANAGE_SET_H

#include"rb_tree.h"
#include"function.h"

namespace MYSTL
{

    template<typename Key, typename Compare = MYSTL::less<Key>,
             typename Alloc = std::allocator<Key> >
    class set
    {
    public:
        typedef  Key      key_type;
        typedef  Key      value_type;
        typedef  Compare  key_compare;
        typedef  Compare  value_compare;

    private:
        typedef _Rb_tree<key_type, value_type, _Identity<value_type>,
                         key_compare> Rep_type;
        Rep_type _M_t;

    public:
        typedef typename Rep_type::const_pointer            pointer;
        typedef typename Rep_type::const_pointer            const_pointer;
        typedef typename Rep_type::const_reference          reference;
        typedef typename Rep_type::const_reference          const_reference;
        typedef typename Rep_type::const_iterator           iterator;
        typedef typename Rep_type::const_iterator           const_iterator;
        typedef typename Rep_type::const_reverse_iterator   reverse_iterator;
        typedef typename Rep_type::const_reverse_iterator   const_reverse_iterator;
        typedef typename Rep_type::size_type                size_type;
        typedef typename Rep_type::difference_type          difference_type;
        typedef typename Rep_type::allocator_type           allocator_type;


        set() : _M_t(Compare()) {}
        explicit set(const Compare& comp)
            : _M_t(comp) {}

        set(const set& s) : _M_t(s._M_t) {}

        set& operator=(const set& s)
        {
            _M_t = s._M_t;
            return *this;
        }


        key_compare   key_comp()   const { return _M_t.key_comp(); }
        value_compare value_comp() const { return _M_t.key_comp(); }
        allocator_type get_allocator() const { return _M_t.get_allocator(); }

        iterator begin() const { return _M_t.begin(); }
        iterator   end() const { return _M_t.end();   }
        reverse_iterator rbegin() const { return _M_t.rbegin(); }
        reverse_iterator   rend() const { return _M_t.rend();   }

        bool      empty() const { return _M_t.empty(); }
        size_type size()  const { return _M_t.size(); }
        size_type max_size() const { return _M_t.max_size(); }
        void swap(set& s) { _M_t.swap(s._M_t); }

        pair<iterator, bool> insert(const value_type& x)
        {
            pair<typename Rep_type::iterator, bool> ret = _M_t.insert_unique(x);
            return pair<iterator, bool>(ret.first, ret.second);
        }

        void erase(iterator pos)
        {
            typedef typename Rep_type::iterator Rep_iterator;
            _M_t.erase( reinterpret_cast<Rep_iterator&>(pos) );
        }

        size_type erase(const key_type& k)
        { return _M_t.erase(k); }

        void clear() { _M_t.clear(); }

        iterator find(const key_type& k) const
        { return _M_t.find(k); }

        size_type count(const key_type& k) const
        { return _M_t.count(k); }

        iterator lower_bound(const key_type& __x) const {
            return _M_t.lower_bound(__x);
        }
        iterator upper_bound(const key_type& __x) const {
            return _M_t.upper_bound(__x);
        }
        pair<iterator,iterator> equal_range(const key_type& __x) const {
            return _M_t.equal_range(__x);
        }

        template <class _K1, class _C1, class _A1>
        friend bool operator== (const set<_K1,_C1,_A1>&, const set<_K1,_C1,_A1>&);
        template <class _K1, class _C1, class _A1>
        friend bool operator< (const set<_K1,_C1,_A1>&, const set<_K1,_C1,_A1>&);
    };

    template <class _Key, class _Compare, class _Alloc>
    inline bool operator==(const set<_Key,_Compare,_Alloc>& __x,
                           const set<_Key,_Compare,_Alloc>& __y)
    {
        return __x._M_t == __y._M_t;
    }

    template <class _Key, class _Compare, class _Alloc>
    inline bool operator<(const set<_Key,_Compare,_Alloc>& __x,
                          const set<_Key,_Compare,_Alloc>& __y)
    {
        return __x._M_t < __y._M_t;
    }


} // namespace MYSTL


#endif //MEMORY_MANAGE_SET_H
