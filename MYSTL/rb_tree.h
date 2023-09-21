//
// Created by 78172 on 2023/4/7.
//

#ifndef MEMORY_MANAGE_RB_TREE_H
#define MEMORY_MANAGE_RB_TREE_H

#include"algo.h"
#include"allocator.h"
#include"iterator.h"
#include"function.h"
#include"pair.h"
#include"queue.h"

namespace MYSTL
{
    typedef bool _Rb_tree_Color_type;
    const _Rb_tree_Color_type _S_rb_tree_red = false; // 红色为 0
    const _Rb_tree_Color_type _S_rb_tree_black = true; // 黑色为 1

    struct _Rb_tree_node_base
    {
        typedef _Rb_tree_Color_type _Color_type;
        typedef _Rb_tree_node_base*  _Base_ptr;

        _Color_type _M_color;
        _Base_ptr   _M_parent;
        _Base_ptr   _M_left;
        _Base_ptr   _M_right;

        static _Base_ptr _S_minimum(_Base_ptr x)
        {
            while( x->_M_left ) x = x->_M_left;
            return x;
        }

        static _Base_ptr _S_maximum(_Base_ptr x)
        {
            while( x->_M_right ) x = x->_M_right;
            return x;
        }
    };


    template<typename Value>
    struct _Rb_tree_node : public _Rb_tree_node_base
    {
        typedef _Rb_tree_node<Value>* _Link_type;
        Value _M_value_field;
    };


    struct _Rb_tree_base_iterator
    {
        typedef  _Rb_tree_node_base::_Base_ptr  _Base_ptr;
        typedef  bidirectional_iterator_tag     iterator_category;
        typedef  ptrdiff_t                      difference_type;

        _Base_ptr  _M_node;

        void _M_increment()
        {
            // 存在右子树，找到右子树的最左节点
            if( _M_node->_M_right )
            {
                _M_node = _M_node->_M_right;
                while( _M_node->_M_left )
                    _M_node = _M_node->_M_left;
            }
            else
            {
                _Base_ptr parent = _M_node->_M_parent;
                // 如果当前节点为父节点的右子树则一直向上走
                while( _M_node == parent->_M_right )
                {
                    _M_node = parent;
                    parent = parent->_M_parent;
                }

                // 满足这个条件时 _M_node == end() 也就是 header
                if( _M_node->_M_right != parent )
                    _M_node = parent;
            }
        }

        void _M_decrement()
        {
            // 当 _M_node 为 end 时
            if( (_M_node->_M_color == _S_rb_tree_red) &&
                (_M_node->_M_parent->_M_parent == _M_node) )
                _M_node = _M_node->_M_right;
            // 存在左子树，找到左子树的最右节点
            else if( _M_node->_M_left )
            {
                _M_node = _M_node->_M_left;
                while( _M_node->_M_right )
                    _M_node = _M_node->_M_right;
            }
            // 如果当前节点为父节点的左子树则一直向上走
            else
            {
                _Base_ptr parent = _M_node->_M_parent;
                while( _M_node == parent->_M_left )
                {
                    _M_node = parent;
                    parent = parent->_M_parent;
                }
                _M_node = parent;
            }
        }

    };

    template <class _Value, class _Ref, class _Ptr>
    struct _Rb_tree_iterator : public _Rb_tree_base_iterator
    {
        typedef _Value  value_type;
        typedef _Ref    reference;
        typedef _Ptr    pointer;

        typedef _Rb_tree_iterator<_Value, _Value&, _Value*>              iterator;
        typedef _Rb_tree_iterator<_Value, const _Value&, const _Value*>  const_iterator;

        typedef _Rb_tree_iterator<_Value, _Ref, _Ptr>  _Self;
        typedef _Rb_tree_node<_Value>*                 _Link_type;

        _Rb_tree_iterator() {}
        _Rb_tree_iterator(_Link_type __x)       { _M_node = __x; }
        _Rb_tree_iterator(const iterator& __it) { _M_node = __it._M_node; }

        reference operator*() const
        { return reinterpret_cast<_Link_type>(_M_node)->_M_value_field; }

        pointer operator->() const { return &(operator*()); }

        // RB-tree 迭代器 ++，-- 操作
        _Self& operator++()
        {
            _M_increment();
            return *this;
        }

        _Self operator++(int)
        {
            _Self __tmp = *this;
            _M_increment();
            return __tmp;
        }

        _Self& operator--()
        {
            _M_decrement();
            return *this;
        }

        _Self operator--(int)
        {
            _Self __tmp = *this;
            _M_decrement();
            return __tmp;
        }
    };

