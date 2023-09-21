//
// Created by 78172 on 2023/4/2.
//

#ifndef MEMORY_MANAGE_ITERATOR_H
#define MEMORY_MANAGE_ITERATOR_H

#include"iterator_base.h"

namespace MYSTL
{

    template<typename Iterator>
    class reverse_iterator
    {
    protected:
        Iterator _current;
    public:
        typedef typename iterator_traits<Iterator>::value_type value_type;
        typedef typename iterator_traits<Iterator>::reference reference;
        typedef typename iterator_traits<Iterator>::difference_type difference_type;
        typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
        typedef typename iterator_traits<Iterator>::pointer pointer;

        typedef Iterator iterator_type;
        typedef reverse_iterator<Iterator> Self;

    public:

        reverse_iterator() = default;
        explicit reverse_iterator(iterator_type it) : _current(it) {}
        reverse_iterator(const Self& rit) : _current(rit._current) {}

        template<typename Iter>
        reverse_iterator(const reverse_iterator<Iter>& other)
            :_current(other.base()) {}

        iterator_type base() const { return _current; }

        reference operator*() const
        {
            Iterator it = _current;
            return *(--it);
        }

        pointer operator->() const
        { return &operator*(); }

        Self& operator++()
        {
            --_current;
            return *this;
        }

        Self operator++(int)
        {
            Self tmp(*this);
            --_current;
            return tmp;
        }

        Self& operator--()
        {
            ++_current;
            return *this;
        }

        Self operator--(int)
        {
            Self tmp(*this);
            ++_current;
            return tmp;
        }

        Self operator+(difference_type n)
        { return tmp(_current - n); }

        Self& operator+=(difference_type n)
        {
            _current -= n;
            return *this;
        }

        Self operator-(difference_type n)
        { return tmp(_current + n); }

        Self& operator-=(difference_type n)
        {
            _current += n;
            return *this;
        }

        reference operator[](difference_type n)
        { return *(*this + n); }
    };

    template<typename Iter>
    bool inline operator==(const reverse_iterator<Iter>& lhs,
                           const reverse_iterator<Iter>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template<typename Iter>
    bool inline operator<(const reverse_iterator<Iter>& lhs,
                           const reverse_iterator<Iter>& rhs)
    {
        return lhs.base() < rhs.base();
    }

    template<typename Iter>
    bool inline operator!=(const reverse_iterator<Iter>& lhs,
                           const reverse_iterator<Iter>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename Iter>
    bool inline operator>(const reverse_iterator<Iter>& lhs,
                           const reverse_iterator<Iter>& rhs)
    {
        return !(lhs < rhs) && !(lhs == rhs);
    }

    template<typename Iter>
    bool inline operator<=(const reverse_iterator<Iter>& lhs,
                           const reverse_iterator<Iter>& rhs)
    {
        return (lhs < rhs) || (lhs == rhs);
    }

    template<typename Iter>
    bool inline operator>=(const reverse_iterator<Iter>& lhs,
                           const reverse_iterator<Iter>& rhs)
    {
        return (lhs > rhs) || (lhs == rhs);
    }


    template <class _Container>
    class insert_iterator {
    protected:
        _Container* container;
        typename _Container::iterator iter;
    public:
        typedef _Container          container_type;
        typedef output_iterator_tag iterator_category;
        typedef void                value_type;
        typedef void                difference_type;
        typedef void                pointer;
        typedef void                reference;

        insert_iterator(_Container& __x, typename _Container::iterator __i)
                : container(&__x), iter(__i) {}

        insert_iterator<_Container>&
        operator=(const typename _Container::value_type& __value)
        {
            iter = container->insert(iter, __value);
            ++iter;
            return *this;
        }

        insert_iterator<_Container>& operator*() { return *this; }
        insert_iterator<_Container>& operator++() { return *this; }
        insert_iterator<_Container>& operator++(int) { return *this; }
    };

    template <class _Container, class _Iterator>
    inline
    insert_iterator<_Container> inserter(_Container& __x, _Iterator __i)
    {
        typedef typename _Container::iterator __iter;
        return insert_iterator<_Container>(__x, __iter(__i));
    }

} // namespace MYSTL

#endif //MEMORY_MANAGE_ITERATOR_H
