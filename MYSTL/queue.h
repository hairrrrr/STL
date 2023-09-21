//
// Created by 78172 on 2023/4/5.
//

#ifndef MEMORY_MANAGE_QUEUE_H
#define MEMORY_MANAGE_QUEUE_H

#include"vector.h"
#include"function.h"
#include"heap.h"
#include<deque>

namespace MYSTL
{

    template <class Tp,
            class Sequence = std::deque<Tp> >
    class queue;

    template <class Tp, class Seq>
    inline bool operator==(const queue<Tp, Seq>&, const queue<Tp, Seq>&);

    template <class Tp, class Seq>
    inline bool operator<(const queue<Tp, Seq>&, const queue<Tp, Seq>&);

    template <class Tp, class Sequence>
    class queue
    {
        template <class _Tp1, class _Seq1>
        friend bool operator== (const queue<_Tp1, _Seq1>&,
                                const queue<_Tp1, _Seq1>&);
        template <class _Tp1, class _Seq1>
        friend bool operator< (const queue<_Tp1, _Seq1>&,
                               const queue<_Tp1, _Seq1>&);

    public:
        typedef typename Sequence::value_type      value_type;
        typedef typename Sequence::size_type       size_type;
        typedef          Sequence                  container_type;

        typedef typename Sequence::reference       reference;
        typedef typename Sequence::const_reference const_reference;
    protected:
        Sequence c;  // 底层容器

    public:
        queue() : c() {}
        explicit queue(const Sequence& __c) : c(__c) {}

        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }

        reference front() { return c.front(); }  // 返回首部元素
        const_reference front() const { return c.front(); }

        reference back() { return c.back(); }  // 返回尾部元素
        const_reference back() const { return c.back(); }

        void push(const value_type& __x) { c.push_back(__x); }  // 尾部进入元素
        void pop() { c.pop_front(); }  // 首部弹出元素
    };

    // 两个 queue 比较
    template <class _Tp, class _Sequence>
    bool
    operator==(const queue<_Tp, _Sequence>& __x, const queue<_Tp, _Sequence>& __y)
    {
        return __x.c == __y.c;
    }

    template <class _Tp, class _Sequence>
    bool
    operator<(const queue<_Tp, _Sequence>& __x, const queue<_Tp, _Sequence>& __y)
    {
        return __x.c < __y.c;
    }

} // namespace MYSTL


namespace MYSTL
{

    template <class Tp,
              class Sequence = MYSTL::vector<Tp>,
              class Compare = MYSTL::less<typename Sequence::value_type> >
    class priority_queue
    {

    public:
        typedef typename Sequence::value_type      value_type;
        typedef typename Sequence::size_type       size_type;
        typedef          Sequence                  container_type;

        typedef typename Sequence::reference       reference;
        typedef typename Sequence::const_reference const_reference;

    protected:
        Sequence c;  // 底层容器
        Compare comp; // 元素大小比较标准

    public:

        priority_queue() = default;

        explicit priority_queue(const Compare& cmp) : c(), comp(cmp) {}

        priority_queue(const Compare& cmp, const Sequence& s)
            : c(), comp(cmp)
        { make_heap(s.begin(), s.end(), cmp); }

        template <class InputIterator>
        priority_queue(InputIterator __first, InputIterator __last)
                : c(__first, __last)
        { make_heap(c.begin(), c.end(), comp); }

        template <class InputIterator>
        priority_queue(InputIterator __first,
                       InputIterator __last, const Compare& __x)
                : c(__first, __last), comp(__x)
        { make_heap(c.begin(), c.end(), comp); }


        /*
         * Big Five 除了 构造函数其余都设为 default
         * 让底层容器 c 代替我们处理
         */
        priority_queue(const priority_queue&) = default;
        priority_queue& operator=(const priority_queue&) = default;
        priority_queue& operator=(priority_queue&&) = default;
        ~priority_queue() = default;

    public:
        bool empty() const { return c.empty(); }
        size_type size() const { return c.size(); }
        const_reference top() const { return c.front(); } // 获得头部元素

        void push(const Tp& x)
        {
            try {
                c.push_back(x);
                push_heap(c.begin(), c.end(), comp);
            } catch (...) {
                c.clear();
            }
        }

        void pop()
        {
            try {
                pop_heap(c.begin(), c.end(). comp);
                c.pop_back();
            } catch(...) {
                c.clear();
            }
        }
    };

} // namespace MYSTL

#endif //MEMORY_MANAGE_QUEUE_H
