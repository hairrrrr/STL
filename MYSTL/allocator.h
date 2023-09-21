//
// Created by 78172 on 2023/3/21.
//

#ifndef MEMORY_MANAGE_ALLOCATOR_H
#define MEMORY_MANAGE_ALLOCATOR_H

#define DO_MYSTL_TEST

#include<cstring>

#include<iostream>
#include<mutex>
#include<vector>
#include<cassert>
#include<new.h>
#include"construct.h"

#ifdef DO_MYSTL_TEST
using std::cout;
using std::endl;
#endif



//namespace perclass_alloc
//{
//    /*
//     * 第一种每个类的内存分配器
//     */
//    class Screen
//    {
//    public:
//        Screen() : _msg(nullptr) {}
//
//        explicit Screen(const char* msg)
//                : _msg(new char[strlen(msg) + 1])
//        { strcpy(_msg, msg); }
//
//        Screen(const Screen& s)
//                : _msg(new char[strlen(s._msg) + 1])
//        { strcpy(_msg, s._msg); }
//
//        Screen& operator=(Screen s)
//        {
//            this->swap(s);
//            return *this;
//        }
//
//        void swap(Screen& rhs)
//        {
//            std::swap(_msg, rhs._msg);
//        }
//
//        ~Screen()
//        {
//            cout << "~Screen()" << endl;
//            delete[] _msg;
//        }
//
//        static void* operator new(size_t sz)
//        {
//            // 只要当前空闲链表为空，就去申请新的 sChuckSize 块空闲区域
//            if(!sFreeStore)
//            {
//                std::cout << "run out of storage: allocate more" << std::endl;
//                size_t tot = sz * sChunkSize;
//                Screen* p = (Screen*)malloc(tot);
//                sFreeStore = reinterpret_cast<Screen*>(p);
//                // 将整块内存拆分成 sChuckSize 个 Screen
//                for(; p != &sFreeStore[sChunkSize - 1]; ++p)
//                    p->_next = p + 1;
//                p->_next = nullptr;
//            }
//
//            void* ret = reinterpret_cast<void*>(sFreeStore);
//            sFreeStore = sFreeStore->_next;
//            return ret;
//        }
//
//        static void operator delete(void* ptr, size_t sz)
//        {
//            Screen* scr = reinterpret_cast<Screen*>(ptr);
//            scr->_next = sFreeStore;
//            sFreeStore = scr;
//        }
//
//        Screen* _next;
//        static int sChunkSize;
//        static Screen* sFreeStore;
//    private:
//        char* _msg;
//    };
//
//    int Screen::sChunkSize = 24;
//    Screen* Screen::sFreeStore = nullptr;
//
//    /*
//     * 第二种每个类的内存分配器
//     */
//    class Airplane
//    {
//    private:
//        struct AirplaneRep {
//            unsigned long miles;
//            char type;
//        };
//
//        union {
//            AirplaneRep _rep;
//            Airplane* _next;
//        };
//    private:
//        static const int BLOCK_SIZE = 512;
//        static Airplane* freeList;
//    public:
//        void set(unsigned long m = 0, char t = 0)
//        { _rep = {m, t}; }
//        unsigned long getMiles() const { return _rep.miles; }
//        char getType() const { return _rep.type; }
//
//        static inline void* operator new(size_t sz);
//        static inline void operator delete(void* ptr, size_t sz);
//    };
//
//    void* Airplane::operator new(size_t sz)
//    {
//        if(sz != sizeof(Airplane))
//            return ::operator new(sz);
//
//        Airplane* p = freeList;
//        if(p)
//            freeList = freeList->_next;
//        else
//        {
//            Airplane* newBlock = reinterpret_cast<Airplane*>(
//                    ::operator new(sz * BLOCK_SIZE) );
//            for(int i = 1 ; i < BLOCK_SIZE - 1; ++i)
//                newBlock[i]._next = &newBlock[i + 1];
//            newBlock[BLOCK_SIZE - 1]._next = nullptr;
//            freeList = &newBlock[1];
//            p = newBlock;
//        }
//        return p;
//    }
//
//    void Airplane::operator delete(void* ptr, size_t sz)
//    {
//        if(sz != sizeof(Airplane))
//            return ::operator delete(ptr);
//        Airplane* ap = reinterpret_cast<Airplane*>(ptr);
//        ap->_next = freeList;
//        freeList = ap;
//    }
//
//}
//
//namespace static_alloc
//{
//    /*
//   * static allocator
//   */
//
//    class allocator
//    {
//    private:
//        // 将一块内存的前面一个指针大小的空间当作对象 obj
//        // 这么做是为了连接自由链表
//        struct obj {
//            struct obj* _next;
//        };
//
//        const int BLOCK_SIZE = 5;
//        obj* _freeList = nullptr;
//    public:
//        void* allocate(size_t sz);
//        void deallocate(void* ptr, size_t sz);
//    };
//
//    void* allocator::allocate(size_t sz)
//    {
//        if(sz < sizeof(obj))
//            return ::operator new(sz);
//
//        void* p = _freeList;
//        if(p)
//            _freeList = _freeList->_next;
//        else
//        {
//            std::cout << "run out of storage" << std::endl;
//            char* newBlock = new char[BLOCK_SIZE *  sz];
//
//            char* cur = newBlock + sz;
//            for(int i = 1; i < BLOCK_SIZE - 1; i++)
//            {
//                ((obj *) cur)->_next = (obj *) (cur + sz);
//                cur += sz;
//            }
//            ((obj*)cur)->_next = nullptr;
//
//            p = newBlock;
//            _freeList = (obj*)(newBlock + sz);
//        }
//
//        return p;
//    }
//    void allocator::deallocate(void* ptr, size_t sz)
//    {
//        if(sz < sizeof(obj))
//            return ::operator delete(ptr);
//
//        ((obj*)ptr)->_next = _freeList;
//        _freeList = (obj*)ptr;
//    }
//
//}
#include<type_traits>
/*
 * STL 两级空间配置器实现
 */
