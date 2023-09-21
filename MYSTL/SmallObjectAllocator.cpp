//
// Created by 78172 on 2023/3/26.
//

#include"allocator.h"



namespace MYSTL
{
    bool Chunk::Init( std::size_t blockSize, unsigned char blocks )
    {
        assert(blockSize > 0);
        assert(blocks > 0);
        const size_t allocSize = blockSize * blocks;
        assert(allocSize / blockSize == blocks);

        _pData = static_cast<unsigned char*>(::operator new(allocSize));

#ifdef DO_MYSTL_TEST
        std::cout << "[Chunk Init] blockSize = " << blockSize << " numBlock = " << (int)blocks;
        std::cout << " p = " << (void*)_pData << endl;
#endif

        Reset(blockSize, blocks);
        return true;
    }

    void Chunk::Reset( std::size_t blockSize, unsigned char blocks )
    {
        assert(blockSize > 0);
        assert(blocks > 0);
        // overflow check
        assert((blockSize * blocks) / blocks == blockSize);

        _blocksAvailable = blocks;
        _firstAvailableBlock = 0;

        // 最后一个 block 初始化为 blocks
        // 但是如果 blocks == 255，那么最后一个 block 就是 0
        unsigned char i = 0;
        for( unsigned char* p = _pData; i!= blocks; p += blockSize )
            *p = ++i;
    }

    void Chunk::Release()
    {
        assert(nullptr != _pData);

#ifdef DO_MYSTL_TEST
        std::cout << "[Chunk Release] p = " << (void*)_pData << endl;
#endif

        ::operator delete(_pData);
    }

    void* Chunk::Allocate( std::size_t blockSize )
    {
        if(IsFilled()) return nullptr;

        //_firstAvailableBlock 可能是 0 不能做分母
        assert((_firstAvailableBlock * blockSize) /  blockSize == _firstAvailableBlock);

        unsigned char* res = _pData + (_firstAvailableBlock * blockSize);
        _firstAvailableBlock = *res;
        _blocksAvailable--;

        return res;
    }

    void  Chunk::Deallocate( void* p, std::size_t blockSize )
    {
        assert(p >= _pData);

#ifdef DO_MYSTL_TEST
        std::cout << "[Chunk Deallocate] p = " << (void*)p << "blockSize = " << blockSize << endl;
#endif

        unsigned char* toFree = static_cast<unsigned char*>(p);

        assert((toFree - _pData) % blockSize == 0);

        unsigned char index = (toFree - _pData) / blockSize;

        *toFree = _firstAvailableBlock;
        _firstAvailableBlock = index;
        ++_blocksAvailable;
    }


} // namespace MYSTL
