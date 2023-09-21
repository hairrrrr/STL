//
// Created by 78172 on 2023/3/20.
//
#ifndef _MYSTL_STRING_
#define _MYSTL_STRING_

#include<atomic>
#include<assert.h>
#include<iterator>
#include<string>
#include<limits>
#include<cstring>

#define IS_LITTLE_ENDIAN (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define FBSTRING_DISABLE_SSO false

namespace fbstring_detail {

    /**
 * @brief Reallocs if there is less slack in the buffer, else performs
 * malloc-copy-free.
 *
 * This function tries to reallocate a buffer of which only the first
 * currentSize bytes are used. The problem with using realloc is that
 * if currentSize is relatively small _and_ if realloc decides it
 * needs to move the memory chunk to a new buffer, then realloc ends
 * up copying data that is not used. It's generally not a win to try
 * to hook in to realloc() behavior to avoid copies - at least in
 * jemalloc, realloc() almost always ends up doing a copy, because
 * there is little fragmentation / slack space to take advantage of.
 *
 * @param p Pointer to start of buffer
 * @param currentSize Current used size
 * @param currentCapacity Capacity of buffer
 * @param newCapacity New capacity for the buffer
 *
 * @return pointer to realloc'ed buffer
 */
    inline void* smartRealloc(
            void* p,
            const size_t currentSize,
            const size_t currentCapacity,
            const size_t newCapacity) {
        assert(p);
        assert(currentSize <= currentCapacity && currentCapacity < newCapacity);

        auto const slack = currentCapacity - currentSize;
        if (slack * 2 > currentSize) {
            // Too much slack, malloc-copy-free cycle:
            auto const result = malloc(newCapacity);
            std::memcpy(result, p, currentSize);
            free(p);
            return result;
        }
        // If there's not too much slack, we realloc in hope of coalescing
        return realloc(p, newCapacity);
    }

    template <class InIt, class OutIt>
    inline std::pair<InIt, OutIt> copy_n(
            InIt b, typename std::iterator_traits<InIt>::difference_type n, OutIt d) {
        for (; n != 0; --n, ++b, ++d) {
            *d = *b;
        }
        return std::make_pair(b, d);
    }

    template <class Pod, class T>
    inline void podFill(Pod* b, Pod* e, T c) {
        assert(b && e && b <= e);
        constexpr auto kUseMemset = sizeof(T) == 1;
        if /* constexpr */ (kUseMemset) {
            memset(b, c, size_t(e - b));
        } else {
            auto const ee = b + ((e - b) & ~7u);
            for (; b != ee; b += 8) {
                b[0] = c;
                b[1] = c;
                b[2] = c;
                b[3] = c;
                b[4] = c;
                b[5] = c;
                b[6] = c;
                b[7] = c;
            }
            // Leftovers
            for (; b != e; ++b) {
                *b = c;
            }
        }
    }

/*
 * Lightly structured memcpy, simplifies copying PODs and introduces
 * some asserts. Unfortunately using this function may cause
 * measurable overhead (presumably because it adjusts from a begin/end
 * convention to a pointer/size convention, so it does some extra
 * arithmetic even though the caller might have done the inverse
 * adaptation outside).
 */
    template <class Pod>
    inline void podCopy(const Pod* b, const Pod* e, Pod* d) {
        assert(b != nullptr);
        assert(e != nullptr);
        assert(d != nullptr);
        assert(e >= b);
        assert(d >= e || d + (e - b) <= b);
        memcpy(d, b, (e - b) * sizeof(Pod));
    }

/*
 * Lightly structured memmove, simplifies copying PODs and introduces
 * some asserts
 */
    template <class Pod>
    inline void podMove(const Pod* b, const Pod* e, Pod* d) {
        assert(e >= b);
        memmove(d, b, (e - b) * sizeof(*b));
    }
} // namespace fbstring_detail

namespace MYSTL
{
/**
* Defines a special acquisition method for constructing fbstring
* objects. AcquireMallocatedString means that the user passes a
* pointer to a malloc-allocated string that the fbstring object will
* take into custody.
*/
    enum class AcquireMallocatedString {};

