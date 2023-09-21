//
// Created by 78172 on 2023/4/3.
//

#ifndef MEMORY_MANAGE_ALGO_H
#define MEMORY_MANAGE_ALGO_H

#include"algobase.h"

namespace MYSTL
{

    /**********************
     * find & find_if
     */

    template<typename InputIteraor, typename Tp>
    inline InputIteraor find(InputIteraor first, InputIteraor last,
                             const Tp& val, input_iterator_tag)
    {
        while( (first != last) && (*first != val) )
           ++first;
        return first;
    }

    template<typename InputIteraor, typename Tp>
    inline InputIteraor find(InputIteraor first, InputIteraor last,
                             const Tp& val, random_access_iterator_tag)
    {
        typename iterator_traits<InputIteraor>::difference_type trip_count =
                (last - first) >> 2;

        for(; trip_count > 0; --trip_count)
        {
            if(*first == val) return first;
            ++first;
            if(*first == val) return first;
            ++first;
            if(*first == val) return first;
            ++first;
            if(*first == val) return first;
            ++first;
        }

        // 剩余的元素，范围在 0 ~ 3
        switch(last - first)
        {
            case 3:
                if(*first == val) return first;
                ++first;
            case 2:
                if(*first == val) return first;
                ++first;
            case 1:
                if(*first == val) return first;
                ++first;
            case 0:
            default:
                return last;
        }
    }

    template<typename InputIteraor, typename Tp>
    inline InputIteraor find(InputIteraor first, InputIteraor last,
                             const Tp& val)
    { return find(first, last, val, MYSTL::iterator_category(first)); }


    template<typename InputIteraor, typename Predicate>
    inline InputIteraor find_if(InputIteraor first, InputIteraor last,
                                Predicate pred, input_iterator_tag)
    {
        while( (first != last) && Pred(*first) )
            ++first;
        return first;
    }

    template<typename InputIteraor, typename Predicate>
    inline InputIteraor find_if(InputIteraor first, InputIteraor last,
                                Predicate pred, random_access_iterator_tag)
    {
        typename iterator_traits<InputIteraor>::difference_type trip_count =
                (last - first) >> 2;

        for(; trip_count > 0; --trip_count)
        {
            if(Pred(*first)) return first;
            ++first;
            if(Pred(*first)) return first;
            ++first;
            if(Pred(*first)) return first;
            ++first;
            if(Pred(*first)) return first;
            ++first;
        }

        // 剩余的元素，范围在 0 ~ 3
        switch(last - first)
        {
            case 3:
                if(Pred(*first)) return first;
                ++first;
            case 2:
                if(Pred(*first)) return first;
                ++first;
            case 1:
                if(Pred(*first)) return first;
                ++first;
            case 0:
            default:
                return last;
        }
    }

    template<typename InputIteraor, typename Predicate>
    inline InputIteraor find_if(InputIteraor first, InputIteraor last,
                                Predicate pred)
    { return find_if(first, last, pred, iterator_category(first)); }


    /*******************************
     *  lower_bound & upper_bound
     */

    template <class _ForwardIter, class _Tp, class _Distance>
    inline _ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                                      const _Tp& __val, _Distance*)
    {
        _Distance len = 0;
        distance(__first, __last, len);
        _Distance half;
        _ForwardIter middle;

        while( len > 0 )
        {
            half = len >> 1;
            middle = __first;
            advance(middle, half);
            if( *middle < __val )
            {
                __first = middle;
                ++__first;
                len = len - half - 1;
            }
            else
                len = half;
        }

        return __first;
    }

    template <class _ForwardIter, class _Tp>
    inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
                                    const _Tp& __val) {
        return __lower_bound(__first, __last, __val,
                             __DISTANCE_TYPE(__first));
    }

    template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
    _ForwardIter __lower_bound(_ForwardIter __first, _ForwardIter __last,
                               const _Tp& __val, _Compare __comp, _Distance*)
    {
        _Distance __len = 0;
        distance(__first, __last, __len);
        _Distance __half;
        _ForwardIter __middle;

        while (__len > 0) {
            __half = __len >> 1;
            __middle = __first;
            advance(__middle, __half);
            if (__comp(*__middle, __val)) {
                __first = __middle;
                ++__first;
                __len = __len - __half - 1;
            }
            else
                __len = __half;
        }
        return __first;
    }

    template <class _ForwardIter, class _Tp, class _Compare>
    inline _ForwardIter lower_bound(_ForwardIter __first, _ForwardIter __last,
                                    const _Tp& __val, _Compare __comp) {

        return __lower_bound(__first, __last, __val, __comp,
                             __DISTANCE_TYPE(__first));
    }

    template <class _ForwardIter, class _Tp, class _Distance>
    _ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                               const _Tp& __val, _Distance*)
    {
        _Distance __len = 0;
        distance(__first, __last, __len);
        _Distance __half;
        _ForwardIter __middle;

        while (__len > 0) {
            __half = __len >> 1;
            __middle = __first;
            advance(__middle, __half);
            if (__val < *__middle)
                __len = __half;
            else {
                __first = __middle;
                ++__first;
                __len = __len - __half - 1;
            }
        }
        return __first;
    }

    template <class _ForwardIter, class _Tp>
    inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                    const _Tp& __val) {
        return __upper_bound(__first, __last, __val,
                             __DISTANCE_TYPE(__first));
    }

    template <class _ForwardIter, class _Tp, class _Compare, class _Distance>
    _ForwardIter __upper_bound(_ForwardIter __first, _ForwardIter __last,
                               const _Tp& __val, _Compare __comp, _Distance*)
    {
        _Distance __len = 0;
        distance(__first, __last, __len);
        _Distance __half;
        _ForwardIter __middle;

        while (__len > 0) {
            __half = __len >> 1;
            __middle = __first;
            advance(__middle, __half);
            if (__comp(__val, *__middle))
                __len = __half;
            else {
                __first = __middle;
                ++__first;
                __len = __len - __half - 1;
            }
        }
        return __first;
    }

    template <class _ForwardIter, class _Tp, class _Compare>
    inline _ForwardIter upper_bound(_ForwardIter __first, _ForwardIter __last,
                                    const _Tp& __val, _Compare __comp) {
        return __upper_bound(__first, __last, __val, __comp,
                             __DISTANCE_TYPE(__first));
    }

}

#endif //MEMORY_MANAGE_ALGO_H
