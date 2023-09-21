//
// Created by 78172 on 2023/4/5.
//

#ifndef MEMORY_MANAGE_HEAP_H
#define MEMORY_MANAGE_HEAP_H

#include"iterator.h"

//#define MYSTL_TEST_HEAP

namespace MYSTL
{
    /**
     * @param first      heap 第一个元素所在位置
     * @param holeIndex  value 将要填入的下标
     * @param topIndex   根节点所在下标
     * @param value      新插入的节点
     * @param cmp        比较函数
     */
    template<typename RandomAccessIterator, typename Distance,
             typename Tp, typename Compare>
    inline void
    __push_heap(RandomAccessIterator first, Distance holeIndex,
                Distance topIndex, Tp value, Compare cmp)
    {
        Distance parent = (holeIndex - 1) >> 1;
        // holeIndex == topIndex 或 cmp(parent 的值，插入的值) == true
        while( holeIndex > topIndex && cmp( *(first + parent), value) )
        {
            // holeIndex 下标填入父节点的值
            *(first + holeIndex) = *(first + parent);
            // 更新父节点与holeIndex
            holeIndex = parent;
            parent = (holeIndex - 1) >> 1;
        }
        // 填入 value
        *(first + holeIndex) = value;
    }


    template<typename RandomAccessIterator, typename Compare,
             typename Distance, typename Tp>
    inline void
    __push_heap_aux(RandomAccessIterator first, RandomAccessIterator last,
              Compare cmp, Distance*, Tp*)
    {
        __push_heap(first, Distance((last - first) - 1), Distance(0),
                    Tp(*(last - 1 )), cmp);
    }


    template<typename RandomAccessIterator, typename Compare>
    inline void
    push_heap(RandomAccessIterator first, RandomAccessIterator last,
              Compare cmp)
    {
        __push_heap_aux(first, last, cmp,
                       distance_type(first), value_type(first));
    }



    /**
     * @param __first
     * @param __holeIndex 一开始为 0
     * @param __len       len 为 容器最后一个元素所在位置（要被删除的位置，根的值在该位置上）
     * @param __value     原本 容器最后一个元素的值
     * @param __comp
     */
    template <class _RandomAccessIterator, class _Distance,
            class _Tp, class _Compare>
    void
    __adjust_heap(_RandomAccessIterator first, _Distance holeIndex,
                  _Distance len, _Tp value, _Compare comp)
    {
        // 计算右儿子所在位置
        _Distance secondChild = 2 * holeIndex + 2;
        while( secondChild < len )
        {
            // 如果左儿子更合适
            if( comp( *(first + secondChild), *(first + secondChild - 1) ) )
                secondChild--;
            // value 比左右儿子都合适
            if( !comp(  value, *(first + secondChild) ) )
                break;

            *(first + holeIndex) = *(first + secondChild);
            // 更新 holeIndex
            holeIndex = secondChild;
            // 设置为其右儿子
            secondChild = 2 * secondChild + 2;
        }

        if( secondChild == len )
        {
            secondChild--;
            if( comp(value, *(first + secondChild)) )
                *(first + holeIndex) = *(first + secondChild);
            holeIndex = secondChild;
        }

        *(first + holeIndex) = value;
    }

    template <class _RandomAccessIterator, class _Tp, class _Compare,
              class _Distance>
    inline void
    __pop_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
               _RandomAccessIterator __result, _Tp __value, _Compare __comp,
               _Distance*)
    {
        // 取出最后一个元素作为 value, 然后将根节点放入最后一个位置
        *__result = *__first;
        __adjust_heap(__first, _Distance(0), _Distance(__last - __first),
                      __value, __comp);
    }

    template <class _RandomAccessIterator, class _Tp, class _Compare>
    inline void
    __pop_heap_aux(_RandomAccessIterator __first,
                   _RandomAccessIterator __last, _Tp*, _Compare __comp)
    {
        __pop_heap(__first, __last - 1, __last - 1, _Tp(*(__last - 1)), __comp,
                   __DISTANCE_TYPE(__first));
    }

    // pop_heap 允许指定大小比较标准
    template <class _RandomAccessIterator, class _Compare>
    inline void
    pop_heap(_RandomAccessIterator __first,
             _RandomAccessIterator __last, _Compare __comp)
    {
        __pop_heap_aux(__first, __last, __VALUE_TYPE(__first), __comp);
    }


    // sort_heap 允许指定大小比较标准
    template <class _RandomAccessIterator, class _Compare>
    void
    sort_heap(_RandomAccessIterator __first,
              _RandomAccessIterator __last, _Compare __comp)
    {
        while (__first != __last) {
            pop_heap(__first, __last, __comp);
            --__last;
        }
    }

#include"test.h"

    template <class _RandomAccessIterator, class _Compare,
            class _Tp, class _Distance>
    void
    __make_heap(_RandomAccessIterator __first, _RandomAccessIterator __last,
                _Compare __comp, _Tp*, _Distance*)
    {
        // 如果元素个数为 0 个或者 1 个，不需要重构
        if (__last - __first < 2) return;
        _Distance __len = __last - __first;

        // __parent 设置为第一个非叶节点
        // 对所有非叶节点进行向下调整
        for(_Distance __parent = (__len - 2) / 2; __parent >= 0; --__parent) {
            __adjust_heap(__first, __parent, __len,
                          _Tp(*(__first + __parent)), __comp);

#ifdef MYSTL_TEST_HEAP
            std::string prompt = "[make_heap] parent = ";
            prompt += std::to_string(__parent);
            prompt += " | range: ";
            MYSTL_print_range(prompt,__first, __last);
#endif
        }
    }

    // make_heap 允许指定大小比较标准
    template <class _RandomAccessIterator, class _Compare>
    inline void
    make_heap(_RandomAccessIterator __first,
              _RandomAccessIterator __last, _Compare __comp)
    {
        __make_heap(__first, __last, __comp,
                    __VALUE_TYPE(__first), __DISTANCE_TYPE(__first));
    }


} // namespace MYSTL


#endif //MEMORY_MANAGE_HEAP_H
