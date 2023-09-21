//
// Created by 78172 on 2023/3/30.
//

#ifndef MEMORY_MANAGE_ALGOBASE_H
#define MEMORY_MANAGE_ALGOBASE_H

#include"iterator_base.h"
#include"type_traits.h"

namespace MYSTL
{

//--------------------------------------------------
// fill and fill_n

// 将 [first, last) 内的所有元素改填新值
    template <class _ForwardIter, class _Tp>
    void fill(_ForwardIter __first, _ForwardIter __last, const _Tp& __value) {
        for ( ; __first != __last; ++__first)
            *__first = __value;
    }
    // 将 [first, last) 内的前 n 个元素改填新值
    template <class _OutputIter, class _Size, class _Tp>
    _OutputIter fill_n(_OutputIter __first, _Size __n, const _Tp& __value) {
        for ( ; __n > 0; --__n, ++__first)
            *__first = __value;
        return __first;
    }

//--------------------------------------------------
// copy

// All of these auxiliary functions serve two purposes.  (1) Replace
// calls to copy with memmove whenever possible.  (Memmove, not memcpy,
// because the input and output ranges are permitted to overlap.)
// (2) If we're using random access iterators, then write the loop as
// a for loop with an explicit count.
// copy 函数-效率 memmove
// copy 版本
    template <class _InputIter, class _OutputIter, class _Distance>
    inline _OutputIter __copy(_InputIter __first, _InputIter __last,
                              _OutputIter __result,
                              input_iterator_tag, _Distance*)
    {
        for ( ; __first != __last; ++__result, ++__first)
            *__result = *__first;
        return __result;
    }

    template <class _RandomAccessIter, class _OutputIter, class _Distance>
    inline _OutputIter
    __copy(_RandomAccessIter __first, _RandomAccessIter __last,
           _OutputIter __result, random_access_iterator_tag, _Distance*)
    {
        for (_Distance __n = __last - __first; __n > 0; --__n) {
            *__result = *__first;
            ++__first;
            ++__result;
        }
        return __result;
    }

    // 使用 memmove
    template <class _Tp>
    inline _Tp*
    __copy_trivial(const _Tp* __first, const _Tp* __last, _Tp* __result) {
        memmove(__result, __first, sizeof(_Tp) * (__last - __first));
        return __result + (__last - __first);
    }

    template <class _InputIter, class _OutputIter>
    inline _OutputIter __copy_aux2(_InputIter __first, _InputIter __last,
                                   _OutputIter __result, __false_type) {
        return __copy(__first, __last, __result,
                      __ITERATOR_CATEGORY(__first),
                      __DISTANCE_TYPE(__first));
    }

//    template <class _InputIter, class _OutputIter>
//    inline _OutputIter __copy_aux2(_InputIter __first, _InputIter __last,
//                                   _OutputIter __result, __true_type) {
//        return __copy(__first, __last, __result,
//                      __ITERATOR_CATEGORY(__first),
//                      __DISTANCE_TYPE(__first));
//    }

    template <class _Tp>
    inline _Tp* __copy_aux2(const _Tp* __first, const _Tp* __last, _Tp* __result,
                            __true_type) {
        return __copy_trivial(__first, __last, __result);
    }


    template <class _InputIter, class _OutputIter, class _Tp>
    inline _OutputIter __copy_aux(_InputIter __first, _InputIter __last,
                                  _OutputIter __result, _Tp*) {
        typedef typename __type_traits<_Tp>::has_trivial_assignment_operator
                _Trivial;
        return __copy_aux2(__first, __last, __result, _Trivial());
    }

    template <class _InputIter, class _OutputIter>
    inline _OutputIter copy(_InputIter __first, _InputIter __last,
                            _OutputIter __result) {
        return __copy_aux(__first, __last, __result, __VALUE_TYPE(__first));
    }


    // ---------------------------------
    // copy_backward

    template <class _BidirectionalIter1, class _BidirectionalIter2,
            class _Distance>
    inline _BidirectionalIter2 __copy_backward(_BidirectionalIter1 __first,
                                               _BidirectionalIter1 __last,
                                               _BidirectionalIter2 __result,
                                               bidirectional_iterator_tag,
                                               _Distance*)
    {
        while (__first != __last)
            *--__result = *--__last;
        return __result;
    }

    template <class _RandomAccessIter, class _BidirectionalIter, class _Distance>
    inline _BidirectionalIter __copy_backward(_RandomAccessIter __first,
                                              _RandomAccessIter __last,
                                              _BidirectionalIter __result,
                                              random_access_iterator_tag,
                                              _Distance*)
    {
        for (_Distance __n = __last - __first; __n > 0; --__n)
            *--__result = *--__last;
        return __result;
    }

    template <class _BI1, class _BI2>
    inline _BI2 copy_backward(_BI1 __first, _BI1 __last, _BI2 __result) {
        return __copy_backward(__first, __last, __result,
                               __ITERATOR_CATEGORY(__first),
                               __DISTANCE_TYPE(__first));
    }

}

#endif //MEMORY_MANAGE_ALGOBASE_H