    inline bool operator==(const _Rb_tree_base_iterator& __x,
                           const _Rb_tree_base_iterator& __y)
    {
        return __x._M_node == __y._M_node;
    }

    inline bool operator!=(const _Rb_tree_base_iterator& __x,
                           const _Rb_tree_base_iterator& __y)
    {
        return __x._M_node != __y._M_node;
    }


    inline void _Rb_tree_rotate_left(_Rb_tree_node_base* node,
                                     _Rb_tree_node_base*& root)
    {
        _Rb_tree_node_base* right  = node->_M_right;
        _Rb_tree_node_base* parent = node->_M_parent;

        // 如果 right 存在左子树
        if( (node->_M_right = right->_M_left) )
            right->_M_left->_M_parent = node;
        right->_M_left = node;

        right->_M_parent = parent;

        // 如果 node 是根节点
        if( node == root )
            root = right;
        // node 是 parent 的左子树
        else if( node == parent->_M_left )
            parent->_M_left = right;
        else
            parent->_M_right = right;

        node->_M_parent = right;
    }

    inline void _Rb_tree_rotate_right(_Rb_tree_node_base* node,
                                     _Rb_tree_node_base*& root)
    {
        _Rb_tree_node_base* left  = node->_M_left;
        _Rb_tree_node_base* parent = node->_M_parent;

        if(( node->_M_left = left->_M_right ))
            left->_M_right->_M_parent = node;

        left->_M_right  = node;
        left->_M_parent = parent;

        if( node == root )
            root = left;
        else if( node == parent->_M_left )
            parent->_M_left  = left;
        else
            parent->_M_right = left;

        node->_M_parent = left;
    }


    inline void _Rb_tree_rebalance(_Rb_tree_node_base* node,
                                   _Rb_tree_node_base*& root)
    {
        node->_M_color = _S_rb_tree_red;

        //  node 不为根节点，node 的父节点为红色
        while( (node != root) && (node->_M_parent->_M_color == _S_rb_tree_red) )
        {
            _Rb_tree_node_base* parent = node->_M_parent;
            _Rb_tree_node_base* gparent = parent->_M_parent;

            if( parent == gparent->_M_left )
            {
                _Rb_tree_node_base* uncle = gparent->_M_right;

                // uncle 存在且为红色
                if( uncle && (uncle->_M_color == _S_rb_tree_red) )
                {
                    parent->_M_color  = _S_rb_tree_black;
                    uncle->_M_color   = _S_rb_tree_black;
                    gparent->_M_color = _S_rb_tree_red;
                    // 继续向上检查
                    node = gparent;
                }
                // uncle 不存在或为黑色
                else
                {
                    // 以 parent 为根，左旋
                    if( node == parent->_M_right )
                    {
                        _Rb_tree_rotate_left(parent, root);
                        std::swap(node, parent);
                    }

                    // 以 gparent 为根，右旋
                    parent->_M_color  = _S_rb_tree_black;
                    gparent->_M_color = _S_rb_tree_red;
                    _Rb_tree_rotate_right(gparent, root);
                }
            }
            else
            {
                _Rb_tree_node_base* uncle = gparent->_M_left;

                if( uncle && (uncle->_M_color == _S_rb_tree_red) )
                {
                    parent->_M_color  = _S_rb_tree_black;
                    uncle->_M_color   = _S_rb_tree_black;
                    gparent->_M_color = _S_rb_tree_red;
                    node = gparent;
                }
                else
                {
                    if( node == parent->_M_left )
                    {
                        _Rb_tree_rotate_right(parent, root);
                        std::swap(node, parent);
                    }

                    parent->_M_color  = _S_rb_tree_black;
                    gparent->_M_color = _S_rb_tree_red;
                    _Rb_tree_rotate_left(gparent, root);
                }
            }
        }
        // 根节点永远为黑色
        root->_M_color = _S_rb_tree_black;
    }