    template<typename Char>
    class fbstring_core
    {
    public:
        fbstring_core() noexcept { reset(); }

        fbstring_core(const fbstring_core& rhs) {
            assert(&rhs != this);
            switch (rhs.category()) {
                case Category::isSmall:
                    copySmall(rhs);
                    break;
                case Category::isMedium:
                    copyMedium(rhs);
                    break;
                case Category::isLarge:
                    copyLarge(rhs);
                    break;
                default:
                    __builtin_unreachable();
            }
            assert(size() == rhs.size());
            assert(memcmp(data(), rhs.data(), size() * sizeof(Char)) == 0);
        }

        fbstring_core& operator=(const fbstring_core& rhs) = delete;

        fbstring_core(fbstring_core&& goner) noexcept {
            // Take goner's guts
            ml_ = goner.ml_;
            // Clean goner's carcass
            goner.reset();
        }

        fbstring_core(
                const Char* const data,
                const size_t size,
                bool disableSSO = FBSTRING_DISABLE_SSO) {
            if (!disableSSO && size <= maxSmallSize) {
                initSmall(data, size);
            } else if (size <= maxMediumSize) {
                initMedium(data, size);
            } else {
                initLarge(data, size);
            }
            assert(this->size() == size);
            assert(size == 0 || memcmp(this->data(), data, size * sizeof(Char)) == 0);
        }

        ~fbstring_core() noexcept {
            if (category() == Category::isSmall) {
                return;
            }
            destroyMediumLarge();
        }

//     Snatches a previously mallocated string. The parameter "size"
//     is the size of the string, and the parameter "allocatedSize"
//     is the size of the mallocated block.  The string must be
//     \0-terminated, so allocatedSize >= size + 1 and data[size] == '\0'.
//
//     So if you want a 2-character string, pass malloc(3) as "data",
//     pass 2 as "size", and pass 3 as "allocatedSize".
        fbstring_core(
                Char* const data,
                const size_t size,
                const size_t allocatedSize,
                AcquireMallocatedString) {
            if (size > 0) {
                assert(allocatedSize >= size + 1);
                assert(data[size] == '\0');
                // Use the medium string storage
                ml_.data_ = data;
                ml_.size_ = size;
                // Don't forget about null terminator
                ml_.setCapacity(allocatedSize - 1, Category::isMedium);
            } else {
                // No need for the memory
                free(data);
                reset();
            }
        }

        // swap below doesn't test whether &rhs == this (and instead
        // potentially does extra work) on the premise that the rarity of
        // that situation actually makes the check more expensive than is
        // worth.
        void swap(fbstring_core& rhs) {
            auto const t = ml_;
            ml_ = rhs.ml_;
            rhs.ml_ = t;
        }

        // In C++11 data() and c_str() are 100% equivalent.
        const Char* data() const { return c_str(); }

        Char* data() { return c_str(); }

        Char* mutableData() {
            switch (category()) {
                case Category::isSmall:
                    return small_;
                case Category::isMedium:
                    return ml_.data_;
                case Category::isLarge:
                    return mutableDataLarge();
            }
            __builtin_unreachable();
        }

        const Char* c_str() const {
            const Char* ptr = ml_.data_;
            // With this syntax, GCC and Clang generate a CMOV instead of a branch.
            ptr = (category() == Category::isSmall) ? small_ : ptr;
            return ptr;
        }

        void shrink(const size_t delta) {
            if (category() == Category::isSmall) {
                shrinkSmall(delta);
            } else if (
                    category() == Category::isMedium || RefCounted::refs(ml_.data_) == 1) {
                shrinkMedium(delta);
            } else {
                shrinkLarge(delta);
            }
        }


        void reserve(size_t minCapacity, bool disableSSO = FBSTRING_DISABLE_SSO) {
            switch (category()) {
                case Category::isSmall:
                    reserveSmall(minCapacity, disableSSO);
                    break;
                case Category::isMedium:
                    reserveMedium(minCapacity);
                    break;
                case Category::isLarge:
                    reserveLarge(minCapacity);
                    break;
                default:
                    __builtin_unreachable();
            }
            assert(capacity() >= minCapacity);
        }

