//
// Created by 78172 on 2023/4/2.
//

#ifndef MEMORY_MANAGE_LIST_H
#define MEMORY_MANAGE_LIST_H

#include"uninitialized.h"
#include"iterator.h"
#include"type_traits.h"
#include "algobase.h"

namespace MYSTL
{


struct _List_node_base
{
    _List_node_base* _M_next;
    _List_node_base* _M_prev;
};


template<typename Tp>
struct _List_node : _List_node_base
{
    Tp _M_data;
};


struct _List_iterator_base
{
    typedef size_t                     size_type;
    typedef ptrdiff_t                  difference_type;
    typedef bidirectional_iterator_tag iterator_category;

    _List_node_base* _M_node;

    _List_iterator_base(_List_node_base* node) : _M_node(node) {}
    _List_iterator_base() = default;

    void _M_incr() { _M_node = _M_node->_M_next; }
    void _M_decr() { _M_node = _M_node->_M_prev; }

    bool operator==(const _List_iterator_base& x) const
    { return _M_node == x._M_node; }

    bool operator!=(const _List_iterator_base& x) const
    { return _M_node != x._M_node; }
};


template<typename Tp, typename Ref, typename Ptr>
struct _List_iterator : _List_iterator_base
{
    typedef _List_iterator<Tp, Ref, Ptr> iterator;
    typedef _List_iterator<Tp, const Ref, const Ptr> const_iterator;
    typedef _List_iterator<Tp, Ref, Ptr> Self;

    typedef Tp  value_type;
    typedef Ptr pointer;
    typedef Ref reference;
    typedef _List_node<Tp> Node;

    _List_iterator(Node* node) : _List_iterator_base(node) {}
    _List_iterator() = default;
    _List_iterator(const _List_iterator& x) : _List_iterator_base(x._M_node) {}

    reference operator*()  const { return ((Node*)_M_node)->_M_data; }

    pointer operator->() const { return &(operator*()); }
//        reference operator->() const { return &(*(*this)); }

    Self& operator++()
    {
        this->_M_incr();
        return *this;
    }

    Self operator++(int)
    {
        Self tmp(*this);
        _M_incr();
        return tmp;
    }

    Self& operator--()
    {
        _M_decr();
        return *this;
    }

    Self operator--(int)
    {
        Self tmp(*this);
        _M_decr();
        return tmp;
    }
};

template<typename Tp, typename Alloc>
class _List_base
{
public:
    typedef Alloc allocator_type;
    allocator_type get_allocator() const { return allocator_type(); }

    _List_base()
    {
        _M_node = _M_get_node();
        _M_node->_M_next = static_cast<list_node_base*>(_M_node);
        _M_node->_M_prev = static_cast<list_node_base*>(_M_node);
    }

    ~_List_base()
    {
        clear();
        _M_put_node(_M_node);
    }

    void clear();

protected:
    typedef _List_node_base list_node_base;
    typedef _List_node<Tp>  list_node;
    typedef _List_node<Tp>* link_type;
    typedef MYSTL::allocator<list_node, Alloc> alloc_type;

    link_type _M_get_node() { return alloc_type().allocate(1); }
    void _M_put_node(link_type ptr) { alloc_type().deallocate(ptr, 1); }

protected:
    link_type _M_node;
};

    template<typename Tp, typename Alloc>
    void _List_base<Tp, Alloc>::clear()
    {
        link_type cur = reinterpret_cast<link_type>(_M_node->_M_next);
        while( cur != _M_node )
        {
            link_type nxt = reinterpret_cast<link_type>(cur->_M_next);
            Destroy(&cur->_M_data);
            _M_put_node(cur);
            cur = nxt;
        }

        // 回复头节点初始状态
        _M_node->_M_next = static_cast<list_node_base*>(_M_node);
        _M_node->_M_prev = static_cast<list_node_base*>(_M_node);
    }


    template<typename Tp, typename Alloc = std::allocator<_List_node<Tp>>>
    class list  : protected _List_base<Tp, std::allocator<_List_node<Tp>>>
    {
        typedef _List_base<Tp, std::allocator<_List_node<Tp>>> Base;

    protected:
        typedef void* Void_pointer;

    public:
        typedef       Tp                value_type;
        typedef       value_type*       pointer;
        typedef const value_type*       const_pointer;
        typedef       value_type&       reference;
        typedef const value_type&       const_reference;
        typedef       _List_node<Tp>    Node;
        typedef       size_t            size_type;
        typedef       ptrdiff_t         difference_type;