    // 删除一个节点后调整
    inline _Rb_tree_node_base*
    _Rb_tree_rebalance_for_erase(_Rb_tree_node_base* __z,
                                 _Rb_tree_node_base*& __root,
                                 _Rb_tree_node_base*& __leftmost,
                                 _Rb_tree_node_base*& __rightmost)
    {
        _Rb_tree_node_base* __y = __z;
        _Rb_tree_node_base* __x = 0;
        _Rb_tree_node_base* __x_parent = 0;
        if (__y->_M_left == 0)     // __z has at most one non-null child. y == z.
            __x = __y->_M_right;     // __x might be null.
        else
        if (__y->_M_right == 0)  // __z has exactly one non-null child. y == z.
            __x = __y->_M_left;    // __x is not null.
        else {                   // __z has two non-null children.  Set __y to
            __y = __y->_M_right;   //   __z's successor.  __x might be null.
            while (__y->_M_left != 0)
                __y = __y->_M_left;
            __x = __y->_M_right;
        }
        if (__y != __z) {          // relink y in place of z.  y is z's successor
            __z->_M_left->_M_parent = __y;
            __y->_M_left = __z->_M_left;
            if (__y != __z->_M_right) {
                __x_parent = __y->_M_parent;
                if (__x) __x->_M_parent = __y->_M_parent;
                __y->_M_parent->_M_left = __x;      // __y must be a child of _M_left
                __y->_M_right = __z->_M_right;
                __z->_M_right->_M_parent = __y;
            }
            else
                __x_parent = __y;
            if (__root == __z)
                __root = __y;
            else if (__z->_M_parent->_M_left == __z)
                __z->_M_parent->_M_left = __y;
            else
                __z->_M_parent->_M_right = __y;
            __y->_M_parent = __z->_M_parent;
            std::swap(__y->_M_color, __z->_M_color);
            __y = __z;
            // __y now points to node to be actually deleted
        }
        else {                        // __y == __z
            __x_parent = __y->_M_parent;
            if (__x) __x->_M_parent = __y->_M_parent;
            if (__root == __z)
                __root = __x;
            else
            if (__z->_M_parent->_M_left == __z)
                __z->_M_parent->_M_left = __x;
            else
                __z->_M_parent->_M_right = __x;
            if (__leftmost == __z)
                if (__z->_M_right == 0)        // __z->_M_left must be null also
                    __leftmost = __z->_M_parent;
                    // makes __leftmost == _M_header if __z == __root
                else
                    __leftmost = _Rb_tree_node_base::_S_minimum(__x);
            if (__rightmost == __z)
                if (__z->_M_left == 0)         // __z->_M_right must be null also
                    __rightmost = __z->_M_parent;
                    // makes __rightmost == _M_header if __z == __root
                else                      // __x == __z->_M_left
                    __rightmost = _Rb_tree_node_base::_S_maximum(__x);
        }
        if (__y->_M_color != _S_rb_tree_red) {
            while (__x != __root && (__x == 0 || __x->_M_color == _S_rb_tree_black))
                if (__x == __x_parent->_M_left) {
                    _Rb_tree_node_base* __w = __x_parent->_M_right;
                    if (__w->_M_color == _S_rb_tree_red) {
                        __w->_M_color = _S_rb_tree_black;
                        __x_parent->_M_color = _S_rb_tree_red;
                        _Rb_tree_rotate_left(__x_parent, __root);
                        __w = __x_parent->_M_right;
                    }
                    if ((__w->_M_left == 0 ||
                         __w->_M_left->_M_color == _S_rb_tree_black) &&
                        (__w->_M_right == 0 ||
                         __w->_M_right->_M_color == _S_rb_tree_black)) {
                        __w->_M_color = _S_rb_tree_red;
                        __x = __x_parent;
                        __x_parent = __x_parent->_M_parent;
                    } else {
                        if (__w->_M_right == 0 ||
                            __w->_M_right->_M_color == _S_rb_tree_black) {
                            if (__w->_M_left) __w->_M_left->_M_color = _S_rb_tree_black;
                            __w->_M_color = _S_rb_tree_red;
                            _Rb_tree_rotate_right(__w, __root);
                            __w = __x_parent->_M_right;
                        }
                        __w->_M_color = __x_parent->_M_color;
                        __x_parent->_M_color = _S_rb_tree_black;
                        if (__w->_M_right) __w->_M_right->_M_color = _S_rb_tree_black;
                        _Rb_tree_rotate_left(__x_parent, __root);
                        break;
                    }
                } else {                  // same as above, with _M_right <-> _M_left.
                    _Rb_tree_node_base* __w = __x_parent->_M_left;
                    if (__w->_M_color == _S_rb_tree_red) {
                        __w->_M_color = _S_rb_tree_black;
                        __x_parent->_M_color = _S_rb_tree_red;
                        _Rb_tree_rotate_right(__x_parent, __root);
                        __w = __x_parent->_M_left;
                    }
                    if ((__w->_M_right == 0 ||
                         __w->_M_right->_M_color == _S_rb_tree_black) &&
                        (__w->_M_left == 0 ||
                         __w->_M_left->_M_color == _S_rb_tree_black)) {
                        __w->_M_color = _S_rb_tree_red;
                        __x = __x_parent;
                        __x_parent = __x_parent->_M_parent;
                    } else {
                        if (__w->_M_left == 0 ||
                            __w->_M_left->_M_color == _S_rb_tree_black) {
                            if (__w->_M_right) __w->_M_right->_M_color = _S_rb_tree_black;
                            __w->_M_color = _S_rb_tree_red;
                            _Rb_tree_rotate_left(__w, __root);
                            __w = __x_parent->_M_left;
                        }
                        __w->_M_color = __x_parent->_M_color;
                        __x_parent->_M_color = _S_rb_tree_black;
                        if (__w->_M_left) __w->_M_left->_M_color = _S_rb_tree_black;
                        _Rb_tree_rotate_right(__x_parent, __root);
                        break;
                    }
                }
            if (__x) __x->_M_color = _S_rb_tree_black;
        }
        return __y;
    }


