//
// Created by 78172 on 2023/3/30.
//

#ifndef MEMORY_MANAGE_UNINITIALIZED_H
#define MEMORY_MANAGE_UNINITIALIZED_H

#include"type_traits.h"
#include"algobase.h"
#include"construct.h"
#include<cstring>

namespace MYSTL
{
    // ==================== uninitialized_fill_n ===========================
    // Valid if copy construction is equivalent to assignment, and if the
    //  destructor is trivial.
    template <class _ForwardIter, class _Size, class _Tp>
    inline _ForwardIter
    __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,
                               const _Tp& __x, __true_type)
    {
        return fill_n(__first, __n, __x);
    }

    template <class _ForwardIter, class _Size, class _Tp>
    _ForwardIter
    __uninitialized_fill_n_aux(_ForwardIter __first, _Size __n,
                               const _Tp& __x, __false_type)
    {
        _ForwardIter __cur = __first;
        try {
            for ( ; __n > 0; --__n, ++__cur)
                Construct(&*__cur, __x);
            return __cur;
        }
        // 如果任何一个对象构造失败，销毁之前已经构造好的所有对象。
        catch (...) {
            (Destroy(__first, __cur));
        }
    }

    template <class _ForwardIter, class _Size, class _Tp, class _Tp1>
    inline _ForwardIter
    __uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x, _Tp1*)
    {
        typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
        return __uninitialized_fill_n_aux(__first, __n, __x, _Is_POD());
    }

    template <class _ForwardIter, class _Size, class _Tp>
    inline _ForwardIter
    uninitialized_fill_n(_ForwardIter __first, _Size __n, const _Tp& __x)
    {
        return __uninitialized_fill_n(__first, __n, __x, __VALUE_TYPE(__first));
    }


    // ==================== uninitialized_copy ===========================
    // Valid if copy construction is equivalent to assignment, and if the
    //  destructor is trivial.
    template <class _InputIter, class _ForwardIter>
    inline _ForwardIter
    __uninitialized_copy_aux(_InputIter __first, _InputIter __last,
                             _ForwardIter __result,
                             __true_type)
    {
        return copy(__first, __last, __result);
    }

    template <class _InputIter, class _ForwardIter>
    _ForwardIter
    __uninitialized_copy_aux(_InputIter __first, _InputIter __last,
                             _ForwardIter __result,
                             __false_type)
    {
        _ForwardIter __cur = __result;
        try {
            for ( ; __first != __last; ++__first, ++__cur)
                Construct(&*__cur, *__first);
            return __cur;
        }
        catch(...){
            Destroy(__result, __cur);
        }

    }

    template <class _InputIter, class _ForwardIter, class _Tp>
    inline _ForwardIter
    __uninitialized_copy(_InputIter __first, _InputIter __last,
                         _ForwardIter __result, _Tp*)
    {
        typedef typename __type_traits<_Tp>::is_POD_type _Is_POD;
        return __uninitialized_copy_aux(__first, __last, __result, _Is_POD());
    }

// 内存的配置与对象的构造行为分离开来。
    template <class _InputIter, class _ForwardIter>
    inline _ForwardIter
    uninitialized_copy(_InputIter __first, _InputIter __last,
                       _ForwardIter __result)
    {
        return __uninitialized_copy(__first, __last, __result,
                                    __VALUE_TYPE(__result));
    }


    inline char* uninitialized_copy(const char* __first, const char* __last,
                                    char* __result) {
        memmove(__result, __first, __last - __first);
        return __result + (__last - __first);
    }

    inline wchar_t*
    uninitialized_copy(const wchar_t* __first, const wchar_t* __last,
                       wchar_t* __result)
    {
        memmove(__result, __first, sizeof(wchar_t) * (__last - __first));
        return __result + (__last - __first);
    }


// ==========================  uninitialized_fill =========================
// Valid if copy construction is equivalent to assignment, and if the
// destructor is trivial.
    template <class _ForwardIter, class _Tp>
    inline void
    __uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last,
                             const _Tp& __x, __true_type)
    {
        fill(__first, __last, __x);
    }

    template <class _ForwardIter, class _Tp>
    void
    __uninitialized_fill_aux(_ForwardIter __first, _ForwardIter __last,
                             const _Tp& __x, __false_type)
    {
        _ForwardIter __cur = __first;
        try {
            for ( ; __cur != __last; ++__cur)
                Construct(&*__cur, __x);
        }
        catch(...){
            Destroy(__first, __cur);
        }
    }

    template <class _ForwardIter, class _Tp, class _Tp1>
    inline void __uninitialized_fill(_ForwardIter __first,
                                     _ForwardIter __last, const _Tp& __x, _Tp1*)
    {
        typedef typename __type_traits<_Tp1>::is_POD_type _Is_POD;
        __uninitialized_fill_aux(__first, __last, __x, _Is_POD());
    }

    template <class _ForwardIter, class _Tp>
    inline void uninitialized_fill(_ForwardIter __first,
                                   _ForwardIter __last,
                                   const _Tp& __x)
    {
        __uninitialized_fill(__first, __last, __x, __VALUE_TYPE(__first));
    }
}

#endif //MEMORY_MANAGE_UNINITIALIZED_H