namespace MYSTL
{
    template<int init>
    class pool_alloc_template;

    typedef pool_alloc_template<0> pool_alloc;

    // Alloc 是一个空类，继承空类实行 空基类优化（EBO），allocator 不占额外内存
    template<typename Tp, typename Alloc = std::allocator<Tp>>
    class allocator : public Alloc
    {
    public:
        typedef Tp         value_type;
        typedef size_t     size_type;
        typedef ptrdiff_t  difference_type;
        typedef Tp*        pointer;
        typedef const Tp*  const_pointer;
        typedef Tp&        reference;
        typedef const Tp&  const_reference;

        Tp* allocate(size_t n)
        {
            // std::allocator<Tp> 的 allocator 参数为 元素个数
            // pool_alloc 参数为字节数
            n = std::is_same<pool_alloc, Alloc>::value ? n * sizeof(Tp) : n;
            return static_cast<Tp*>( Alloc::allocate( n ) );
        }

        void deallocate(Tp* ptr,  size_t n)
        {
            n = std::is_same<pool_alloc, Alloc>::value ? n * sizeof(Tp) : n;
            Alloc::deallocate( ptr, n );
        }

        // Other Alloc
        template <class U, class OAlloc>
        struct rebind
        {
            typedef allocator<U, OAlloc> other;
        };

        pointer address(reference x)
        { return  std::__addressof(x); }

        const_pointer address(const_reference x)
        { return std::__addressof(x); }

        void construct(pointer p, const Tp& value)
        { MYSTL::construct(p, value); }

        void destroy(pointer p)
        { MYSTL::destroy(p); }

        size_type max_size() const
        { return UINT_MAX / sizeof(Tp); }

    };

    template<int init = 0>
    class malloc_alloc_template
    {
    public:
        static void* allocate(size_t sz)
        {
#ifdef DO_MYSTL_TEST
            cout << "[malloc alloc] size = " << sz << endl;
#endif
            return ::operator new(sz);
        }

        static void deallocate(void* ptr, size_t sz)
        {
#ifdef DO_MYSTL_TEST
            cout << "[malloc dealloc] ptr = " << ptr << " size = " << sz << endl;
#endif
            ::operator delete(ptr);
        }
    };

    typedef malloc_alloc_template<0>  malloc_alloc;

    template<int init = 0>
    class pool_alloc_template
    {
    private:
        enum { _ALIGN = 8 };
        enum { _MAX_BYTES = 128 };
        enum { _NFREELISTS = 16 };

        static size_t
        _S_round_up(size_t bytes)
        { return (bytes + _ALIGN - 1) & ~(_ALIGN - 1); }

    private:
        union _Obj {
            _Obj* _M_free_list_next;
            char _M_client_data[1]; // client sees this
        };

        static _Obj* volatile _S_free_list[_NFREELISTS];

        static char*  _S_start_free;
        static char*  _S_end_free;
        static size_t  _S_heap_size;

        static std::mutex _S_mutex;

        static size_t _S_free_list_index(size_t bytes)
        { return (bytes + _ALIGN - 1) / _ALIGN - 1; }