    template <class _Tp, class _Alloc>
    struct _Rb_tree_base
    {
        typedef _Alloc allocator_type;
        allocator_type get_allocator() const { return allocator_type(); }

        _Rb_tree_base() { _M_header = _M_get_node(); }
        ~_Rb_tree_base() { _M_put_node(_M_header); }

    protected:
        _Rb_tree_node<_Tp>* _M_header;

        typedef MYSTL::allocator<_Rb_tree_node<_Tp>, _Alloc> _Alloc_type;

        _Rb_tree_node<_Tp>* _M_get_node()
        { return _Alloc_type().allocate(1); }
        void _M_put_node(_Rb_tree_node<_Tp>* __p)
        { _Alloc_type().deallocate(__p, 1); }
    };

// _Rb_tree 数据结构
    template <class _Key, class _Value, class _KeyOfValue, class _Compare,
              class _Alloc = std::allocator<_Rb_tree_node<_Value> > >
    class _Rb_tree : protected _Rb_tree_base<_Value, _Alloc>
    {
        typedef _Rb_tree_base<_Value, _Alloc> _Base;
    protected:
        typedef _Rb_tree_node_base*    _Base_ptr;
        typedef _Rb_tree_node<_Value>  _Rb_tree_node;
        typedef _Rb_tree_Color_type    _Color_type;
    public:
        typedef       _Key            key_type;
        typedef       _Value          value_type;
        typedef       value_type*     pointer;
        typedef const value_type*     const_pointer;
        typedef       value_type&     reference;
        typedef const value_type&     const_reference;
        typedef       _Rb_tree_node*  _Link_type;
        typedef       size_t          size_type;
        typedef       ptrdiff_t       difference_type;

        typedef typename _Base::allocator_type allocator_type;
        allocator_type get_allocator() const { return _Base::get_allocator(); }

    protected:
        using _Base::_M_get_node;
        using _Base::_M_put_node;
        using _Base::_M_header;  // header，实现上的技巧

    protected:
        // 创建一个节点
        _Link_type _M_create_node(const value_type& __x)
        {
            _Link_type __tmp = _M_get_node(); // 配置空间
            try {
                construct(&__tmp->_M_value_field, __x); // 构造内容
            }
            catch(...) {
                _M_put_node(__tmp);
            }

            return __tmp;
        }

        // 复制一个节点，注意节点的数据结构
        _Link_type _M_clone_node(_Link_type __x)
        {
            _Link_type __tmp = _M_create_node(__x->_M_value_field);
            __tmp->_M_color = __x->_M_color;
            __tmp->_M_left  = nullptr;
            __tmp->_M_right = nullptr;
            return __tmp;
        }

        // 删除一个节点
        void destroy_node(_Link_type __p)
        {
            destroy(&__p->_M_value_field); // 析构内容
            _M_put_node(__p); // 释放空间
        }

    protected:
        size_type _M_node_count; // keeps track of size of tree 节点数量
        _Compare  _M_key_compare; // 节点间的键值大小比较准则

        // 利用 header 快速找到 RB-tree 最小值，最大值，根节点
        _Link_type& _M_root() const
        { return reinterpret_cast<_Link_type&>( _M_header->_M_parent ); }
        _Link_type& _M_leftmost() const
        { return reinterpret_cast<_Link_type&>( _M_header->_M_left ); }
        _Link_type& _M_rightmost() const
        { return reinterpret_cast<_Link_type&>( _M_header->_M_right ); }

        // 按理来说，下面的 C 风格强转都应该改为 reinterpret_cast ，太长了，不想改了

