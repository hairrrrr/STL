//
// Created by 78172 on 2023/2/27.
//

#ifndef MEMORY_MANAGE_PAIR_H
#define MEMORY_MANAGE_PAIR_H

#include<iostream>
#include"function.h"

namespace MYSTL
{
    template <class T1, class T2>
    struct pair
    {
        typedef T1 first_type;
        typedef T2 second_type;

        T1 first;  // 第一个参数
        T2 second; // 第二个参数

        pair() : first(T1()), second(T2()) {}


        template<class U1 = T1, class U2 = T2>
        pair(const U1&, const U2&);

        // 这里不能声明为 explicit，否则 map 会报错
        template<class U1, class U2>
        pair(const pair<U1, U2>& p)
            :first(p.first), second(p.second)
        {}
    };

    template<class T1, class T2>
    template<class U1, class U2>
    pair<T1, T2>::pair(const U1& f, const U2& s)
        :first(f), second(s)
    {
        //std::cout << "pair(const U1& f, const U2& s)" << std::endl;
    }

    template <class T1, class T2>
    bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return ( lhs.first == rhs.first ) && ( lhs.second == rhs.second );
    }

    template <class T1, class T2>
    bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return ( lhs.first < rhs.first ) ||
               ( lhs.first == rhs.first && lhs.second < rhs.second );
    }

    template <class T1, class T2>
    bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return !(lhs == rhs);
    }

    template <class T1, class T2>
    bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs)
    {
        return !(lhs == rhs) && !(lhs < rhs);
    }


    template<class Pair>
    struct select1st : public unary_function<Pair, typename Pair::first_type>
    {
        const typename Pair::first_type&
        operator()(const Pair& pair) const
        {
            return pair.first;
        }
    };

    template<typename T1, typename T2>
    inline pair<T1, T2> make_pair(const T1& t1, const T2& t2)
    { return pair<T1, T2>(t1, t2); }

    template<class Pair>
    struct select2nd : public std::unary_function<Pair, typename Pair::second_type>
    {
        const typename Pair::second_type&
        operator()(const Pair& pair) const
        {
            return pair.second;
        }
    };

};

//int main()
//{
//    //std::pair<int, int> p;
//    std::plus<int>()(1,1);
//
//    MySTL::pair<int, double> p(1, 3.1);
//    MySTL::select1st<MySTL::pair<int, double>> s1;
//    MySTL::select2nd<MySTL::pair<int, double>> s2;
//    cout << s1(p) << " " << s2(p) << endl;
//}

#endif //MEMORY_MANAGE_PAIR_H