        static void* _S_refill(size_t n);

        static char* _S_chuck_alloc(size_t sz, int& nobjs);

    public:
        static void* allocate(size_t sz)
        {
            if(sz > _MAX_BYTES)
                return malloc_alloc::allocate(sz);

            size_t round_up = _S_round_up(sz);
            size_t idx = _S_free_list_index(round_up);

            _Obj* volatile *free_list_item = _S_free_list + idx;

            std::unique_lock<std::mutex> lock(_S_mutex);
            void* p = (void*)*free_list_item;
            if(p)
            {
                // 这块必须用二级指针去改
                *free_list_item = (*free_list_item)->_M_free_list_next;
#ifdef DO_MYSTL_TEST
                cout << "[pool alloc] ask for [" << sz << "] bytes, round up : [" << round_up << "], success!" << endl;
#endif
            }
            else
            {
#ifdef DO_MYSTL_TEST
                cout << "[pool alloc] ask for [" << sz << "] bytes, round up : [" << round_up << "], refill!" << endl;
#endif
                p = _S_refill(round_up);
            }

            return p;
        }

        static void deallocate(void* ptr, size_t sz)
        {
            if(sz > _MAX_BYTES)
                return malloc_alloc::deallocate(ptr, sz);

#ifdef DO_MYSTL_TEST
            cout << "[pool dealloc] ptr = " << ptr << " size = " << sz << endl;
#endif

            size_t idx = _S_free_list_index(sz);
            _Obj* volatile *pfree_list_item = _S_free_list + idx;

            std::unique_lock<std::mutex> lock(_S_mutex);

            _Obj* new_head = (_Obj*)ptr;
            new_head->_M_free_list_next = *pfree_list_item;
            *pfree_list_item = new_head;

        }
    };

    template<int init>
    typename pool_alloc_template<init>::_Obj* volatile
    pool_alloc_template<init>::_S_free_list[_NFREELISTS] = { nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
        nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
    };


    template<int init>
    char*  pool_alloc_template<init>::_S_start_free = nullptr;


    template<int init>
    char*  pool_alloc_template<init>::_S_end_free   = nullptr;

    template<int init>
    size_t  pool_alloc_template<init>::_S_heap_size = 0;


    template<int init>
    std::mutex pool_alloc_template<init>::_S_mutex;

    template<int init>
    void* pool_alloc_template<init>::_S_refill(size_t n)
    {
        size_t idx = _S_free_list_index(n);

        int nobjs = 20;

#ifdef DO_MYSTL_TEST
        cout << "============= expand memory =============" << endl;
#endif

        char* start = _S_chuck_alloc(n, nobjs);

#ifdef DO_MYSTL_TEST
        cout << "=========================================" << endl;
#endif

        _Obj* head = (_Obj*)(start + n);
        _Obj* volatile *pfree_list_item = _S_free_list + idx;
        *pfree_list_item = head;

        for(int i = 1; i < nobjs - 1; ++i)
        {
            _Obj* next = (_Obj*)((char*)head + n);
            head->_M_free_list_next = next;
            head = next;
        }
        head->_M_free_list_next = nullptr;

        return start;
    }

    template<int init>
    char* pool_alloc_template<init>::_S_chuck_alloc(size_t sz, int& nobjs)
    {
        char* res;
        size_t total_bytes = sz * nobjs;
        size_t left_bytes  = _S_end_free - _S_start_free;

#ifdef DO_MYSTL_TEST
        cout << "sz = " << sz << endl;
        cout << "total_bytes: " << total_bytes << " " << "left_bytes: " << left_bytes << endl;
#endif

        if(left_bytes >= total_bytes)
        {
            res = _S_start_free;
            _S_start_free += total_bytes;
            return res;
        }

        if(left_bytes >= sz)
        {
            res = _S_start_free;
            nobjs = (int)(left_bytes / sz);
            _S_start_free += nobjs * sz;
            return res;
        }

        // 将备用池中剩下的字节放入 free_list 中
        if(left_bytes > 0)
        {
            _Obj* volatile *pfree_list_item = _S_free_list + _S_free_list_index(left_bytes);
            _Obj* new_head = (_Obj*)_S_start_free;
            new_head->_M_free_list_next = *pfree_list_item;
            *pfree_list_item = new_head;
        }

        total_bytes = sz * nobjs * 2 + _S_round_up(_S_heap_size >> 4);

        _S_start_free = (char*)malloc(total_bytes);
        if(!_S_start_free)
        {
            for(size_t i = sz + 8; i <= _MAX_BYTES; i += 8)
            {
                _Obj* volatile *pfree_list_item = _S_free_list + _S_free_list_index(i);
                if(*pfree_list_item == nullptr) continue;
                _S_start_free = (char*)*pfree_list_item;
                _S_end_free   = _S_start_free + i;
                *pfree_list_item = (*pfree_list_item)->_M_free_list_next;
                return _S_chuck_alloc(sz, nobjs);
            }
            _S_end_free = nullptr;
            _S_start_free = (char*)malloc_alloc::allocate(total_bytes);
            // 如果 alloc_alloc::allocate 调用失败则会抛出异常
        }

        _S_heap_size += total_bytes;
        _S_end_free = _S_start_free + total_bytes;


        return _S_chuck_alloc(sz, nobjs);
    }


}

