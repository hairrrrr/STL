//
// Created by 78172 on 2023/3/31.
//

#ifndef MEMORY_MANAGE_STRING_H
#define MEMORY_MANAGE_STRING_H

#include"iterator_base.h"
#include"allocator.h"
#include"hash_func.h"
#include<iostream>


/************************
 *  char traits
 */
namespace MYSTL
{

    template<typename _CharT>
    struct char_traits;

    template<>
    struct char_traits<char>
    {
        typedef char                   char_type;
        typedef int                    int_type;
        typedef std::streampos         pos_type;
        typedef std::streamoff         off_type;
        typedef std::mbstate_t         state_type;


        static void
        assign(char_type& __c1, const char_type& __c2)
        { __c1 = __c2; }

        static bool
        eq(const char_type& __c1, const char_type& __c2)
        { return __c1 == __c2; }

        static  bool
        lt(const char_type& __c1, const char_type& __c2)
        {
            // LWG 467.
            return (static_cast<unsigned char>(__c1)
                    < static_cast<unsigned char>(__c2));
        }

        static  int
        compare(const char_type* __s1, const char_type* __s2, size_t __n)
        {
            if (__n == 0)
                return 0;
            return __builtin_memcmp(__s1, __s2, __n);
        }

        static  size_t
        length(const char_type* __s)
        {
            return __builtin_strlen(__s);
        }

        static  const char_type*
        find(const char_type* __s, size_t __n, const char_type& __a)
        {
            if (__n == 0)
                return 0;

            return static_cast<const char_type*>(__builtin_memchr(__s, __a, __n));
        }

        static  char_type*
        move(char_type* __s1, const char_type* __s2, size_t __n)
        {
            if (__n == 0)
                return __s1;

            return static_cast<char_type*>(__builtin_memmove(__s1, __s2, __n));
        }

        static  char_type*
        copy(char_type* __s1, const char_type* __s2, size_t __n)
        {
            if (__n == 0)
                return __s1;

            return static_cast<char_type*>(__builtin_memcpy(__s1, __s2, __n));
        }

        static  char_type*
        assign(char_type* __s, size_t __n, char_type __a)
        {
            if (__n == 0)
                return __s;

            return static_cast<char_type*>(__builtin_memset(__s, __a, __n));
        }

        static  char_type
        to_char_type(const int_type& __c)
        { return static_cast<char_type>(__c); }

        // To keep both the byte 0xff and the eof symbol 0xffffffff
        // from ending up as 0xffffffff.
        static  int_type
        to_int_type(const char_type& __c)
        { return static_cast<int_type>(static_cast<unsigned char>(__c)); }

        static  bool
        eq_int_type(const int_type& __c1, const int_type& __c2)
        { return __c1 == __c2; }

        static  int_type
        eof()
        { return static_cast<int_type>(_GLIBCXX_STDIO_EOF); }

        static  int_type
        not_eof(const int_type& __c)
        { return (__c == eof()) ? 0 : __c; }
    };
}

namespace MYSTL
{
    template<typename CharT, typename Traits, typename Alloc>
    class basic_string
    {
    public:
        typedef Traits					        traits_type;
        typedef typename Traits::char_type		value_type;
        typedef          Alloc				    allocator_type;
        typedef typename Alloc::size_type		size_type;
        typedef typename Alloc::difference_type	difference_type;
        typedef typename Alloc::reference		reference;
        typedef typename Alloc::const_reference	const_reference;
        typedef typename Alloc::pointer		    pointer;
        typedef typename Alloc::const_pointer	const_pointer;
        typedef pointer       iterator;
        typedef const_pointer const_iterator;

        static const size_type npos = static_cast<size_type>(-1);

    private:

        pointer   _data;
        size_type _size;

        enum { _E_local_capacity = 15 / sizeof(CharT)  };

        union {
            CharT _local_buf[_E_local_capacity + 1];
            size_type _capacity;
        };

    public:

        basic_string() : _data(_local_buf), _size(0) {}

        basic_string(const char* str)
            : _data(_local_buf), _size(0)
        {
            const CharT* end = str ? traits_type::length(str) + str
                    // 获取一个非 0 的地址
                                   : reinterpret_cast<const CharT*>(__alignof__(CharT));
            _M_construct(str, end);
        }

        basic_string(const char* s, size_type pos, size_type n = npos)
                : _data(_local_buf), _size(0)
        {
            assert(pos >= 0 && n >= 0);

            size_type length = traits_type::length(s);

            if( pos >= length )
                std::__throw_out_of_range_fmt(__N("basic_string::find: pos "
                                                  "(which is %zu) >= this->size() "
                                                  "(which is %zu)"), pos, length);

            if( (n == npos) || (pos + n > length) ) n = length - pos;

            const CharT* end = s ?  s + pos + n
                               // 获取一个非 0 的地址
                                   : reinterpret_cast<const CharT*>(__alignof__(CharT));
            _M_construct(s + pos, end);
        }


