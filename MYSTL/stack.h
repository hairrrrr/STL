//
// Created by 78172 on 2023/4/5.
//

#ifndef MEMORY_MANAGE_STACK_H
#define MEMORY_MANAGE_STACK_H

//#include<deque>

namespace MYSTL
{

    template<typename Tp, typename Sequence = std::deque<Tp>>
    class stack;

    template<typename Tp, typename Sequence>
    class stack
    {
        template<typename Tp1, typename Sequence1>
        friend bool operator==(const stack<Tp1, Sequence1>&,
                               const stack<Tp1, Sequence1>&);
        template<typename Tp1, typename Sequence1>
        friend bool operator< (const stack<Tp1, Sequence1>&,
                               const stack<Tp1, Sequence1>&);

    public:
        typedef typename Sequence::value_type      value_type;
        typedef typename Sequence::size_type       size_type;
        typedef          Sequence                  container_type;
        typedef typename Sequence::reference       reference;
        typedef typename Sequence::const_reference const_reference;

    protected:
        Sequence c;

    public:
        stack() = default;
        explicit stack(const Sequence& s) : c(s) {}

        bool empty() const { return c.empty(); }
        size_type size() const  { return c.size();  }

        reference top() { return c.back(); }
        const_reference top() const { return c.back(); }

        void push(const Tp& x) { c.push_back(x); }
        void pop() { c.pop_back(); }
    };

    template<typename Tp1, typename Sequence1>
    bool operator==(const stack<Tp1, Sequence1>& s1, const stack<Tp1, Sequence1>& s2)
    {
        return s1.c == s2.c;
    }
    template<typename Tp1, typename Sequence1>
    bool operator< (const stack<Tp1, Sequence1>& s1, const stack<Tp1, Sequence1>& s2)
    {
        return s1.c < s2.c;
    }

} // namespace MYSTL

#endif //MEMORY_MANAGE_STACK_H