/*
 * loki::SmallObjAllocator
 */
namespace MYSTL
{
#ifndef LOKI_DEFAULT_CHUNK_SIZE
#define LOKI_DEFAULT_CHUNK_SIZE 4096
#endif

#ifndef LOKI_MAX_SMALL_OBJECT_SIZE
#define LOKI_MAX_SMALL_OBJECT_SIZE 256
#endif

#ifndef LOKI_DEFAULT_OBJECT_ALIGNMENT
#define LOKI_DEFAULT_OBJECT_ALIGNMENT 4
#endif


    class Chunk
    {
    private:
        template<typename Ctnr>
        friend class FixedAllocator;

        /** Initializes a just-constructed Chunk.
          @param blockSize Number of bytes per block.
          @param blocks Number of blocks per Chunk.
          @return True for success, false for failure.
          */
        bool Init( std::size_t blockSize, unsigned char blocks );

        /** Allocate a block within the Chunk.  Complexity is always O(1), and
        this will never throw.  Does not actually "allocate" by calling
        malloc, new, or any other function, but merely adjusts some internal
        indexes to indicate an already allocated block is no longer available.
        @return Pointer to block within Chunk.
        */
        void* Allocate( std::size_t blockSize );


        /** Deallocate a block within the Chunk. Complexity is always O(1), and
         this will never throw.  For efficiency, this assumes the address is
         within the block and aligned along the correct byte boundary.  An
         assertion checks the alignment, and a call to HasBlock is done from
         within VicinityFind.  Does not actually "deallocate" by calling free,
         delete, or other function, but merely adjusts some internal indexes to
         indicate a block is now available.
         */
        void  Deallocate( void* p, std::size_t blockSize );

       /** Resets the Chunk back to pristine values. The available count is
        set back to zero, and the first available index is set to the zeroth
        block.  The stealth indexes inside each block are set to point to the
        next block. This assumes the Chunk's data was already using Init.
        */
        void Reset( std::size_t blockSize, unsigned char blocks );


        /// Releases the allocated block of memory.
        void Release();

        inline bool HasBlock( void* p, std::size_t chunkLength ) const
        {
            unsigned char* pc = static_cast<unsigned char*>(p);
            return (pc >= _pData) && (pc < _pData + chunkLength);
        }

        inline bool HasAvailable( unsigned char numBlock ) const
        { return numBlock == _blocksAvailable; }

        inline bool IsFilled() const
        {
            return 0 == _blocksAvailable;
        }


        /// Pointer to array of allocated blocks.
        unsigned char* _pData;
        /// Index of first empty block.
        unsigned char _firstAvailableBlock;
        /// Count of empty blocks.
        unsigned char _blocksAvailable;
    };

/**
 *     Offers services for allocating fixed-sized objects.  It has a container
 *     of "containers" of fixed-size blocks.  The outer container has all the
 *     Chunks.  The inner container is a Chunk which owns some blocks.
 */

    template<typename Ctnr>
    class FixedAllocator
    {
    public:
        typedef Ctnr Chunks;
        typedef typename Chunks::iterator ChunkIter;
        typedef typename Chunks::const_iterator ChunkCIter;
    private:
        /** Deallocates the block at address p, and then handles the internal
        bookkeeping needed to maintain class invariants.  This assumes that
        deallocChunk_ points to the correct chunk.
        */
        void DoDeallocate( void* p );

        /** Creates an empty Chunk and adds it to the end of the ChunkList.
        All calls to the lower-level memory allocation functions occur inside
        this function, and so the only try-catch block is inside here.
        @return true for success, false for failure.
        */
        bool MakeNewChunk( void );