        Char* expandNoinit(
                const size_t delta,
                bool expGrowth = false,
                bool disableSSO = FBSTRING_DISABLE_SSO);

        void push_back(Char c) { *expandNoinit(1, /* expGrowth = */ true) = c; }


        size_t size() const {
            size_t ret = ml_.size_;
            if /* constexpr */ (IS_LITTLE_ENDIAN) {
                // We can save a couple instructions, because the category is
                // small iff the last char, as unsigned, is <= maxSmallSize.
                typedef typename std::make_unsigned<Char>::type UChar;
                auto maybeSmallSize = size_t(maxSmallSize) -
                                      size_t(static_cast<UChar>(small_[maxSmallSize]));
                // With this syntax, GCC and Clang generate a CMOV instead of a branch.
                ret = (static_cast<ssize_t>(maybeSmallSize) >= 0) ? maybeSmallSize : ret;
            } else {
                ret = (category() == Category::isSmall) ? smallSize() : ret;
            }
            return ret;
        }

        size_t capacity() const {
            switch (category()) {
                case Category::isSmall:
                    return maxSmallSize;
                case Category::isLarge:
                    // For large-sized strings, a multi-referenced chunk has no
                    // available capacity. This is because any attempt to append
                    // data would trigger a new allocation.
                    if (RefCounted::refs(ml_.data_) > 1) {
                        return ml_.size_;
                    }
                    break;
                case Category::isMedium:
                default:
                    break;
            }
            return ml_.capacity();
        }
    private:

        void reset() { setSmallSize(0); }

         void destroyMediumLarge() noexcept {
            auto const c = category();
            assert(c != Category::isSmall);
            if (c == Category::isMedium) {
                free(ml_.data_);
            } else {
                RefCounted::decrementRefs(ml_.data_);
            }
        }

        typedef uint8_t category_type;

        enum class Category : category_type {
            isSmall  = 0,
            isMedium = IS_LITTLE_ENDIAN ? 0x80 : 2,       //  10000000 , 00000010
            isLarge  = IS_LITTLE_ENDIAN ? 0x40 : 1,       //  01000000 , 00000001
        };

        Category category() const {
            // works for both big-endian and little-endian
            return static_cast<Category>(bytes_[lastChar] & categoryExtractMask);
        }

        struct RefCounted {
            std::atomic<size_t> refCount_;
            Char data_[1];

            constexpr static size_t getDataOffset() {
                return offsetof(RefCounted, data_);
            }

            static RefCounted* fromData(Char* p) {
                return static_cast<RefCounted*>(static_cast<void*>(
                        static_cast<unsigned char*>(static_cast<void*>(p)) -
                        getDataOffset()));
            }

            static size_t refs(Char* p) {
                return fromData(p)->refCount_.load(std::memory_order_acquire);
            }

            static void incrementRefs(Char* p) {
                fromData(p)->refCount_.fetch_add(1, std::memory_order_acq_rel);
            }

            static void decrementRefs(Char* p) {
                auto const dis = fromData(p);
                size_t oldcnt = dis->refCount_.fetch_sub(1, std::memory_order_acq_rel);
                assert(oldcnt > 0);
                if (oldcnt == 1) {
                    free(dis);
                }
            }

            static RefCounted* create(size_t* size) {
                // 先算出总共要分配多少个字符
                size_t capacityBytes = *size + 1;
                // 算出总共要分出多少字节
                capacityBytes = capacityBytes * sizeof(Char) + getDataOffset();

                auto result = static_cast<RefCounted*>(operator new(capacityBytes));
                result->refCount_.store(1, std::memory_order_release);
                *size = (capacityBytes - getDataOffset()) / sizeof(Char) - 1;
                return result;
            }

            static RefCounted* create(const Char* data, size_t* size) {
                const size_t effectiveSize = *size;
                auto result = create(size);

                fbstring_detail::podCopy(data, data + effectiveSize, result->data_);

                return result;
            }