        basic_string(size_type n, CharT ch)
                : _data(_local_buf), _size(0)
        {
            _M_construct(n, ch);
        }


        basic_string(const basic_string& rhs)
                : _data(_local_buf), _size(0)
        {
            basic_string tmp(rhs._data);
            this->swap(tmp);
        }


        basic_string& operator=(basic_string rhs)
        {
            this->swap( rhs);
            return *this;
        }

        ~basic_string()
        { _M_dispose(); }

        // iterator
        iterator begin() { return iterator(_data); }
        const_iterator begin() const { return iterator(_data); }
        iterator end() { return iterator(_data + _size); }
        const_iterator end() const { return iterator(_data + _size); }

        // size capacity 更改操作
        void clear()
        { _size = 0; }

        void reserve(size_type size);

        void reserve();

        void resize(size_type n, CharT val = CharT());

        // 插入
        basic_string& insert(size_type pos, size_type count, CharT ch);
        basic_string& insert(size_type pos, const char* s);
        basic_string& insert(size_type pos, const basic_string& str);
        basic_string& insert(size_type pos, const basic_string& str,
                             size_type index, size_type count = npos);

        void push_back(CharT ch)
        { insert(_size, 1, ch); }

        void append(size_type n, CharT ch) { insert(_size, n, ch); }

        void append(const char* s)
        { insert(_size, s); }

        void append(const basic_string& str)
        { insert(_size, str); }

        basic_string& operator+=(CharT ch)
        {
            append(ch);
            return *this;
        }

        basic_string& operator+=(const char* s)
        {
            append(s);
            return *this;
        }

        basic_string& operator+=(const basic_string& str)
        {
            append(str);
            return *this;
        }


        // 删除
        basic_string& erase(size_type pos = 0, size_type len = npos)
        {
            assert(pos >= 0);
            if( pos >= _size )
                std::__throw_out_of_range("basic_string::erase");

            if(len == npos)
                _M_set_size(pos);
            else if(len != 0)
                _M_erase(pos, std::min(len, _size - pos));

            return *this;
        }

        // 查找
        size_type find( const basic_string& str, size_type pos = 0 ) const
        {
            assert(pos >= 0);
            if( pos >= _size )
                std::__throw_out_of_range_fmt(__N("basic_string::find: pos "
                                             "(which is %zu) >= this->size() "
                                             "(which is %zu)"), pos, _size);

            CharT* res = std::strstr(_data + pos, str._data);
            if(res)
                return res - _data;
            return npos;
        }

        size_type find( const CharT* s, size_type pos = 0 ) const
        {
            assert(pos >= 0);
            if( pos >= _size )
                std::__throw_out_of_range_fmt(__N("basic_string::find: pos "
                                             "(which is %zu) >= this->size() "
                                             "(which is %zu)"), pos, _size);
            CharT* res = std::strstr(_data + pos, s);
            if(res)
                return res - _data;
            return npos;
        }
        size_type find( CharT ch, size_type pos = 0 ) const
        {
            assert(pos >= 0);
            if( pos >= _size )
                std::__throw_out_of_range_fmt(__N("basic_string::find: pos "
                                             "(which is %zu) >= this->size() "
                                             "(which is %zu)"), pos, _size);

            const char* data = _data + pos;
            const char* res = traits_type::find(data, _size - pos, ch);

            if(res)
                return res - _data;

            return npos;
        }


        basic_string substr(size_type pos, size_type n = npos) const
        {
            assert(pos >= 0);
            if( pos >= _size )
                std::__throw_out_of_range_fmt(__N("basic_string::substr: pos "
                                                  "(which is %zu) >= this->size() "
                                                  "(which is %zu)"), pos, _size);

            return basic_string(_data, pos, n);
        }


        // 元素访问
        reference operator[](size_type pos)
        {
            assert(pos >= 0);

            return _data[pos];
        }

        const_reference operator[](size_type pos) const
        {
            assert(pos >= 0);

            return _data[pos];
        }

        reference at(size_type pos)
        {
            if( pos < 0 || pos >= _size )
                __throw_out_of_range_fmt(__N("basic_string::at: __n "
                                             "(which is %zu) >= this->size() "
                                             "(which is %zu)"),
                                         pos, _size);
            return _data[pos];
        }