        // 获取 __x 的成员
        static _Link_type& _S_left(_Link_type __x)
        { return (_Link_type&)(__x->_M_left); }
        static _Link_type& _S_right(_Link_type __x)
        { return (_Link_type&)(__x->_M_right); }
        static _Link_type& _S_parent(_Link_type __x)
        { return (_Link_type&)(__x->_M_parent); }
        static reference _S_value(_Link_type __x)
        { return __x->_M_value_field; }
        static const _Key& _S_key(_Link_type __x)
        { return _KeyOfValue()(_S_value(__x)); }
        static _Color_type& _S_color(_Link_type __x)
        { return (_Color_type&)(__x->_M_color); }

        static _Link_type& _S_left(_Base_ptr __x)
        { return (_Link_type&)(__x->_M_left); }
        static _Link_type& _S_right(_Base_ptr __x)
        { return (_Link_type&)(__x->_M_right); }
        static _Link_type& _S_parent(_Base_ptr __x)
        { return (_Link_type&)(__x->_M_parent); }
        static reference _S_value(_Base_ptr __x)
        { return ((_Link_type)__x)->_M_value_field; }
        static const _Key& _S_key(_Base_ptr __x)
        { return _KeyOfValue()(_S_value(_Link_type(__x)));}
        static _Color_type& _S_color(_Base_ptr __x)
        { return (_Color_type&)(_Link_type(__x)->_M_color); }
        // 求最小值和最大值
        static _Link_type _S_minimum(_Link_type __x)
        { return (_Link_type)  _Rb_tree_node_base::_S_minimum(__x); }

        static _Link_type _S_maximum(_Link_type __x)
        { return (_Link_type) _Rb_tree_node_base::_S_maximum(__x); }

    public:
        typedef _Rb_tree_iterator<value_type, reference, pointer> iterator; // RB-tree 迭代器
        typedef _Rb_tree_iterator<value_type, const_reference, const_pointer>
                const_iterator;

        typedef reverse_iterator<const_iterator>  const_reverse_iterator;
        typedef reverse_iterator<iterator>        reverse_iterator;

    public:
        // allocation/deallocation
        _Rb_tree()
                :  _M_node_count(0), _M_key_compare()
        { _M_empty_initialize(); }

        _Rb_tree(const _Compare& __comp)
                :  _M_node_count(0), _M_key_compare(__comp)
        { _M_empty_initialize(); }

        // 初始化
        _Rb_tree(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x)
                : _M_node_count(0), _M_key_compare(__x._M_key_compare)
        {
            if (__x._M_root() == 0)
                _M_empty_initialize();
            else {
                _S_color(_M_header) = _S_rb_tree_red;  // header 的颜色为红色
                _M_root()      = _M_copy(__x._M_root(), _M_header); // 根节点的颜色为黑色
                _M_leftmost()  = _S_minimum(_M_root());
                _M_rightmost() = _S_maximum(_M_root());
            }
            _M_node_count = __x._M_node_count;
        }

        ~_Rb_tree() { clear(); }

        _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>&
        operator=(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x);

    private:
        void _M_empty_initialize()
        {
            _S_color(_M_header) = _S_rb_tree_red; // used to distinguish header from
            // __root, in iterator.operator++
            _M_root() = 0;
            _M_leftmost()  = _M_header;
            _M_rightmost() = _M_header;
        }

    private:
        iterator    _M_insert(_Base_ptr __x, _Base_ptr __y, const value_type& __v);
        _Link_type  _M_copy(_Link_type __x, _Link_type __p);
        void        _M_erase(_Link_type __x);

    public:
        // accessors:
        _Compare key_comp() const { return _M_key_compare; }
        iterator begin() { return _M_leftmost(); } // RB-tree 的起点为最左节点处
        const_iterator begin() const { return _M_leftmost(); }
        iterator end() { return _M_header; }  // RB-tree 的终点为 header 所指处
        const_iterator end() const { return _M_header; }
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const {
            return const_reverse_iterator(end());
        }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const {
            return const_reverse_iterator(begin());
        }
        bool empty() const { return _M_node_count == 0; }
        size_type size() const { return _M_node_count; }
        size_type max_size() const { return size_type(-1); }

        void swap(_Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __t)
        {
            std::swap(_M_header, __t._M_header);
            std::swap(_M_node_count, __t._M_node_count);
            std::swap(_M_key_compare, __t._M_key_compare);
        }

    public:
        // insert/erase
        pair<iterator,bool> insert_unique(const value_type& __x);
        iterator insert_equal(const value_type& __x);

        void erase(iterator __position);
        size_type erase(const key_type& __x);
        void erase(iterator __first, iterator __last);