            static RefCounted* reallocate(
                    Char* const data,
                    const size_t currentSize,
                    const size_t currentCapacity,
                    size_t* newCapacity) {
                assert(*newCapacity > 0 && *newCapacity > currentSize);

                size_t capacityBytes = *newCapacity + 1;

                capacityBytes = capacityBytes * sizeof(Char) + getDataOffset();

                auto const dis = fromData(data);

                assert(dis->refCount_.load(std::memory_order_acquire) == 1);

                auto result = static_cast<RefCounted*>(fbstring_detail::smartRealloc(
                        dis,
                        getDataOffset() + (currentSize + 1) * sizeof(Char),
                        getDataOffset() + (currentCapacity + 1) * sizeof(Char),
                        capacityBytes));

                assert(result->refCount_.load(std::memory_order_acquire) == 1);

                *newCapacity = (capacityBytes - getDataOffset()) / sizeof(Char) - 1;

                return result;
            }
        };

        struct MediumLarge {
            Char* data_;
            size_t size_;
            size_t capacity_;

            size_t capacity() const
            {
                return IS_LITTLE_ENDIAN ?
                        capacity_ & capacityExtractMask : capacity_ >> 2;
            }

            void setCapacity(size_t cap, Category cat) {
                capacity_ = IS_LITTLE_ENDIAN
                            ? cap | (static_cast<size_t>(cat) << kCategoryShift)
                            : (cap << 2) | static_cast<size_t>(cat);
            }
        };

        union {
            uint8_t bytes_[sizeof(MediumLarge)]; // For accessing the last byte.
            Char small_[sizeof(MediumLarge) / sizeof(Char)];
            MediumLarge ml_;
        };



        constexpr static size_t  lastChar            = sizeof(MediumLarge) - 1;
        constexpr static size_t  maxSmallSize        = lastChar / sizeof(Char);
        constexpr static size_t  maxMediumSize       = 254 / sizeof(Char);
        constexpr static uint8_t categoryExtractMask = IS_LITTLE_ENDIAN ? 0xC0 : 0x3;
        constexpr static size_t  kCategoryShift      = (sizeof(size_t) - 1) * 8;
        constexpr static size_t  capacityExtractMask = IS_LITTLE_ENDIAN
                                                     ? ~(size_t(categoryExtractMask) << kCategoryShift)
                                                     : 0x0 /* unused */;

        size_t smallSize() const {
            assert(category() == Category::isSmall);
            constexpr auto shift = IS_LITTLE_ENDIAN ? 0 : 2;
            auto smallShifted = static_cast<size_t>(small_[maxSmallSize]) >> shift;
            assert(static_cast<size_t>(maxSmallSize) >= smallShifted);
            return static_cast<size_t>(maxSmallSize) - smallShifted;
        }

        void setSmallSize(size_t s) {
            // Warning: this should work with uninitialized strings too,
            // so don't assume anything about the previous value of
            // small_[maxSmallSize].
            assert(s <= maxSmallSize);
            constexpr auto shift = IS_LITTLE_ENDIAN ? 0 : 2;
            small_[maxSmallSize] = char((maxSmallSize - s) << shift);
            small_[s] = '\0';
            assert(category() == Category::isSmall && size() == s);
        }

        void copySmall(const fbstring_core&);
        void copyMedium(const fbstring_core&);
        void copyLarge(const fbstring_core&);

        void initSmall(const Char* data, size_t size);
        void initMedium(const Char* data, size_t size);
        void initLarge(const Char* data, size_t size);

        void reserveSmall(size_t minCapacity, bool disableSSO);
        void reserveMedium(size_t minCapacity);
        void reserveLarge(size_t minCapacity);

        void shrinkSmall(size_t delta);
        void shrinkMedium(size_t delta);
        void shrinkLarge(size_t delta);

        void unshare(size_t minCapacity = 0);
        Char* mutableDataLarge();

    };

    template <class Char>
    inline void fbstring_core<Char>::copySmall(const fbstring_core& rhs) {
        static_assert(offsetof(MediumLarge, data_) == 0, "fbstring layout failure");
        static_assert(
                offsetof(MediumLarge, size_) == sizeof(ml_.data_),
                "fbstring layout failure");
        static_assert(
                offsetof(MediumLarge, capacity_) == 2 * sizeof(ml_.data_),
                "fbstring layout failure");
        // Just write the whole thing, don't look at details. In
        // particular we need to copy capacity anyway because we want
        // to set the size (don't forget that the last character,
        // which stores a short string's length, is shared with the
        // ml_.capacity field).
        ml_ = rhs.ml_;
        assert(category() == Category::isSmall && this->size() == rhs.size());
    }

