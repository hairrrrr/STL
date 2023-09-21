//
// Created by 78172 on 2023/4/10.
//

#ifndef MEMORY_MANAGE_BLOOMFILTER_H
#define MEMORY_MANAGE_BLOOMFILTER_H

#include<cstddef>
#include<string>
#include<bitset>

namespace utility
{

    struct HashBKDR
    {
        // BKDR
        size_t operator()(const std::string& key)
        {
            size_t val = 0;
            for (auto ch : key)
            {
                val *= 131;
                val += ch;
            }

            return val;
        }
    };

    struct HashAP
    {
        // BKDR
        size_t operator()(const std::string& key)
        {
            size_t hash = 0;
            for (size_t i = 0; i < key.size(); i++)
            {
                if ((i & 1) == 0)
                {
                    hash ^= ((hash << 7) ^ key[i] ^ (hash >> 3));
                }
                else
                {
                    hash ^= (~((hash << 11) ^ key[i] ^ (hash >> 5)));
                }
            }
            return hash;
        }
    };

    struct HashDJB
    {
        // BKDR
        size_t operator()(const std::string& key)
        {
            size_t hash = 5381;
            for (auto ch : key)
            {
                hash += (hash << 5) + ch;
            }

            return hash;
        }
    };


    // N 是要存入的元素个数
    template<size_t N, typename Key = std::string, typename Hash1 = HashBKDR,
             typename Hash2 = HashDJB, typename Hash3 = HashAP>
    class BloomFilter
    {
    public:

        BloomFilter()
            : _M_bit(new std::bitset<N * _S_ratio>()) {}

        void set(const std::string& str)
        {
            size_t n = N * _S_ratio;
            size_t hash1 = Hash1()(str) % n;
            size_t hash2 = Hash2()(str) % n;
            size_t hash3 = Hash3()(str) % n;
            _M_bit->set(hash1);
            _M_bit->set(hash2);
            _M_bit->set(hash3);
        }

        bool test(const std::string& str) const
        {
            size_t n = N * _S_ratio;
            size_t hash1 = Hash1()(str) % n;
            size_t hash2 = Hash2()(str) % n;
            size_t hash3 = Hash3()(str) % n;

            bool in1 = _M_bit->test(hash1);
            bool in2 = _M_bit->test(hash2);
            bool in3 = _M_bit->test(hash3);

            return in1 && in2 && in3;
        }

    private:
        enum{ _S_ratio = 5 };
        std::bitset<N * _S_ratio>* _M_bit;
    };

}


#endif //MEMORY_MANAGE_BLOOMFILTER_H