        const_reference at(size_type pos) const
        {
            if( pos < 0 || pos >= _size )
                __throw_out_of_range_fmt(__N("basic_string::at: __n "
                                             "(which is %zu) >= this->size() "
                                             "(which is %zu)"),
                                         pos, _size);
            return _data[pos];
        }

        reference front()
        {
            assert(!empty());
            return operator[](0);
        }

        const_reference front() const
        {
            assert(!empty());
            return operator[](0);
        }

        reference back()
        {
            assert(!empty());
            return operator[](_size - 1);
        }

        const_reference back() const
        {
            assert(!empty());
            return operator[](_size - 1);
        }


        // 获取内部元素
        const CharT* c_str() const
        { return _data; }

        const CharT* data() const
        { return _data; }

        size_type capacity() const { return _capacity; }
        size_type size() const { return _size; }

        bool empty() const
        { return _size == 0; }

        template<class C, class T, class A>
        friend std::ostream& operator<<(std::ostream& os, const basic_string<C, T, A>& str);

        template<class C, class T, class A>
        friend std::istream& operator>>(std::istream& is, const basic_string<C, T, A>& str);

    private:

        size_type max_size() const
        {
            return ( allocator_type().max_size() - 1 ) / 2;
        }

        void _M_set_size(size_type size)
        {
            _size = size;
            _data[size] = CharT();
        }

        bool _M_is_local()
        { return _data == _local_buf; }

        void _M_dispose()
        {
            if(!_M_is_local())
                _M_destroy();
        }

        void _M_destroy()
        {
            // 加上 null terminator 字符
            allocator_type().deallocate(_data, _capacity + 1);
        }

        template<typename InputIteraotr>
        void _M_construct(InputIteraotr begin, InputIteraotr end);

        void _M_construct(size_type n, CharT ch);

        pointer _M_create(size_type& capacity, size_type old_capacity);

        void _M_insert(CharT* place, size_type pos, const CharT* s, size_type index, size_type count);

        void _M_insert_transfer( size_type pos, const CharT* s, size_type index, size_type count );

        void _M_erase(size_type pos, size_type len);

        void swap(basic_string& rhs);
    };

    typedef basic_string<char, char_traits<char>, MYSTL::allocator<char>> string;

    /*
     * 构造函数内调用，初始化 small string 和 large string
     */
    template<typename CharT, typename Traits, typename Alloc>
    template<typename InputIteraotr>
    void basic_string<CharT, Traits, Alloc>::
    _M_construct(InputIteraotr begin, InputIteraotr end)
    {
        if ( begin == nullptr && begin != end )
            std::__throw_logic_error(__N("basic_string::"
                                         "_M_construct null not valid"));

        size_type length = static_cast<size_type>( MYSTL::distance(begin, end));
        size_type capacity = length;

        if(length > _E_local_capacity)
        {
            _data     = _M_create(capacity, 0);
            _capacity = capacity;
        }

        try {
            traits_type::copy(_data, begin, length);
        }
        catch(...) {
            _M_dispose();
            __throw_exception_again;
        }

        // small string 不需要这是 capacity
        _M_set_size(length);
    }


    template<typename CharT, typename Traits, typename Alloc>
    void basic_string<CharT, Traits, Alloc>::
    _M_construct(size_type n, CharT ch)
    {
        assert(n >= 0);

        if(n <= _E_local_capacity)
            traits_type::assign(_data, n, ch);
        else
        {
            size_type capacity = n;
            _data = _M_create(capacity, 0);
            traits_type::assign(_data, n, ch);
            _capacity = capacity;
        }

        _M_set_size(n);
    }

    /*
     * 扩充 capacity
     */
    template<typename CharT, typename Traits, typename Alloc>
    typename basic_string<CharT, Traits, Alloc>::pointer
    basic_string<CharT, Traits, Alloc>::
    _M_create(size_type& capacity, size_type old_capacity)
    {
        if( capacity > max_size() )
            std::__throw_length_error(__N("basic_string::_M_create"));

        if( capacity > old_capacity && capacity < 2 * old_capacity )
        {
            capacity = 2 * old_capacity;
            if(capacity > max_size())
                capacity = max_size();
        }

        // 加上 null terminator
        return allocator_type().allocate(capacity + 1);
    }