    template <class Char>
    void fbstring_core<Char>::copyMedium(const fbstring_core& rhs) {
        // Medium strings are copied eagerly. Don't forget to allocate
        // one extra Char for the null terminator.
        auto const allocSize = (1 + rhs.ml_.size_) * sizeof(Char);
        ml_.data_ = static_cast<Char*>(operator new(allocSize));
        // Also copies terminator.
        fbstring_detail::podCopy(
                rhs.ml_.data_, rhs.ml_.data_ + rhs.ml_.size_ + 1, ml_.data_);
        ml_.size_ = rhs.ml_.size_;
        ml_.setCapacity(allocSize / sizeof(Char) - 1, Category::isMedium);
        assert(category() == Category::isMedium);
    }

    template <class Char>
    void fbstring_core<Char>::copyLarge(const fbstring_core& rhs) {
        // Large strings are just refcounted
        ml_ = rhs.ml_;
        RefCounted::incrementRefs(ml_.data_);
        assert(category() == Category::isLarge && size() == rhs.size());
    }

    // Small strings are bitblitted
    template <class Char>
    inline void fbstring_core<Char>::initSmall(
    const Char* const data, const size_t size) {
        // Layout is: Char* data_, size_t size_, size_t capacity_
        static_assert(
                sizeof(*this) == sizeof(Char*) + 2 * sizeof(size_t),
                "fbstring has unexpected size");
        static_assert(
                sizeof(Char*) == sizeof(size_t), "fbstring size assumption violation");
        // sizeof(size_t) must be a power of 2
        static_assert(
                (sizeof(size_t) & (sizeof(size_t) - 1)) == 0,
                "fbstring size assumption violation");

// If data is aligned, use fast word-wise copying. Otherwise,
// use conservative memcpy.
// The word-wise path reads bytes which are outside the range of
// the string, and makes ASan unhappy, so we disable it when
// compiling with ASan.
        if ((reinterpret_cast<size_t>(data) & (sizeof(size_t) - 1)) == 0) {
            const size_t byteSize = size * sizeof(Char);
            constexpr size_t wordWidth = sizeof(size_t);
            switch ((byteSize + wordWidth - 1) / wordWidth) { // Number of words.
                case 3:
                    ml_.capacity_ = reinterpret_cast<const size_t*>(data)[2];
                case 2:
                    ml_.size_ = reinterpret_cast<const size_t*>(data)[1];
                case 1:
                    ml_.data_ = *reinterpret_cast<Char**>(const_cast<Char*>(data));
                case 0:
                    break;
            }
        } else
        {
            if (size != 0) {
                fbstring_detail::podCopy(data, data + size, small_);
            }
        }
        setSmallSize(size);
    }

    template <class Char>
    void fbstring_core<Char>::initMedium(
    const Char* const data, const size_t size) {
        // Medium strings are allocated normally. Don't forget to
        // allocate one extra Char for the terminating null.
        auto const allocSize = (1 + size) * sizeof(Char);
        ml_.data_ = static_cast<Char*>(::operator new(allocSize));

        ml_.size_ = size;
        ml_.setCapacity(allocSize / sizeof(Char) - 1, Category::isMedium);
        ml_.data_[size] = '\0';
    }

    template <class Char>
    void fbstring_core<Char>::initLarge(
    const Char* const data, const size_t size) {
        // Large strings are allocated differently
        size_t effectiveCapacity = size;
        auto const newRC = RefCounted::create(data, &effectiveCapacity);
        ml_.data_ = newRC->data_;
        ml_.size_ = size;
        ml_.setCapacity(effectiveCapacity, Category::isLarge);
        ml_.data_[size] = '\0';
    }

