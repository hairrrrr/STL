#include <iostream>
#include <vector>
// NOTE: 123123
// ISSUE: 123123
// TODO: 123123
// FIXME: 12323


using std::cout;
using std::endl;

//inline void* operator new(size_t size)
//{
//    void* ptr = malloc(size);
//    cout << "\n       [global operator new] size = "  << size  << " ptr = " << ptr << endl;
//    cout << endl;
//    return ptr;
//}

//inline void* operator new[](size_t size)
//{
//    void * ptr = malloc(size);
//    cout << "\n       [global operator new[]] size = "  << size << " ptr = " << ptr << endl;
//    cout << endl;
//    return ptr;
//}

// 要加上第二参数才会调用到自定义的 operator delete
//inline void operator delete(void* ptr, size_t)
//{
//    cout << "[global operator delete] ptr = " << ptr << endl;
//    free(ptr);
//}
//
//inline void operator delete[](void* ptr, size_t)
//{
//    cout << "[global operator delete[]] ptr = " << ptr << endl;
//    free(ptr);
//}


#include"utility/bitset.h"

#include"utility/bloomfilter.h"

#include <memory>

#include <unordered_map>

#include "MYSTL/hash_map.h"

int main()
{

    std::shared_ptr<int[]> sp(new int[10]);

    std::cout << sizeof(std::unordered_map<int, int>) << std::endl;
    std::cout << sizeof(MYSTL::hash_map<int, int>) << std::endl;

    return 0;
}