        /** Finds the Chunk which owns the block at address p.  It starts at
         deallocChunk_ and searches in both forwards and backwards directions
         from there until it finds the Chunk which owns p.  This algorithm
         should find the Chunk quickly if it is deallocChunk_ or is close to it
         in the Chunks container.  This goes both forwards and backwards since
         that works well for both same-order and opposite-order deallocations.
         (Same-order = objects are deallocated in the same order in which they
         were allocated.  Opposite order = objects are deallocated in a last to
         first order.  Complexity is O(C) where C is count of all Chunks.  This
         never throws.
         @return Pointer to Chunk that owns p, or NULL if no owner found.
         */
        Chunk* VicinityFind( void * p ) const;


        FixedAllocator(const FixedAllocator&);
        FixedAllocator& operator=(const FixedAllocator&);

        /// Fewest # of objects managed by a Chunk.
        static unsigned char _S_MinObjectsPerChunk;
        /// Most # of objects managed by a Chunk - never exceeds UCHAR_MAX.
        static unsigned char _S_MaxObjectsPerChunk;

        /// Number of bytes in a single block within a Chunk.
        std::size_t _blockSize;
        /// Number of blocks managed by each Chunk.
        unsigned char _numBlocks;

        /// Container of Chunks.
        Chunks _chunks;
        /// Pointer to Chunk used for last or next allocation.
        Chunk* _allocChunk;
        /// Pointer to Chunk used for last or next deallocation.
        Chunk* _deallocChunk;
        /// Pointer to the only empty Chunk if there is one, else NULL.
        Chunk* _emptyChunk;

    public:
        /// Create a FixedAllocator which manages blocks of 'blockSize' size.
        FixedAllocator()
            : _blockSize(0),
              _numBlocks(0),
              _chunks(Ctnr()),
              _allocChunk(nullptr),
              _deallocChunk(nullptr),
              _emptyChunk(nullptr)
        {}

        /// Destroy the FixedAllocator and release all its Chunks.
        ~FixedAllocator()
        {
            for(ChunkIter i(_chunks.begin()); i != _chunks.end(); ++i)
                i->Release();
        }

        /// Initializes a FixedAllocator by calculating # of blocks per Chunk.
        void Initialize( std::size_t blockSize, std::size_t pageSize );

        /** Returns count of empty Chunks held by this allocator.  Complexity
        is O(C) where C is the total number of Chunks - empty or used.
        */
        std::size_t CountEmptyChunks( void ) const;

        /** Returns pointer to allocated memory block of fixed size - or NULL
         if it failed to allocate.
         */
        void * Allocate( void );

        /** Deallocate a memory block previously allocated with Allocate.  If
         the block is not owned by this FixedAllocator, it returns false so
        that SmallObjAllocator can call the default deallocator.  If the
         block was found, this returns true.
        */
        bool Deallocate( void * p, Chunk * hint );

        /// Returns block size with which the FixedAllocator was initialized.
        inline std::size_t BlockSize() const { return _blockSize; }

        const Chunk* HasBlock( void* p ) const;
        inline Chunk* HasBlock( void* p )
        {
            return const_cast<Chunk*>(
                    const_cast<const FixedAllocator*>(this)->HasBlock(p)
                    );
        }

        inline ChunkIter end() const
        { return _chunks.end(); }
    };

    template<typename Ctnr>
    unsigned char FixedAllocator<Ctnr>::_S_MinObjectsPerChunk = 8;
    template<typename Ctnr>
    unsigned char FixedAllocator<Ctnr>::_S_MaxObjectsPerChunk = UCHAR_MAX;

    template<typename Ctnr>
    void FixedAllocator<Ctnr>::Initialize( std::size_t blockSize,
                                           std::size_t pageSize )
    {
        assert(blockSize > 0);
        assert(pageSize >= blockSize);
        _blockSize = blockSize;

        size_t numBlocks = pageSize / blockSize;
        if(numBlocks < _S_MinObjectsPerChunk) numBlocks = _S_MinObjectsPerChunk;
        if(numBlocks > _S_MaxObjectsPerChunk) numBlocks = _S_MaxObjectsPerChunk;

        _numBlocks = static_cast<unsigned char>(numBlocks);
        assert(_numBlocks == numBlocks);
    }

    // 直接用 ChunkIter 静态分析会报错
    template<typename Ctnr>
    const Chunk*
    FixedAllocator<Ctnr>::HasBlock( void* p ) const
    {
        std::size_t chunkLength = _numBlocks * _blockSize;
        // 这里是 ChunkCIter 而不是 ChunkIter
        for(ChunkCIter i(_chunks.begin()); i != _chunks.end(); ++i)
        {
            if( i->HasBlock(p, chunkLength) )
                return &*i;
        }
        return nullptr;
    }

