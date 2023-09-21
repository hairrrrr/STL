//
// Created by 78172 on 2023/4/10.
//

#ifndef MEMORY_MANAGE_BITSET_H
#define MEMORY_MANAGE_BITSET_H

#include<cstddef>
#include<vector>

namespace utility
{

    template<size_t N>
    class bitset
    {
    public:

        bitset() : _M_bits((N + 7) / 8) {}


        void set(size_t x)
        {
            _M_bits[x / 8] |= 1 << ( x % 8 );
        }

        void reset(size_t x)
        {
            _M_bits[x / 8] &= ~( 1 << (x % 8) );
        }

        bool get(size_t x) const
        {
            return _M_bits[x / 8] & (1 << (x % 8));
        }

        size_t size_bytes() const { return _M_bits.size(); }

    private:
        std::vector<char> _M_bits;
    };


    template<size_t N>
    class twobitset
    {
    public:
        twobitset()
        { }

        void set(size_t x)
        {
            bool in_bit1 = _M_bit1.get(x);
            bool in_bit2 = _M_bit2.get(x);

            // x 第一次出现 00->01
            if( !in_bit1 && !in_bit2 )
                _M_bit1.set(x);
            // x 第二次出现 01->10
            else if( in_bit1 && !in_bit2 )
                _M_bit1.reset(x), _M_bit2.set(x);
            // x 出现第三次 10->11
            else
                _M_bit1.set(x);
        }

        void print_once_num() const
        {
            std::cout << "[nums show once]: ";
            for(size_t i = 0; i < N; ++i)
                if( _M_bit1.get(i) && !_M_bit2.get(i) )
                    std::cout << i << " ";
            std::cout << endl;
        }

    private:
        bitset<N> _M_bit1;
        bitset<N> _M_bit2;
    };

}


#endif //MEMORY_MANAGE_BITSET_H