        void clear() {
            if (_M_node_count != 0) {
                _M_erase(_M_root());
                _M_leftmost() = _M_header;
                _M_root() = 0;
                _M_rightmost() = _M_header;
                _M_node_count = 0;
            }
        }

    public:
        // set operations:
        iterator find(const key_type& __x);
        const_iterator find(const key_type& __x) const;
        size_type count(const key_type& __x) const;
        iterator lower_bound(const key_type& __x);
        const_iterator lower_bound(const key_type& __x) const;
        iterator upper_bound(const key_type& __x);
        const_iterator upper_bound(const key_type& __x) const;
        pair<iterator,iterator> equal_range(const key_type& __x);
        pair<const_iterator, const_iterator> equal_range(const key_type& __x) const;

    public:
        // Debugging.
        bool __rb_verify() const;

        void inorder_traversal() const
        {
            cout << "inOrder Traversal: [" << endl;
            _M_dfs(_M_header->_M_parent);
            cout << "]" << endl;
        }

        void level_traversal() const
        {
            cout << "Level Order Traversal------------" << endl;
            MYSTL::queue<_Rb_tree_node_base*> que;
            que.push(_M_header->_M_parent);
            while(que.size())
            {
                int n = que.size();
                while(n--)
                {
                    auto t = que.front(); que.pop();
                    if(t == nullptr)
                        cout << nullptr << " ";
                    else
                    {
                        cout << " [" << _S_value(t) << " | " << _S_color(t) << "] ";
                        que.push(t->_M_left);
                        que.push(t->_M_right);
                    }
                }
                cout << endl;
            }
            cout << "---------------finish" << endl;
        }

    private:
        void _M_dfs(_Rb_tree_node_base* root) const
        {
            if(!root) return;
            _M_dfs(root->_M_left);
            cout << _S_value(root) << endl;
            _M_dfs(root->_M_right);
        }
    };

    // p 拷贝 x
    template <class _Key, class _Val, class _KoV, class _Compare, class _Alloc>
    typename _Rb_tree<_Key, _Val, _KoV, _Compare, _Alloc>::_Link_type
    _Rb_tree<_Key,_Val,_KoV,_Compare,_Alloc>
    ::_M_copy(_Link_type __x, _Link_type __p)
    {
        // structural copy.  __x and __p must be non-null.
        _Link_type __top = _M_clone_node(__x);
        __top->_M_parent = __p;

        try {
                if (__x->_M_right)
                    __top->_M_right = _M_copy(_S_right(__x), __top);
                __p = __top;
                __x = _S_left(__x);

                while (__x != 0) {
                    _Link_type __y = _M_clone_node(__x);
                    __p->_M_left = __y;
                    __y->_M_parent = __p;
                    if (__x->_M_right)
                        __y->_M_right = _M_copy(_S_right(__x), __y);
                    __p = __y;
                    __x = _S_left(__x);
                }
        }
        catch(...) {
            _M_erase(__top);
        }

        return __top;
    }

    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    void _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
    ::_M_erase(_Link_type __x)
    {
        // erase without rebalancing
        while (__x != 0) {
            _M_erase(_S_right(__x));
            _Link_type __y = _S_left(__x);
            destroy_node(__x);
            __x = __y;
        }
    }

    // 赋值操作符
    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>&
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
    ::operator=(const _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>& __x)
    {
        if (this != &__x) {
            // Note that _Key may be a constant type.
            clear();
            _M_node_count = 0;
            _M_key_compare = __x._M_key_compare;
            if (__x._M_root() == 0) {
                _M_root() = 0;
                _M_leftmost() = _M_header;
                _M_rightmost() = _M_header;
            }
            else {
                _M_root() = _M_copy(__x._M_root(), _M_header);
                _M_leftmost() = _S_minimum(_M_root());
                _M_rightmost() = _S_maximum(_M_root());
                _M_node_count = __x._M_node_count;
            }
        }
        return *this;
    }