    /*
     * 交换两个 basic_string
     */
    template<typename CharT, typename Traits, typename Alloc>
    void basic_string<CharT, Traits, Alloc>::
    swap(basic_string& rhs)
    {
        // this 为 small string
        if( _M_is_local() )
        {
            // rhs 也是 small string
            if(rhs._M_is_local())
            {
                if( _size && rhs._size )
                {
                    CharT tmp[_E_local_capacity];
                    traits_type::copy(tmp, _data, _size);
                    traits_type::copy(_data, rhs._data, rhs._size);
                    traits_type::copy(rhs._data, tmp, _size);
                }
                else if( _size )
                {
                    traits_type::copy(rhs._data, _data, _size);
                    rhs._M_set_size(_size);
                    _M_set_size(0);
                    return;
                }
                else
                {
                    traits_type::copy(_data, rhs._data, rhs._size);
                    _M_set_size(rhs._size);
                    rhs._M_set_size(0);
                    return;
                }
            }
            // rhs 不是 small string
            else
            {
                size_type rhs_cap = rhs._capacity;
                traits_type::copy(rhs._local_buf, _data, _size);
                _data = rhs._data;
                rhs._data = rhs._local_buf;
                _capacity = rhs_cap;
            }
        }
        // this 不是 small string
        else
        {
            // rhs 是 small string
            if( rhs._M_is_local() )
            {
                size_type this_cap = _capacity;
                traits_type::copy(_local_buf, rhs._data, rhs._size);
                rhs._data = _data;
                _data = _local_buf;
                rhs._capacity = this_cap;
            }
            // rhs 不是 samll string
            else
            {
                std::swap(_data, rhs._data);
                std::swap(_capacity, rhs._capacity);
            }
        }
        // 最后交换 size
        std::swap(_size, rhs._size);
        _M_set_size(_size);
        rhs._M_set_size(rhs._size);
    }


    /*
     * 将 capacity 扩容到 size
     * 如果 size 小于等于 capacity，什么也不做
     * 如果 size 大于 capacity，扩容
     * 只要 string 被扩容，一定不会是 small string
     */
    template<typename CharT, typename Traits, typename Alloc>
    void basic_string<CharT, Traits, Alloc>::
    reserve(size_type size)
    {
        if(size <= _size) return;

        size_type capacity = _capacity;

        CharT* tmp = _M_create(size, capacity);
        // 下面不会更改 size，所以这里需要多拷贝一个 null terminator
        traits_type::copy(tmp, _data, _size + 1);
        _M_dispose();
        _data = tmp;
        _capacity = size;
    }

    /*
     * 类似 shrink-to-fit request
     * 调用后，capacity 拥有一个 大于或者等于 size 的不定值
     * 如果 string 不是 small string，capacity 会调整为 size
     */
    template<typename CharT, typename Traits, typename Alloc>
    void basic_string<CharT, Traits, Alloc>::
    reserve()
    {
        // small string 不需要 capacity
        if( _M_is_local() ) return;

        size_type size = _size, capacity = _capacity;
        if(_size <= _E_local_capacity)
        {

            traits_type::copy(_local_buf, _data, size + 1);
            _M_destroy();
            _data = _local_buf;
        }
        else if(_size < _capacity)
        {
            CharT* tmp = allocator_type().allocate(size + 1);
            traits_type::copy(tmp, _data, size + 1);
            _M_destroy();
            _data     = tmp;
            _capacity = size;
        }
    }

    template<typename CharT, typename Traits, typename Alloc>
    void basic_string<CharT, Traits, Alloc>::
    resize(size_type n, CharT ch)
    {
        if(n > max_size())
            std::__throw_length_error("basic_string::resize");
        if( n > _size )
            append(n, ch);
        else if(n < _size)
            _M_set_size(n);
    }


    template<typename CharT, typename Traits, typename Alloc>
    void basic_string<CharT, Traits, Alloc>::
    _M_insert_transfer( size_type pos, const CharT* s,
                        size_type index, size_type count )
    {
        size_type new_size = count + _size;
        CharT* new_place = _data;
        /*
        * 扩容的情况：
        * 如果是小字符串并且 new size 大于 _E_local_capacity
        * 或者是大字符串并且 new size 大于 _capacity
        *
        * 不扩容的情况
        * 如果是小字符串并且 new size 小于等于 _E_local_capacity
        * 或者是大字符串并且 new size 小于等于 _capacity
        */
        if( _M_is_local() && new_size > _E_local_capacity ||
            !_M_is_local() &&  _capacity < new_size  )
        {
            size_type capacity = new_size;
            new_place = _M_create(capacity, _capacity);

            _M_insert(new_place, pos, s, 0, count);
            _M_set_size(new_size);
            _data = new_place;
            _capacity = capacity;
        }
        else
        {
            _M_insert(new_place, pos, s, 0, count);
            _M_set_size(new_size);
        }
    }

