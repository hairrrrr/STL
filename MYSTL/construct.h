//
// Created by 78172 on 2023/3/29.
//

#ifndef MEMORY_MANAGE_CONSTRUCT_H
#define MEMORY_MANAGE_CONSTRUCT_H

#include<bits/c++config.h>
#include<bits/move.h>
#include"type_traits.h"
#include"iterator_base.h"

#undef DO_MYSTL_TEST

//#ifndef DO_MYSTL_TEST
//#define DO_MYSTL_TEST
//#endif

namespace MYSTL
{
/**********************************
 * construct
 *
 * C++ 11 之后使用变参模板，函数参数为右值引用，配合完美转发
 */
#if __cplusplus >= 201103L

    template<typename T, typename... Args>
    inline void Construct(T* p, Args&&... value)
    { new( (void*)p ) T( std::forward<Args>(value)... ); }

    template<typename T, typename... Args>
    inline void construct(T* p, Args&&... value)
    { Construct(p, std::forward<Args>(value)...); }

#else

    template<class T1, class T2>
    inline void Construct(T1* p, const T2& value)
    { new( (void*)p ) T1( value ); }

    template<class T1>
    inline void Construct(T1* p)
    { new( (void*)p ) T1(); }

    template<class T1, class T2>
    inline void construct(T1* p, const T2& value)
    { Construct(p, value); }

    template<class T1>
    inline void construct(T1* p)
    { Construct(p); }

#endif

    /************************
    * destruct
    */

    template<class T>
    inline void Destroy(T* p)
    { p->~T(); }


    template<class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, MYSTL::__true_type)
    {
#ifdef DO_MYSTL_TEST
        std::cout << "__true_type" << std::endl;
#endif
    }

    template<class ForwardIterator>
    inline void __destroy_aux(ForwardIterator first, ForwardIterator last, MYSTL::__false_type)
    {
#ifdef DO_MYSTL_TEST
        std::cout << "__false_type" << std::endl;
#endif

        for(ForwardIterator it = first; it != last; ++it)
            Destroy(&*it);
    }


    template<class ForwardIterator, class Tp>
    inline void Destroy(ForwardIterator first, ForwardIterator last, Tp*)
    {
        typedef typename MYSTL::__type_traits<Tp>::has_trivial_destructor type;
        __destroy_aux(first, last, type());
    }

    template<class ForwardIterator>
    inline void Destroy(ForwardIterator first, ForwardIterator last)
    {

        Destroy(first, last, MYSTL::value_type(first));
    }


    template<class T>
    inline void destroy(T* p)
    { Destroy(p); }


    template<class ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last)
    { Destroy(first, last); }
}



#endif //MEMORY_MANAGE_CONSTRUCT_H