    template <class Char>
    void fbstring_core<Char>::unshare(size_t minCapacity) {
        assert(category() == Category::isLarge);
        size_t effectiveCapacity = std::max(minCapacity, ml_.capacity());
        auto const newRC = RefCounted::create(&effectiveCapacity);
        // If this fails, someone placed the wrong capacity in an
        // fbstring.
        assert(effectiveCapacity >= ml_.capacity());
        // Also copies terminator.
        fbstring_detail::podCopy(ml_.data_, ml_.data_ + ml_.size_ + 1, newRC->data_);
        RefCounted::decrementRefs(ml_.data_);
        ml_.data_ = newRC->data_;
        ml_.setCapacity(effectiveCapacity, Category::isLarge);
        // size_ remains unchanged.
    }

    template <class Char>
    inline Char* fbstring_core<Char>::mutableDataLarge() {
        assert(category() == Category::isLarge);
        if (RefCounted::refs(ml_.data_) > 1) { // Ensure unique.
            unshare();
        }
        return ml_.data_;
    }

    template <class Char>
    void fbstring_core<Char>::reserveLarge(size_t minCapacity) {
        assert(category() == Category::isLarge);
        if (RefCounted::refs(ml_.data_) > 1) { // Ensure unique
            // We must make it unique regardless; in-place reallocation is
            // useless if the string is shared. In order to not surprise
            // people, reserve the new block at current capacity or
            // more. That way, a string's capacity never shrinks after a
            // call to reserve.
            unshare(minCapacity);
        } else {
            // String is not shared, so let's try to realloc (if needed)
            if (minCapacity > ml_.capacity()) {
                // Asking for more memory
                auto const newRC = RefCounted::reallocate(
                        ml_.data_, ml_.size_, ml_.capacity(), &minCapacity);
                ml_.data_ = newRC->data_;
                ml_.setCapacity(minCapacity, Category::isLarge);
            }
            assert(capacity() >= minCapacity);
        }
    }

    template <class Char>
    void fbstring_core<Char>::reserveMedium(
    const size_t minCapacity) {
        assert(category() == Category::isMedium);
        // String is not shared
        if (minCapacity <= ml_.capacity()) {
            return; // nothing to do, there's enough room
        }
        if (minCapacity <= maxMediumSize) {
            // Keep the string at medium size. Don't forget to allocate
            // one extra Char for the terminating null.
            size_t capacityBytes = (1 + minCapacity) * sizeof(Char);
            // Also copies terminator.
            ml_.data_ = static_cast<Char*>(fbstring_detail::smartRealloc(
                    ml_.data_,
                    (ml_.size_ + 1) * sizeof(Char),
                    (ml_.capacity() + 1) * sizeof(Char),
                    capacityBytes));
            ml_.setCapacity(capacityBytes / sizeof(Char) - 1, Category::isMedium);
        } else {
            // Conversion from medium to large string
            fbstring_core nascent;
            // Will recurse to another branch of this function
            nascent.reserve(minCapacity);
            nascent.ml_.size_ = ml_.size_;
            // Also copies terminator.
            fbstring_detail::podCopy(
                    ml_.data_, ml_.data_ + ml_.size_ + 1, nascent.ml_.data_);
            nascent.swap(*this);
            assert(capacity() >= minCapacity);
        }
    }

    template <class Char>
    void fbstring_core<Char>::reserveSmall(
    size_t minCapacity, const bool disableSSO) {
        assert(category() == Category::isSmall);
        if (!disableSSO && minCapacity <= maxSmallSize) {
            // small
            // Nothing to do, everything stays put
        } else if (minCapacity <= maxMediumSize) {
            // medium
            // Don't forget to allocate one extra Char for the terminating null
            auto const allocSizeBytes =
                    (1 + minCapacity) * sizeof(Char);
            auto const pData = static_cast<Char*>(malloc(allocSizeBytes));
            auto const size = smallSize();
            // Also copies terminator.
            fbstring_detail::podCopy(small_, small_ + size + 1, pData);
            ml_.data_ = pData;
            ml_.size_ = size;
            ml_.setCapacity(allocSizeBytes / sizeof(Char) - 1, Category::isMedium);
        } else {
            // large
            auto const newRC = RefCounted::create(&minCapacity);
            auto const size = smallSize();
            // Also copies terminator.
            fbstring_detail::podCopy(small_, small_ + size + 1, newRC->data_);
            ml_.data_ = newRC->data_;
            ml_.size_ = size;
            ml_.setCapacity(minCapacity, Category::isLarge);
            assert(capacity() >= minCapacity);
        }
    }

