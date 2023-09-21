////
//// Created by 78172 on 2023/2/11.
////
//

#include"MYSTL/allocator.h"
#include<vector>

void le()
{

    MYSTL::SmallObjAllocator<std::vector<MYSTL::Chunk>> allocator(LOKI_DEFAULT_CHUNK_SIZE,
                                                                LOKI_MAX_SMALL_OBJECT_SIZE,
                                                                LOKI_DEFAULT_OBJECT_ALIGNMENT);

    for(int i = 0; i < 10; ++i)
    {
        allocator.Allocate(1, true); // 4  1
        allocator.Allocate(10, true);// 12 3
        allocator.Allocate(20, true);// 20 5
        allocator.Allocate(70, true);// 72 18
        allocator.Allocate(100, true);//100 25
        allocator.Allocate(253, true);//256 64
    }
}

//#include<map>
//#include<iostream>
//#include<vector>
//#include<unordered_map>
//
//using namespace std;
//
////Definition for a binary tree node.
//struct TreeNode
//{
//  int val;
//  TreeNode *left;
//  TreeNode *right;
//  TreeNode() : val(0), left(nullptr), right(nullptr) {}
//  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
//  TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
//};
//
//class levelOrder
//{
//public:
//    vector<vector<TreeNode*>> ans;
//
//    void dfs(TreeNode* root, int level)
//    {
//        if(!root) return;
//
//        if(ans.size() == level)
//            ans.resize(level + 1);
//        ans[level].push_back(root->left);
//        ans[level].push_back(root->right);
//
//        dfs(root->left, level + 1);
//        dfs(root->right, level + 1);
//    }
//
//    vector<vector<TreeNode*>> operator()(TreeNode* root)
//    {
//        if(!root) return ans;
//        ans.push_back({root});
//        dfs(root, 1);
//        return ans;
//    }
//};
//
//class buildTreeFromPreIn {
//public:
//    vector<int> preorder, inorder;
//    multimap<int, int> in_val2idx;
//    int pre_idx;
//    TreeNode* dfs(int l, int r)
//    {
//        if(l > r) return nullptr;
//
//        TreeNode* root = new TreeNode(preorder[pre_idx++]);
//
//        auto it = in_val2idx.find(root->val);
//        int m = it->second;
//        in_val2idx.erase(it);
//
//        root->left  = dfs(l, m - 1);
//        root->right = dfs(m + 1, r);
//
//        return root;
//    }
//
//    TreeNode* operator()(vector<int>& pre, vector<int>& in)
//    {
//        preorder = pre, inorder = in;
//        pre_idx = 0;
//
//        int n = in.size();
//        // 用哈希记录 inordered 的值与下标对应关系，dfs 可以加速处理
//        for(int i = 0; i < n; i++)
//            in_val2idx.insert({inorder[i], i});
//
//        return dfs(0, in.size() - 1);
//    }
//};
//
//template<class T>
//void ShowVectorVector(const vector<vector<T>>& vv)
//{
//    cout << "[" << endl;
//    for(auto v : vv)
//    {
//        cout << "   [";
//        for(auto p : v)
//        {
//            if(p)
//                cout << p->val << ", ";
//            else
//                cout << nullptr << ", ";
//        }
//        cout << "]" << endl;
//    }
//    cout << "]" << endl;
//}
//
//class DeductPostFromPreIn
//{
//public:
//    vector<int> pre, in, post;
//    unordered_map<int, int> in_val2idx;
//
//    int post_idx;
//    int pre_idx;
//    void dfs(int l, int r)
//    {
//        if(l > r) return;
//        int root = pre[pre_idx++];
//        int m = in_val2idx[root];
//        dfs(l, m - 1), dfs(m + 1, r);
//        post.push_back(root);
//    }
//
//    vector<int> operator()(vector<int>& preorder, vector<int>& inorder)
//    {
//        post_idx = pre_idx = 0;
//        pre = preorder, in = inorder;
//
//        int n = inorder.size();
//        for(int i = 0; i < n; ++i)
//            in_val2idx[in[i]] = i;
//
//        dfs(0, n - 1);
//
//        return post;
//    }
//};
//
//class buildTreeFromLevelIn
//{
//public:
//    vector<int> in, level, pre, post;
//    unordered_map<int, int> level_val2idx;
//
//    TreeNode* dfs(int l, int r) &
//    {
//        if(l > r) return nullptr;
//
//        // 遍历区间 [l, r]，层序遍历下标最小的点是根节点
//        int j = l;
//        for(int i = j + 1; i <= r; ++i)
//            if(level_val2idx[in[i]] < level_val2idx[in[j]])
//                j = i;
//
//        TreeNode* root = new TreeNode(in[j]);
//
//        root->left  = dfs(l, j - 1);
//        root->right = dfs(j + 1, r);
//
//        return root;
//    }
//
//    void dfs(int l, int r) &&
//    {
//        if(l > r) return;
//
//        // 遍历区间 [l, r]，层序遍历下标最小的点是根节点
//        int j = l;
//        for(int i = j + 1; i <= r; ++i)
//            if(level_val2idx[in[i]] < level_val2idx[in[j]])
//                j = i;
//        //cout << j << endl;
//        pre.push_back(in[j]);
//
//        std::move(*this).dfs(l, j - 1);
//        std::move(*this).dfs(j + 1, r);
//
//        post.push_back(in[j]);
//    }
//
//    void PreOrderPrint(TreeNode* r)
//    {
//        if(!r)
//        {
//            cout << nullptr << ", ";
//            return;
//        }
//        cout << r->val << ", ";
//        if(!r->left && !r->right) return;
//        PreOrderPrint(r->left);
//        PreOrderPrint(r->right);
//    }
//
//    void operator()(vector<int>& inorder, vector<int>& levelorder)
//    {
//        in = inorder, level = levelorder;
//        int n = in.size();
//
//        for(int i = 0; i < n; ++i)
//            level_val2idx[levelorder[i]] = i;
//
//        std::move(*this).dfs(0, n - 1);
//
//        //PreOrderPrint(dfs(0, n - 1));
//
//        cout << "pre: ";
//        for(auto e : pre) cout << e << " ";
//        cout << "\n" << "post: ";
//        for(auto e : post) cout << e << " ";
//        cout << endl;
//    }
//};
//
////int main()
////{
////    vector<int> in{13,9,12,3,15,20,7}, level{3,9,20,13,12,15,7};
////    buildTreeFromLevelIn s;
////    s(in, level);
////
////    return 0;
////}
