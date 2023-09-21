//
// Created by 78172 on 2023/4/5.
//

#ifndef MEMORY_MANAGE_FUNCTIONAL_H
#define MEMORY_MANAGE_FUNCTIONAL_H

#include<cstring>
#include "string.h"

namespace MYSTL
{

// 一元函数的参数类型和返回值类型
    template <class _Arg, class _Result>
    struct unary_function {
        typedef _Arg argument_type;
        typedef _Result result_type;
    };

// 二元函数的第一个参数类型和第二个参数类型，以及返回值类型
    template <class _Arg1, class _Arg2, class _Result>
    struct binary_function {
        typedef _Arg1 first_argument_type;
        typedef _Arg2 second_argument_type;
        typedef _Result result_type;
    };

    // 小于
    template <class _Tp>
    struct less : public binary_function<_Tp,_Tp,bool>
    {
        bool operator()(const _Tp& __x, const _Tp& __y) const
        { return __x < __y; }
    };

    // 小于
    template <class _Tp>
    struct greater : public binary_function<_Tp,_Tp,bool>
    {
        bool operator()(const _Tp& __x, const _Tp& __y) const
        { return __x > __y; }
    };

    // 等于
    template <class _Tp>
    struct equal_to : public binary_function<_Tp,_Tp,bool>
    {
        bool operator()(const _Tp& __x, const _Tp& __y) const { return __x == __y; }
    };

    template<>
    struct equal_to<MYSTL::string>
            : public binary_function<MYSTL::string,MYSTL::string,bool>
    {
        bool operator()(const MYSTL::string& __x, const MYSTL::string& __y) const
        { return strcmp(__x.c_str(), __y.c_str()) == 0; }
    };

    // identity is an extensions: it is not part of the standard.
    template <class _Tp>
    struct _Identity : public unary_function<_Tp,_Tp> {
        const _Tp& operator()(const _Tp& __x) const { return __x; }
    };

    template <class _Tp> struct identity : public _Identity<_Tp> {};
}

#endif //MEMORY_MANAGE_FUNCTIONAL_H