    template <class Char>
    inline void fbstring_core<Char>::shrinkSmall(const size_t delta) {
        // Check for underflow
        assert(delta <= smallSize());
        setSmallSize(smallSize() - delta);
    }

    template <class Char>
    inline void fbstring_core<Char>::shrinkMedium(const size_t delta) {
        // Medium strings and unique large strings need no special
        // handling.
        assert(ml_.size_ >= delta);
        ml_.size_ -= delta;
        ml_.data_[ml_.size_] = '\0';
    }

    template <class Char>
    inline void fbstring_core<Char>::shrinkLarge(const size_t delta) {
        assert(ml_.size_ >= delta);
        // Shared large string, must make unique. This is because of the
        // durn terminator must be written, which may trample the shared
        // data.
        if (delta) {
            fbstring_core(ml_.data_, ml_.size_ - delta).swap(*this);
        }
        // No need to write the terminator.
    }

    template <class Char>
    inline Char* fbstring_core<Char>::expandNoinit(
            const size_t delta,
            bool expGrowth, /* = false */
            bool disableSSO /* = FBSTRING_DISABLE_SSO */) {
        // Strategy is simple: make room, then change size
        assert(capacity() >= size());
        size_t sz, newSz;
        if (category() == Category::isSmall) {
            sz = smallSize();
            newSz = sz + delta;
            if (!disableSSO && (newSz <= maxSmallSize)) {
                setSmallSize(newSz);
                return small_ + sz;
            }
            reserveSmall(
                    expGrowth ? std::max(newSz, 2 * maxSmallSize) : newSz, disableSSO);
        } else {
            sz = ml_.size_;
            newSz = sz + delta;
            if (newSz > capacity()) {
                // ensures not shared
                reserve(expGrowth ? std::max(newSz, 1 + capacity() * 3 / 2) : newSz);
            }
        }
        assert(capacity() >= newSz);
        // Category can't be small - we took care of that above
        assert(category() == Category::isMedium || category() == Category::isLarge);
        ml_.size_ = newSz;
        ml_.data_[newSz] = '\0';
        assert(size() == newSz);
        return ml_.data_ + sz;
    }

    template <
            typename E,
            class T = std::char_traits<E>,
            class A = std::allocator<E>,
            class Storage = fbstring_core<E>>
    class basic_fbstring {
//        static_assert(
//                std::is_same<A, std::allocator<E>>::value,
//                "fbstring ignores custom allocators");
//
//        template<typename Ex, typename... Args>
//        FOLLY_ALWAYS_INLINE static void enforce(bool condition, Args &&... args) {
//            if (!condition) {
//                throw_exception<Ex>(static_cast<Args &&>(args)...);
//            }
//        }
//
//        bool isSane() const {
//            return begin() <= end() && empty() == (size() == 0) &&
//                   empty() == (begin() == end()) && size() <= max_size() &&
//                   capacity() <= max_size() && size() <= capacity() &&
//                   begin()[size()] == '\0';
//        }
//
//        struct Invariant {
//            Invariant &operator=(const Invariant &) = delete;
//
//            explicit Invariant(const basic_fbstring &s) noexcept: s_(s) {
//                assert(s_.isSane());
//            }
//
//            ~Invariant() noexcept { assert(s_.isSane()); }
//
//        private:
//            const basic_fbstring &s_;
//        };

    public:
        // types
        typedef T traits_type;
        typedef typename traits_type::char_type value_type;
        typedef A allocator_type;
        typedef typename std::allocator_traits<A>::size_type size_type;
        typedef typename std::allocator_traits<A>::difference_type difference_type;

        typedef typename std::allocator_traits<A>::value_type &reference;
        typedef typename std::allocator_traits<A>::value_type const &const_reference;
        typedef typename std::allocator_traits<A>::pointer pointer;
        typedef typename std::allocator_traits<A>::const_pointer const_pointer;

        typedef E *iterator;
        typedef const E *const_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

        static constexpr size_type npos = size_type(-1);
        typedef std::true_type IsRelocatable;