    template<typename Ctnr>
    bool FixedAllocator<Ctnr>::MakeNewChunk()
    {
        Chunk newChunk;
        // Init 调用 operator new 分配内存，所以如果 Init 可以返回，则一定拿到了内存
        // 否则 operator new 内会抛出异常
        bool allocated =  newChunk.Init(_blockSize, _numBlocks);
        try
        {
            _chunks.push_back(newChunk);
        }
        catch(...)
        {
            allocated = false;
            newChunk.Release();
        }

        if(!allocated) return false;

        // push_back 可能会引发增容，迭代器可能会失效
       _deallocChunk = &_chunks.front();
       _allocChunk   = &_chunks.back();

       return true;
    }

    template<typename Ctnr>
    std::size_t FixedAllocator<Ctnr>::CountEmptyChunks() const
    {
#ifdef DO_MYSTL_TEST
        std::size_t cnt = 0;
        for(ChunkCIter i(_chunks.begin()); i != _chunks.end(); ++i)
            if(i->HasAvailable(_numBlocks))
                cnt++;
        return cnt;
#else
        return _emptyChunk == nullptr ? 0 : 1;
#endif
    }

    template<typename Ctnr>
    void* FixedAllocator<Ctnr>::Allocate()
    {
        assert((_emptyChunk == nullptr) || (_emptyChunk->HasAvailable(_numBlocks)));
        assert(CountEmptyChunks() < 2);

        if( (_allocChunk == nullptr) || (_allocChunk->IsFilled()) )
        {
            if( nullptr != _emptyChunk )
            {
                _allocChunk = _emptyChunk;
                _emptyChunk = nullptr;
            }
            else
            {
                for( ChunkIter i(_chunks.begin()); ; i++)
                {
                    if(i == _chunks.end())
                    {
                        if(!MakeNewChunk())
                            return nullptr;
                        _allocChunk = &_chunks.back();
                        break;
                    }
                    if(!i->IsFilled())
                    {
                        _allocChunk = &*i;
                        break;
                    }
                }
            }
        }
        else if(_allocChunk == _emptyChunk)
            _emptyChunk = nullptr;

#ifdef DO_MYSTL_TEST
        if(_allocChunk->IsFilled())
            std::cout << "_blockSize = " << _blockSize << " numBlocks = " << (int)_numBlocks << endl;
#endif

        assert(_allocChunk != nullptr);
        assert(!_allocChunk->IsFilled());
        assert((_emptyChunk == nullptr) || (_emptyChunk->HasAvailable(_numBlocks)));
        assert(CountEmptyChunks() < 2);

        return _allocChunk->Allocate(_blockSize);
    }

    template<typename Ctnr>
    bool FixedAllocator<Ctnr>::Deallocate( void * p, Chunk* hint )
    {
        assert(!_chunks.empty());
        assert(&_chunks.front() <= _deallocChunk);
        assert(&_chunks.back()  >=  _deallocChunk);
        assert(CountEmptyChunks() < 2);

        Chunk* findChunk = hint == nullptr ? VicinityFind(p) : hint;
        if(findChunk == nullptr) return false;

        assert(findChunk->HasBlock(p, _numBlocks * _blockSize));

        _deallocChunk = findChunk;
        DoDeallocate(p);

        assert(CountEmptyChunks() < 2);

        return true;
    }

    // 迭代器必须支持使用随机访问
    template<typename Ctnr>
    Chunk* FixedAllocator<Ctnr>::VicinityFind( void * p ) const
    {
        assert(_deallocChunk);

        const std::size_t chunkLength = _numBlocks * _blockSize;
        Chunk* lo = _deallocChunk;
        Chunk* hi = _deallocChunk + 1;
        const Chunk* loBound = &_chunks.front();
        const Chunk* hiBound = &_chunks.back() + 1;

        for(;;)
        {
            if(lo != nullptr)
            {
                if (lo->HasBlock(p, chunkLength)) return lo;
                if (lo == loBound)
                {
                    lo = nullptr;
                    if(hi == nullptr) break;
                }
                else
                    --lo;
            }

            if(hi != nullptr)
            {
                if (hi->HasBlock(p, chunkLength)) return hi;
                if (++hi == hiBound)
                {
                    hi = nullptr;
                    if(lo == nullptr) break;
                }

            }
        }

        return nullptr;
    }