    /*****************************
     * 帮助 insert 函数完成插入动作
     * insert 函数会在 _data[pos] 前插入
     * place 为 dest
     * pos 为 s 隔开 _data 的位置
     * s 为要插入的字符串
     * index 为 从 s[index] 开始插
     * count 为插入 s 的字符数
     *
     * 标准 STL insert 是不支持尾插的，因为 pos 必须 < size
     * 但是尾插的逻辑和 insert 类似，为了避免重复代码，这里我们认为
     * 当 pos 为 size 时，尾插
     */
    template<typename CharT, typename Traits, typename Alloc>
    void basic_string<CharT, Traits, Alloc>::
    _M_insert(CharT* place, size_type pos, const CharT* s,
              size_type index, size_type count)
    {
        assert(count >= 0);
        assert(pos >= 0);
        assert(index >= 0);
        assert(s != nullptr && place != nullptr);
        if( pos > _size )
            std::__throw_out_of_range(__N("basic_string::_M_insert"));
        if( index > traits_type::length(s) )
            std::__throw_out_of_range(__N("basic_string::_M_insert"));

        if(count == 0) return;
        // 由于 place 指向 _data，我们插入的顺序为：
        // [_data[pos, _size), s[index, index + count), _data[0, pos)
        size_type locate = pos + count;

        traits_type::move(place + locate, _data + pos, _size - pos);
        locate -= count;
        traits_type::move(place + locate, s + index, count);
        locate -= pos;
        traits_type::move(place + locate, _data, pos);
    }

    template<typename CharT, typename Traits, typename Alloc>
    basic_string<CharT, Traits, Alloc>&
    basic_string<CharT, Traits, Alloc>::
    insert(size_type pos, size_type count, CharT ch)
    {
        assert(count >= 0);
        assert(pos >= 0);
        if( pos > _size )
            std::__throw_out_of_range(__N("basic_string::insert(pos, count, ch)"));

        basic_string tmp(count, ch);

        _M_insert_transfer(pos, tmp._data, 0, count);
    }

    template<typename CharT, typename Traits, typename Alloc>
    basic_string<CharT, Traits, Alloc>&
    basic_string<CharT, Traits, Alloc>::
    insert(size_type pos, const char* s)
    {
        assert(s);
        assert(pos >= 0);
        if( pos > _size )
            std::__throw_out_of_range(__N("basic_string::insert(pos, s)"));

        size_type count = traits_type::length(s);

        _M_insert_transfer(pos, s, 0, count);
    }

    template<typename CharT, typename Traits, typename Alloc>
    basic_string<CharT, Traits, Alloc>&
    basic_string<CharT, Traits, Alloc>::
    insert(size_type pos, const basic_string& str)
    {
        assert(str._data);
        assert(pos >= 0);
        if( pos > _size )
            std::__throw_out_of_range(__N("basic_string::insert(pos, str)"));


        _M_insert_transfer(pos, str._data, 0, str._size);
    }

    template<typename CharT, typename Traits, typename Alloc>
    basic_string<CharT, Traits, Alloc>&
    basic_string<CharT, Traits, Alloc>::
    insert(size_type pos, const basic_string& str,
           size_type index, size_type count)
    {
        assert(str._data);
        assert(pos >= 0);
        if( pos > _size )
            std::__throw_out_of_range(__N("basic_string::insert(pos, str, index, count)"));

        _M_insert_transfer(pos, str._size, index, count);
    }


    template<typename CharT, typename Traits, typename Alloc>
    void basic_string<CharT, Traits, Alloc>::
    _M_erase(size_type pos, size_type len)
    {
        size_type how_much = _size - pos - len;
        traits_type::copy(_data + pos, _data + pos + len, how_much);
        _M_set_size(_size - len);
    }


    template<typename CharT, typename Traits, typename Alloc>
    std::ostream& operator<<(std::ostream& os,
                            const basic_string<CharT, Traits, Alloc>& str)
    {
         os << str._data;
         return os;
    }

    template<class C, class T, class A>
    std::istream& operator>>(std::istream& is, const basic_string<C, T, A>& str)
    {
        str.clear();

        const int N = 32;
        char buff[N];
        int i = 0;
        char ch = is.get();

        while(ch != ' ' && ch != '\n')
        {
            buff[i++] = ch;
            if(i == N - 1)
            {
                buff[i] = '\0';
                str += buff;
                i = 0;
            }
            ch = is.get();
        }

        buff[i] = '\0';
        str += buff;

        return is;
    }


    template<>
    struct hash<string>
    {
        size_t operator()(const string& __s) const noexcept
        { return MYSTL::hash<const char*>()(__s.c_str()); }
    };

} // namespace MYSTL

#endif //MEMORY_MANAGE_STRING_H
