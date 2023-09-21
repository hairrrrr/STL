//
// Created by 78172 on 2023/4/7.
//

#ifndef MEMORY_MANAGE_BST_H
#define MEMORY_MANAGE_BST_H

#define MYSTL_TEST_BST

#include<bits/move.h>

#ifdef MYSTL_TEST_BST
#include<iostream>
#endif

namespace BST
{
    template<typename Key>
    struct _Set_node
    {
        _Set_node* _M_left;
        _Set_node* _M_right;
        Key _M_key;

        _Set_node(const Key& k)
            : _M_left(nullptr), _M_right(nullptr), _M_key(k) {}
    };

    template<typename Key>
    class Set
    {
        typedef _Set_node<Key> Node;

    public:
        Set() : _M_root(nullptr) {}

        /**
         * 查找 key 是否在 set 中
         * @param key 要查找的 key
         * @return 如果找到，返回 true
         */
        bool find(const Key& key)
        {
            Node* cur = _M_root;
            while(cur)
            {
                if( cur->_M_key > key )
                    cur = cur->_M_left;
                else if( cur->_M_key < key )
                    cur = cur->_M_right;
                else
                    return true;
            }
            return false;
        }

        /**
         * 插入 key
         * @param key
         * @return 如果树中已经存在 key，则插入失败。否则插入 key
         */
        bool insert(const Key& key)
        {
            Node* parent = nullptr;
            Node* cur = _M_root;
            while(cur)
            {
                parent = cur;
                if( cur->_M_key > key )
                    cur = cur->_M_left;
                else if( cur->_M_key < key )
                    cur = cur->_M_right;
                else
                    return false;
            }

            cur = new Node(key);
            if( !parent )
                _M_root = cur;
            else
            {
                if( key > parent->_M_key )
                    parent->_M_right = cur;
                else
                    parent->_M_left = cur;
            }

            return true;
        }


        bool erase(const Key& key)
        {
            Node* parent = nullptr;
            Node* cur = _M_root;

            while(cur)
            {
                parent = cur;
                if( cur->_M_key > key )
                    cur = cur->_M_left;
                else if( cur->_M_key < key )
                    cur = cur->_M_right;
                else
                    break;
            }

            if( !cur ) return false;

            // 1. 要删除的节点没有左子树
            if( !cur->_M_left )
            {
                // 要删除的节点为根节点
                if( !parent )
                    _M_root = cur->_M_right;
                if( cur == parent->_M_left )
                    parent->_M_left = cur->_M_right;
                else
                    parent->_M_right = cur->_M_right;
            }
            // 2. 要删除的节点没有右子树
            else if ( !cur->_M_right )
            {
                if( !parent )
                    _M_root = cur->_M_left;
                if( cur == parent->_M_left )
                    parent->_M_left = cur->_M_left;
                else
                    parent->_M_right = cur->_M_left;
            }
            // 3. 要删除的节点左右子树都存在
            else
            {
                // 待删除节点所在子树的新的根节点为：
                // 1. 左子树的最右节点（左子树的key最大节点） 2. 右子树的最左节点（右子树的key最小节点）
                // 首先要找到 右子树的最左节点 和其 父节点
                Node* minParent = cur;
                Node* minNode   = cur->_M_right;
                while(minNode -> _M_left)
                {
                    minParent = minNode;
                    minNode = minNode->_M_left;
                }

                // 我们不会真的删除待删除节点 cur，而是将待删除节点的值和右子树最左节点的值 minNode 交换
                // 然后删掉 minNode
                std::swap(cur->_M_key, minNode->_M_key);

                // 这样判断是因为 minParent 可能最终还是 cur，并没有进入循环里
                if( minNode == minParent->_M_left )
                    minParent->_M_left = minNode->_M_right;
                else
                    minParent->_M_right = minNode->_M_right;

                delete minNode;
            }

            return true;
        }

        /**
         * 将树中 key 为 tar 的节点的 key 修改为 val
         * 删除 + 查找
         */
        bool modify(const Key& tar, const Key& val)
        {
            if( !erase(tar) ) return false;
            return insert(val);
        }


        void InorderTraversal()
        {
#ifdef MYSTL_TEST_BST
            std::cout << "[Inorder Traversal]: ";
#endif

            dfs(_M_root);

#ifdef MYSTL_TEST_BST
            std::cout << std::endl;
#endif
        }


        Set( const Set& s )
        { _M_root =  _copy(s._M_root); }

        Set& operator=(Set s)
        { std::swap(_M_root, s._M_root); return *this; }

        ~Set() { _destroy(_M_root); _M_root = nullptr; }


        bool findR(const Key& key)
        { return _findR(_M_root, key); }
        bool insertR(const Key& key)
        { return _insertR(_M_root, key); }
        bool eraseR(const Key& key)
        { return _eraseR(_M_root, key); }

    private:

        void dfs(Node* root)
        {
            if(!root) return;
            dfs(root->_M_left);

#ifdef MYSTL_TEST_BST
            std::cout << root->_M_key << " ";
#endif

            dfs(root->_M_right);
        }

        bool _findR(Node* root, const Key& key)
        {
            if(!root) return false;

            if( root->_M_val > key )
                _findR(root->_M_left, key);
            else if( root->_M_val < key )
                _findR(root->_M_right, key);
            else
                return true;
        }

        bool _insertR(Node*& root, const Key& key)
        {
            if( !root )
            {
                root = new Node(key);
                return true;
            }

            if( root->_M_key > key )
                _insertR(root->_M_left, key);
            else if( root->_M_key < key )
                _insertR(root->_M_right, key);
            else
                return false;
        }

        bool _eraseR(Node*& root, const Key& key)
        {
            if(!root) return false;

            if( root->_M_key > key )
                _eraseR(root->_M_left, key);
            else if(root->_M_key < key)
                _eraseR(root->_M_right, key);
            else
            {
                // del 拿到 root 本身
                Node* del = root;
                // root 本身也是其父节点的 left 或 right
                // 由于函数传入的是指针的引用，则可以直接修改父节点
                if( !root->_M_left )
                    root = root->_M_right;
                else if( !root->_M_right )
                    root = root->_M_left;
                else
                {
                    Node* minNode = root->_M_right;
                    while(minNode->_M_left)
                        minNode = minNode->_M_left;
                    std::swap(root->_M_key, minNode->_M_key);

                    _eraseR(root->_M_right, key);
                }

                delete del;
                return true;
            }
        }

        Node* _copy(Node* root)
        {
            if(!root) return nullptr;

            Node* r = new Node(root->_M_key);
            r->_M_left  = _copy(root->_M_left);
            r->_M_right = _copy(root->_M_right);
            return r;
        }

        void _destroy(Node* root)
        {
            if( !root ) return;
            _destroy(root->_M_left);
            _destroy(root->_M_right);
            delete root;
        }

    private:
        Node* _M_root;
    };

}

#endif //MEMORY_MANAGE_BST_H