    // 进入这个函数时，_deallocate 指向 p 所属的 Chunk
    template<typename Ctnr>
    void FixedAllocator<Ctnr>::DoDeallocate( void* p )
    {
        assert(_deallocChunk->HasBlock(p, _numBlocks * _blockSize));
        assert( ( nullptr == _emptyChunk ) || ( _emptyChunk->HasAvailable( _numBlocks ) ) );

        _deallocChunk->Deallocate(p, _blockSize);

        if( _deallocChunk->HasAvailable(_numBlocks) )
        {
            assert(_emptyChunk != _deallocChunk);

            if(_emptyChunk != nullptr)
            {
                Chunk* lastChunk = &_chunks.back();
                if(lastChunk == _deallocChunk)
                    _deallocChunk = _emptyChunk;
                else if(lastChunk != _emptyChunk)
                    std::swap(*lastChunk, *_emptyChunk);

                lastChunk->Release();
                _chunks.pop_back();

                if((_allocChunk == lastChunk) || (_allocChunk->IsFilled()))
                    _allocChunk = _deallocChunk;
            }

            _emptyChunk = _deallocChunk;
        }
    }



    /// @ingroup SmallObjectGroupInternal
    /// Calculates index into array where a FixedAllocator of numBytes is located.
    // 向上取正数，round up 到 alignment 整数倍
    inline std::size_t GetOffSet(std::size_t numBytes, std::size_t alignment)
    { return (numBytes + alignment - 1) / alignment; }

    /** @ingroup SmallObjectGroupInternal
    Calls the default allocator when SmallObjAllocator decides not to handle a
    request.  SmallObjAllocator calls this if the number of bytes is bigger than
    the size which can be handled by any FixedAllocator.
    @param numBytes number of bytes
    @param doThrow True if this function should throw an exception, or false if it
    should indicate failure by returning a NULL pointer.
    */
    inline void* DefaultAllocate( std::size_t numBytes, bool doThrow )
    {
        return doThrow ? ::operator new(numBytes)
                       : ::operator new(numBytes, std::nothrow_t());
    }

    /** @ingroup SmallObjectGroupInternal
    Calls default deallocator when SmallObjAllocator decides not to handle a
    request.  The default deallocator could be the global delete operator or the
    free function.  The free function is the preferred default deallocator since
    it matches malloc which is the preferred default allocator.  SmallObjAllocator
    will call this if an address was not found among any of its own blocks.
    */
    inline void DefaultDeallocate( void* p )
    {
        return ::operator delete(p);
    }


    template<typename Ctnr = std::vector<Chunk>>
    class SmallObjAllocator
    {
    public:
        /** The only available constructor needs certain parameters in order to
        initialize all the FixedAllocator's.  This throws only if
        @param pageSize # of bytes in a page of memory.
        @param maxObjectSize Max # of bytes which this may allocate.
        @param objectAlignSize # of bytes between alignment boundaries.
        */
        SmallObjAllocator(std::size_t pageSize, std::size_t maxObjectSize,
                          std::size_t objectAlignSize );

        /** Destructor releases all blocks, all Chunks, and FixedAllocator's.
          Any outstanding blocks are unavailable, and should not be used after
          this destructor is called.  The destructor is deliberately non-virtual
          because it is protected, not public.
          */
        ~SmallObjAllocator();

        /** Allocates a block of memory of requested size.  Complexity is often
         constant-time, but might be O(C) where C is the number of Chunks in a
         FixedAllocator.

         @par Exception Safety Level
         Provides either strong-exception safety, or no-throw exception-safety
         level depending upon doThrow parameter.  The reason it provides two
         levels of exception safety is because it is used by both the nothrow
         and throwing new operators.  The underlying implementation will never
         throw of its own accord, but this can decide to throw if it does not
         allocate.  The only exception it should emit is std::bad_alloc.

         @par Allocation Failure
         If it does not allocate, it will call TrimExcessMemory and attempt to
         allocate again, before it decides to throw or return NULL.  Many
         allocators loop through several new_handler functions, and terminate
         if they can not allocate, but not this one.  It only makes one attempt
         using its own implementation of the new_handler, and then returns NULL
         or throws so that the program can decide what to do at a higher level.
         (Side note: Even though the C++ Standard allows allocators and
         new_handlers to terminate if they fail, the Loki allocator does not do
         that since that policy is not polite to a host program.)

         @param size # of bytes needed for allocation.
         @param doThrow True if this should throw if unable to allocate, false
          if it should provide no-throw exception safety level.
         @return NULL if nothing allocated and doThrow is false.  Else the
          pointer to an available block of memory.
         */
        void* Allocate( std::size_t numBytes, bool doThrow );

        /** Deallocates a block of memory at a given place and of a specific
        size.  Complexity is almost always constant-time, and is O(C) only if
        it has to search for which Chunk deallocates.  This never throws.
         */
        void Deallocate( void* p, std::size_t size );