        typedef typename Base::allocator_type allocator_type;
        allocator_type get_allocator() { return Base::get_allocator(); }

    public:
        typedef _List_iterator<Tp, Tp&, Tp*>              iterator;
        typedef _List_iterator<Tp, const Tp&, const Tp*>  const_iterator;
        typedef reverse_iterator<const_iterator>          const_reverse_iterator;
        typedef reverse_iterator<iterator>                reverse_iterator;

        typedef _List_node_base Node_base;

    protected:
        using Base::_M_node;
        using Base::_M_get_node;
        using Base::_M_put_node;

    protected:
        Node* _M_create_node(const Tp& x = Tp())
        {
            Node* node = _M_get_node();
            try {
                construct(&node->_M_data, x);
            } catch(...) {
                _M_put_node(node);
            }
            return node;
        }

    public:

        list() = default;

        iterator       begin()        { return reinterpret_cast<Node*>(_M_node->_M_next); }
        const_iterator begin() const  { return reinterpret_cast<Node*>(_M_node->_M_next); }

        iterator       end()       { return _M_node; }
        const_iterator end() const { return _M_node; }

        reverse_iterator       rbegin()       { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

        reverse_iterator       rend()
        { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const
        { return const_reverse_iterator(begin()); }

        bool empty() const { return _M_node->_M_next == _M_node; }

        size_type size() const
        {
            size_type n = 0;
            distance(begin(), end(), n);
            return n;
        }

        size_type max_size() const { return size_type(-1); }

        reference       front()       { return *begin(); }
        const_reference front() const { return *begin(); }

        reference back()             { return *(--end()); }
        const_reference back() const { return *(--end()); }

        void swap(list& x) { std::swap(_M_node, x._M_node); }


        /*-----------------------------------
         * insert 函数
         *-----------------------------------*/

        /**
         * @param pos 在 pos 位置插入元素
         * @param val
         * @return 返回新插入的元素的迭代器
         */
        iterator insert(iterator pos, const Tp& val = Tp())
        {
            Node* node = _M_create_node(val);
            Node_base* position = pos._M_node;
            node->_M_next  = position;
            node->_M_prev  = position->_M_prev;
            position->_M_prev->_M_next = static_cast<Node_base*>(node);
            position->_M_prev = static_cast<Node_base*>(node);
            return node;
        }

        /**
         * 两个版本：
         * 1. 将 [first, last) 区间插入 pos 前
         * 2. 将 first 个 last 插入 pos 前
         */
        template<typename InputIterator>
        void insert(iterator pos, InputIterator first, InputIterator last)
        {
            typedef typename _Is_integer<InputIterator>::_Integral integral;
            _M_insert_dispatch(pos, first, last, integral());
        }

        void insert(iterator pos, size_type n, const Tp& val)
        { _M_fill_insert(pos, n, val); }

        void push_front(const Tp& val = Tp()) { insert(begin(), val); }
        void push_back (const Tp& val = Tp()) { insert(end(), val); }

        iterator erase(iterator pos)
        {
            Node_base* node_base_pos = pos._M_node;

            node_base_pos->_M_prev->_M_next = node_base_pos->_M_next;
            node_base_pos->_M_next->_M_prev = node_base_pos->_M_prev;

            Node* node_pos = reinterpret_cast<Node*>(node_base_pos);
            Node* res = reinterpret_cast<Node*>(node_base_pos->_M_next);

            destroy(&node_pos->_M_data);
            _M_put_node(node_pos);

            return res;
        }

        iterator erase(iterator first, iterator last)
        {
            while(first != last)
                erase(first++);
            return last;
        }

        void clear() { Base::clear(); }

        void resize(size_type n, const Tp& val = Tp())
        {
            size_type i = n;
            iterator it = begin();
            while(i && it != end()) --i, ++it;
            if(i)
                insert(end(), i, val);
            else
                erase(it, end());
        }

        void pop_back()  { erase(--end()); }
        void pop_front() { erase(begin()); }

        /*************************
         * 构造函数
         */
        list(size_type n, const Tp& val)
        { insert(begin(), n, val); }

        list(const Tp* first, const Tp* last)
        { insert(begin(), first, last); }

        list(const_iterator first, const_iterator last)
        { insert(begin(), first, last); }

        list(const list& x)
        { insert(begin(), x.begin(), x.end()); }

        list(list&& x)
        { this->swap(x); }

        list& operator=(list x)
        {
            this->swap(x);
            return *this;
        }


    protected:
        /*
         * 将 [first, last) 内的所有元素移动到 position 之前
         */
        void transfer(iterator pos, iterator first, iterator last)
        {
            if(pos != last)
            {
                Node_base *node_base_pos = pos._M_node;
                Node_base *node_base_first = first._M_node;
                Node_base *node_base_last = last._M_node;

                node_base_pos->_M_prev->_M_next = node_base_first;
                node_base_first->_M_prev->_M_next = node_base_last;
                node_base_last->_M_prev->_M_next = node_base_pos;

                Node_base *pos_prev = node_base_pos->_M_prev;
                node_base_pos->_M_prev = node_base_last->_M_prev;
                node_base_last->_M_prev = node_base_first->_M_prev;
                node_base_first->_M_prev = pos_prev;
            }
        }

    public:
        // 将 __x 接合于 position 所指位置之前，__x 必须不同于 *this
        void splice(iterator pos, list& x)
        {
            if(!x.empty())
                transfer(pos, x.begin(), x.end());
        }

        // 将 i 所指元素接合于 __position 所指位置之前，__position 和 i 可指向同一个 list
        void splice(iterator pos, list&, iterator i)
        {
            iterator j = i;
            ++j;
            if(pos == i || pos == j) return;
            transfer(pos, i, j);
        }

        // 将 [first, last) 内所有元素接合于 __position 所指位置之前
        void splice(iterator pos, list&, iterator first, iterator last)
        {
            if(first != last)
                transfer(pos, first, last);
        }

        void remove(const Tp& val)
        {
            iterator cur = begin();
            while(cur != end())
            {
                if(*cur == val)
                    cur = erase(cur);
                else
                    ++cur;
            }
        }

        void reverse()
        {
            Node_base* cur = _M_node;
            do
            {
                std::swap(cur->_M_next, cur->_M_prev);
                cur = cur->_M_prev;
            } while(cur != _M_node);
        }

        // 移除数值相同的连续元素。
        void unique()
        {
            iterator first = begin(), last = end();
            if(first == last) return;
            iterator next = first;
            while(++next != last)
            {
                if(*next == *first)
                    erase(next);
                else
                    first = next;
                next = first;
            }
        }

        void merge(list& rhs)
        {
            iterator first1 = begin(), first2 = rhs.begin();
            iterator last1 = end(), last2 = rhs.end();
            while(first1 != last1 && first2 != last2)
            {
                if(*first1 > *first2)
                {
                    iterator next = first2;
                    transfer(first1, first2, ++next);
                    first2 = next;
                }
                else
                    ++first1;
            }
            if(first2 != last2)
                transfer(last1, first2, last2);
        }

        void sort()
        {
            // 如果元素个数小于 2，不进行操作
            if(_M_node->_M_next == _M_node || _M_node->_M_next->_M_next == _M_node) return;

            list carry;
            list counter[64];
            int fill = 0;
            while(!empty())
            {
                carry.splice(carry.begin(), *this, begin());
                int i = 0;
                while(i < fill && !counter[i].empty())
                {
                    carry.merge(counter[i]);
                    i++;
                }
                counter[i].swap(carry);
                // counter[i] 已经存有元素了，fill 扩大
                if(i == fill) fill++;
            }
            // 元素个数至少为 2，所以一定从 counter[1] 开始有链表
            for(int i = 2; i < fill; ++i)
                counter[i].merge(counter[i - 1]);
            this->swap(counter[fill - 1]);
        }

    private:
        template<typename InputIterator>
        void _M_insert_dispatch (iterator pos, InputIterator first,
                                 InputIterator last, __false_type);

        template<typename Integer>
        iterator _M_insert_dispatch (iterator pos, Integer n,
                                     Integer val, __true_type)
        { _M_fill_insert(pos, n, val); }

        void _M_fill_insert(iterator pos, size_type n, const Tp& x)
        { while(n--) insert(pos, x); }
    };

    template<typename Tp, typename Alloc>
    template<typename InputIterator>
    void
    list<Tp, Alloc>::_M_insert_dispatch(iterator pos, InputIterator first,
                                        InputIterator last, __false_type)
    {
        // 始终往 pos 前插入
        for(; first != last; ++first)
            insert(pos, *first);
    }
}

#endif //MEMORY_MANAGE_LIST_H