    template <class _Key, class _Value, class _KeyOfValue,
              class _Compare, class _Alloc>
    pair<typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator,bool>
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::
    insert_unique(const value_type& __x)
    {
        _Link_type parent = _M_header;
        _Link_type cur = _M_root();
        bool comp = true;
        while( cur != nullptr )
        {
            parent = cur;
//            cout << "_KeyOfValue()(__x): " << _KeyOfValue()(__x) << endl;
//            cout << "_S_value(cur)"  << _S_value(cur).first << " " << _S_value(cur).second << endl;
//            cout << "_S_key(cur) " << _S_key(cur) << endl;

            comp = _M_key_compare( _KeyOfValue()(__x), _S_key(cur) );
            cur =  comp ? _S_left(cur) : _S_right(cur);
        }

        iterator it(parent);
        if( comp )
        {
            // 如果父节点为最左节点，且当前节点更小，不可能重复
            if( it == begin() )
                return pair<iterator, bool>(_M_insert(cur, parent, __x), true);
            else
                --it;
        }

        // 如果中序遍历的前一个节点的值不小于 x，说明 x 与其重复
        if( _M_key_compare( _S_key( it._M_node ), _KeyOfValue()(__x) ) )
            return pair<iterator, bool>(_M_insert(cur, parent, __x), true);
        else
            return pair<iterator, bool>(it, false);
    }

    template <class _Key, class _Value, class _KeyOfValue,
              class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::
    insert_equal(const value_type& __x)
    {
        _Link_type parent = _M_header;
        _Link_type cur = _M_root();
        while( cur != nullptr )
        {
            parent = cur;
            cur =  _M_key_compare( _KeyOfValue()(__x), _S_key(cur) ) ?
                   _S_left(cur) : _S_right(cur);
        }

        return _M_insert(cur, parent, __x);
    }

    template <class _Key, class _Value, class _KeyOfValue,
              class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::
    _M_insert(_Base_ptr __x, _Base_ptr __y, const value_type& __v)
    {
        _Link_type cur = reinterpret_cast<_Link_type>(__x);
        _Link_type parent = reinterpret_cast<_Link_type>(__y);
        _Link_type node;

        if( ( parent == _M_header ) || cur != nullptr ||
              _M_key_compare( _KeyOfValue()(__v), _S_key( parent ) ) )
        {
            node = _M_create_node(__v);
            _S_left(parent) = node;
            if( parent == _M_header )
            {
                _M_root() = node;
                _M_rightmost() = node;
                // _M_leftmost() = node; // 上面 _S_left(parent) = node; 已经做了这件事
            }
            else if( parent == _M_leftmost() )
                _M_leftmost() = node;

        }
        else
        {
            node = _M_create_node(__v);
            _S_right(parent) = node;

            if( parent == _M_rightmost() )
                _M_rightmost() = node;
        }

        _S_parent(node) = parent;
        _S_left(node) = nullptr;
        _S_right(node) = nullptr;
        _Rb_tree_rebalance(node, _M_header->_M_parent);
        ++_M_node_count;

        return iterator(node);
    }


    template <class _Key, class _Value, class _KeyOfValue,
              class _Compare, class _Alloc>
    void
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::
    erase(iterator __position)
    {
        _Link_type del = reinterpret_cast<_Link_type>(
                _Rb_tree_rebalance_for_erase(__position._M_node,
                                             _M_header->_M_parent,
                                             _M_header->_M_left,
                                             _M_header->_M_right) );
        destroy_node(del);
    }

    template <class _Key, class _Value, class _KeyOfValue,
              class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::size_type
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::
    erase(const key_type& __x)
    {
        pair<iterator, iterator> range = equal_range(__x);
        size_type n = 0;
        distance(range.first, range.second, n);
        erase(range.first, range.second);
        return n;
    }

    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    void _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
    ::erase(iterator __first, iterator __last)
    {
        if (__first == begin() && __last == end())
            clear();
        else
            while (__first != __last) erase(__first++);
    }


    // find 查找
    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::find(const _Key& __k)
    {
        _Link_type __y = _M_header;      // Last node which is not less than __k.
        _Link_type __x = _M_root();      // Current node.

        // 找到中序遍历最右侧的值
        while (__x != 0)
            if (!_M_key_compare(_S_key(__x), __k))
                __y = __x, __x = _S_left(__x);
            else
                __x = _S_right(__x);

        iterator __j = iterator(__y);
        return (__j == end() || _M_key_compare(__k, _S_key(__j._M_node))) ?
               end() : __j;
    }

    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::const_iterator
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::find(const _Key& __k) const
    {
        _Link_type __y = _M_header; /* Last node which is not less than __k. */
        _Link_type __x = _M_root(); /* Current node. */

        while (__x != 0) {
            if (!_M_key_compare(_S_key(__x), __k))
                __y = __x, __x = _S_left(__x);
            else
                __x = _S_right(__x);
        }
        const_iterator __j = const_iterator(__y);
        return (__j == end() || _M_key_compare(__k, _S_key(__j._M_node))) ?
               end() : __j;
    }

    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::size_type
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
    ::count(const _Key& __k) const
    {
        pair<const_iterator, const_iterator> __p = equal_range(__k);
        size_type __n = 0;
        distance(__p.first, __p.second, __n);
        return __n;
    }