        /** Deallocates a block of memory at a given place but of unknown size
        size.  Complexity is O(F + C) where F is the count of FixedAllocator's
        in the pool, and C is the number of Chunks in all FixedAllocator's.  This
        does not throw exceptions.  This overloaded version of Deallocate is
        called by the nothow delete operator - which is called when the nothrow
        new operator is used, but a constructor throws an exception.
         */
        void Deallocate( void* p );

        inline std::size_t GetMaxObjectSize() const
        { return  _maxSmallObjectSize; };

        inline std::size_t GetAlignment() const
        { return _objectAlignSize; }


    private:

        SmallObjAllocator();
        SmallObjAllocator( const SmallObjAllocator& );
        SmallObjAllocator& operator=(const SmallObjAllocator&);

        /// Pointer to array of fixed-size allocators.
        MYSTL::FixedAllocator<Ctnr>* _pool;

        /// Largest object size supported by allocators.
        const std::size_t _maxSmallObjectSize;

        /// Size of alignment boundaries.
        const std::size_t _objectAlignSize;
    };

    template<typename Ctnr>
    SmallObjAllocator<Ctnr>::SmallObjAllocator(
            std::size_t pageSize,
            std::size_t maxObjectSize,
            std::size_t objectAlignSize )
            :_pool(nullptr)
            ,_maxSmallObjectSize(maxObjectSize)
            ,_objectAlignSize(objectAlignSize)
    {
        assert( 0 != objectAlignSize );

        std::size_t allocCount = GetOffSet(maxObjectSize, objectAlignSize);

        _pool = new FixedAllocator<Ctnr>[ allocCount ];
        for(int i = 0; i < allocCount; ++i)
            _pool[i].Initialize((i + 1) * objectAlignSize, pageSize);
    }

    template<typename Ctnr>
    SmallObjAllocator<Ctnr>::~SmallObjAllocator()
    {
        delete[] _pool;
    }

    template<typename Ctnr>
    void*  SmallObjAllocator<Ctnr>::Allocate(
            std::size_t numBytes, bool doThrow )
    {
        if(numBytes > GetMaxObjectSize())
            return DefaultAllocate(numBytes, doThrow);

        if(0 == numBytes)
            numBytes = 1;

        std::size_t index = GetOffSet(numBytes, GetAlignment()) - 1;
        std::size_t allocCount = GetOffSet(GetMaxObjectSize(),
                                           GetAlignment());
        assert(index < allocCount);
        assert(nullptr != _pool);

        FixedAllocator<Ctnr>& allocator = _pool[index];

        assert(allocator.BlockSize() >= numBytes);
        assert(allocator.BlockSize() < numBytes + GetAlignment());

        void* place = allocator.Allocate();

        if((place == nullptr) && doThrow)
            throw std::bad_alloc();

        return place;
    }

    template<typename Ctnr>
    void SmallObjAllocator<Ctnr>::Deallocate(
                    void* p, std::size_t size )
    {
        if(!p) return;
        assert(size != 0);

        if(size > GetMaxObjectSize())
            return DefaultDeallocate(p);

        std::size_t index = GetOffSet(size, GetAlignment()) - 1;
        std::size_t allocCount = GetOffSet(GetMaxObjectSize(), GetAlignment());

        assert(index < allocCount);
        assert(nullptr != _pool);

        FixedAllocator<Ctnr>& deAllocator = _pool[index];

        assert(deAllocator.BlockSize() >= size);
        assert(deAllocator.BlockSize() < size + GetAlignment());

        bool found = deAllocator.Deallocate(p, nullptr);

        assert(found);
    }

    template<typename Ctnr>
    void SmallObjAllocator<Ctnr>::Deallocate( void* p )
    {
        if(!p) return;

        assert(_pool != nullptr);

        FixedAllocator<Ctnr>* pDeallocator = nullptr;
        Chunk* chunk = nullptr;
        std::size_t allocCount = GetOffSet(GetMaxObjectSize(), GetAlignment());

        for(int i = 0; i < allocCount; ++i)
        {
            chunk =  _pool[i].HasBlock(p);
            if(chunk != nullptr)
            {
                pDeallocator = &_pool[i];
                break;
            }
        }

        if(chunk == nullptr)
            return DefaultDeallocate(p);

        assert(chunk != nullptr);

        bool found = pDeallocator->Deallocate(p, chunk);
        assert(found);
    }
}


#endif //MEMORY_MANAGE_ALLOCATOR_H