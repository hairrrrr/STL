//
// Created by 78172 on 2023/4/9.
//

#ifndef MEMORY_MANAGE_HASH_FUNC_H
#define MEMORY_MANAGE_HASH_FUNC_H

#include <stddef.h>

#define __STL_TEMPLATE_NULL template<>

namespace MYSTL
{

    // 类类型的通用哈希函数，将对象指针转为字符指针，每次取一个字符的值
    inline size_t _Hash_bytes(const void* ptr, size_t len)
    {
        const unsigned char* p = static_cast<const unsigned char*>(ptr);
        size_t h = 0;
        for (size_t i = 0; i < len; ++i) {
            h = 31 * h + p[i];
        }
        return h;
    }


    // 对字符字符串进行转换
    inline size_t __stl_hash_string(const char* __s)
    {
        unsigned long __h = 0;
        for ( ; *__s; ++__s)
            // BKDR Hash Function
            __h = 131 *__h + *__s;

        return size_t(__h);
    }

    template <class _Key> struct hash
    {
        size_t operator()(const _Key& key) const
        {
            return _Hash_bytes(&key, sizeof(_Key));
        }
    };


    __STL_TEMPLATE_NULL struct hash<char*>
    {
        size_t operator()(const char* __s) const { return __stl_hash_string(__s); }
    };

    __STL_TEMPLATE_NULL struct hash<const char*>
    {
        size_t operator()(const char* __s) const { return __stl_hash_string(__s); }
    };

// 其它类型，直接返回原值
    __STL_TEMPLATE_NULL struct hash<char> {
        size_t operator()(char __x) const { return __x; }
    };
    __STL_TEMPLATE_NULL struct hash<unsigned char> {
        size_t operator()(unsigned char __x) const { return __x; }
    };
    __STL_TEMPLATE_NULL struct hash<signed char> {
        size_t operator()(unsigned char __x) const { return __x; }
    };
    __STL_TEMPLATE_NULL struct hash<short> {
        size_t operator()(short __x) const { return __x; }
    };
    __STL_TEMPLATE_NULL struct hash<unsigned short> {
        size_t operator()(unsigned short __x) const { return __x; }
    };
    __STL_TEMPLATE_NULL struct hash<int> {
        size_t operator()(int __x) const { return __x; }
    };
    __STL_TEMPLATE_NULL struct hash<unsigned int> {
        size_t operator()(unsigned int __x) const { return __x; }
    };
    __STL_TEMPLATE_NULL struct hash<long> {
        size_t operator()(long __x) const { return __x; }
    };
    __STL_TEMPLATE_NULL struct hash<unsigned long> {
        size_t operator()(unsigned long __x) const { return __x; }
    };


}


#endif //MEMORY_MANAGE_HASH_FUNC_H