    private:
//        static void procrustes(size_type &n, size_type nmax) {
//            if (n > nmax) {
//                n = nmax;
//            }
//        }
//
//        static size_type traitsLength(const value_type *s);

    public:
        // C++11 21.4.2 construct/copy/destroy

        // Note: while the following two constructors can be (and previously were)
        // collapsed into one constructor written this way:
        //
        //   explicit basic_fbstring(const A& a = A()) noexcept { }
        //
        // This can cause Clang (at least version 3.7) to fail with the error:
        //   "chosen constructor is explicit in copy-initialization ...
        //   in implicit initialization of field '(x)' with omitted initializer"
        //
        // if used in a struct which is default-initialized.  Hence the split into
        // these two separate constructors.


        basic_fbstring(const basic_fbstring &str) : store_(str.store_) {}

        // Move constructor
        basic_fbstring(basic_fbstring &&goner) noexcept
                : store_(std::move(goner.store_)) {}

        // This is defined for compatibility with std::string
        template<typename A2>
        /* implicit */ basic_fbstring(const std::basic_string<E, T, A2> &str)
                : store_(str.data(), str.size()) {}

        basic_fbstring(
                const basic_fbstring &str,
                size_type pos,
                size_type n = npos,
                const A & /* a */ = A()) {
            assign(str, pos, n);
        }

         basic_fbstring(const value_type *s, const A & /*a*/ = A())
                : store_(s, traitsLength(s)) {}

        basic_fbstring(const value_type *s, size_type n, const A & /*a*/ = A())
                : store_(s, n) {}

        basic_fbstring(size_type n, value_type c, const A & /*a*/ = A()) {
            auto const pData = store_.expandNoinit(n);
            fbstring_detail::podFill(pData, pData + n, c);
        }

        template<class InIt>
         basic_fbstring(
                InIt begin,
                InIt end,
                typename std::enable_if<
                        !std::is_same<InIt, value_type *>::value,
                        const A>::type & /*a*/
                = A()) {
            assign(begin, end);
        }

        // Specialization for const char*, const char*
        basic_fbstring(const value_type *b, const value_type *e, const A & /*a*/ = A())
                : store_(b, size_type(e - b)) {}

        // Nonstandard constructor
        basic_fbstring(
                value_type *s, size_type n, size_type c, AcquireMallocatedString a)
                : store_(s, n, c, a) {}

//        // Construction from initialization list
//        basic_fbstring(std::initializer_list<value_type> il) {
//            assign(il.begin(), il.end());
//        }

        ~basic_fbstring() noexcept {}

        basic_fbstring &operator=(const basic_fbstring &lhs);

        // Move assignment
        basic_fbstring &operator=(basic_fbstring &&goner) noexcept;

//        // Compatibility with std::string
//        template<typename A2>
//        basic_fbstring &operator=(const std::basic_string<E, T, A2> &rhs) {
//            return assign(rhs.data(), rhs.size());
//        }

        // Compatibility with std::string
        std::basic_string<E, T, A> toStdString() const {
            return std::basic_string<E, T, A>(data(), size());
        }

//        basic_fbstring &operator=(const value_type *s) { return assign(s); }
//
//        basic_fbstring &operator=(value_type c);


        // Get
        void swap(basic_fbstring& rhs) { store_.swap(rhs.store_); }

        const value_type* c_str() const { return store_.c_str(); }

        const value_type* data() const { return c_str(); }

        value_type* data() { return store_.data(); }

        // C++11 21.4.4 capacity:
        size_type size() const { return store_.size(); }

        size_type length() const { return size(); }

        size_type max_size() const { return std::numeric_limits<size_type>::max(); }

        void resize(size_type n, value_type c = value_type());

        size_type capacity() const { return store_.capacity(); }


        // C++11 21.4.3 iterators:
        iterator begin() { return store_.mutableData(); }

        const_iterator begin() const { return store_.data(); }

        const_iterator cbegin() const { return begin(); }

        iterator end() { return store_.mutableData() + store_.size(); }


        void push_back(const value_type c) { // primitive
            store_.push_back(c);
        }

    private:
        // Data
        Storage store_;
    };
}


#endif