    // 第一个大于等于 k 的节点
    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
    ::lower_bound(const _Key& __k)
    {
        _Link_type __y = _M_header; /* Last node which is not less than __k. */
        _Link_type __x = _M_root(); /* Current node. */

        while (__x != 0)
            if (!_M_key_compare(_S_key(__x), __k))
                __y = __x, __x = _S_left(__x);
            else
                __x = _S_right(__x);

        return iterator(__y);
    }

    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::const_iterator
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
    ::lower_bound(const _Key& __k) const
    {
        _Link_type __y = _M_header; /* Last node which is not less than __k. */
        _Link_type __x = _M_root(); /* Current node. */

        while (__x != 0)
            if (!_M_key_compare(_S_key(__x), __k))
                __y = __x, __x = _S_left(__x);
            else
                __x = _S_right(__x);

        return const_iterator(__y);
    }

    // 第一个大于 k 的节点
    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
    ::upper_bound(const _Key& __k)
    {
        _Link_type __y = _M_header; /* Last node which is greater than __k. */
        _Link_type __x = _M_root(); /* Current node. */

        while (__x != 0)
            if (_M_key_compare(__k, _S_key(__x)))
                __y = __x, __x = _S_left(__x);
            else
                __x = _S_right(__x);

        return iterator(__y);
    }

    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::const_iterator
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
    ::upper_bound(const _Key& __k) const
    {
        _Link_type __y = _M_header; /* Last node which is greater than __k. */
        _Link_type __x = _M_root(); /* Current node. */

        while (__x != 0)
            if (_M_key_compare(__k, _S_key(__x)))
                __y = __x, __x = _S_left(__x);
            else
                __x = _S_right(__x);

        return const_iterator(__y);
    }

    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    inline
    pair<typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator,
            typename _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::iterator>
    _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>
    ::equal_range(const _Key& __k)
    {
        return pair<iterator, iterator>(lower_bound(__k), upper_bound(__k));
    }

    template <class _Key, class _Value, class _KoV, class _Compare, class _Alloc>
    inline
    pair<typename _Rb_tree<_Key, _Value, _KoV, _Compare, _Alloc>::const_iterator,
            typename _Rb_tree<_Key, _Value, _KoV, _Compare, _Alloc>::const_iterator>
    _Rb_tree<_Key, _Value, _KoV, _Compare, _Alloc>
    ::equal_range(const _Key& __k) const
    {
        return pair<const_iterator,const_iterator>(lower_bound(__k),
                                                   upper_bound(__k));
    }

    inline int
    __black_count(_Rb_tree_node_base* __node, _Rb_tree_node_base* __root)
    {
        if (__node == 0)
            return 0;
        else {
            int __bc = __node->_M_color == _S_rb_tree_black ? 1 : 0;
            if (__node == __root)
                return __bc;
            else
                return __bc + __black_count(__node->_M_parent, __root);
        }
    }

    template <class _Key, class _Value, class _KeyOfValue,
            class _Compare, class _Alloc>
    bool _Rb_tree<_Key,_Value,_KeyOfValue,_Compare,_Alloc>::__rb_verify() const
    {
        if (_M_node_count == 0 || begin() == end())
            return _M_node_count == 0 && begin() == end() &&
                   _M_header->_M_left == _M_header && _M_header->_M_right == _M_header;

        int __len = __black_count(_M_leftmost(), _M_root());
        for (const_iterator __it = begin(); __it != end(); ++__it) {
            _Link_type __x = (_Link_type) __it._M_node;
            _Link_type __L = _S_left(__x);
            _Link_type __R = _S_right(__x);

            if (__x->_M_color == _S_rb_tree_red)
                if ((__L && __L->_M_color == _S_rb_tree_red) ||
                    (__R && __R->_M_color == _S_rb_tree_red))
                    return false;

            if (__L && _M_key_compare(_S_key(__x), _S_key(__L)))
                return false;
            if (__R && _M_key_compare(_S_key(__R), _S_key(__x)))
                return false;

            if (!__L && !__R && __black_count(__x, _M_root()) != __len)
                return false;
        }

        if (_M_leftmost() != _Rb_tree_node_base::_S_minimum(_M_root()))
            return false;
        if (_M_rightmost() != _Rb_tree_node_base::_S_maximum(_M_root()))
            return false;

        return true;
    }

} // namespace MYSTL


#endif //MEMORY_MANAGE_RB_TREE_H
