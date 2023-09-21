//
// Created by 78172 on 2023/3/10.
//

#include<iostream>
#include<memory>

namespace MySTL
{
    using std::cout;

    template<typename T>
    class NaiveVector
    {
    private:
        size_t _size;
        T* _ptr;
    public:

        NaiveVector() : _size(0), _ptr(nullptr) {}
        explicit NaiveVector(int size) : _size(size), _ptr(new T[size]) {}


        NaiveVector(const NaiveVector& nv)
            : _size(nv._size), _ptr(new T[_size])
        {
            std::copy(nv._ptr, nv._ptr + nv._size, _ptr);
        }

        NaiveVector(NaiveVector&& nv) noexcept
            :_size(nv._size), _ptr(nv._ptr)
        {
            nv._size = 0, nv._ptr = nullptr;
        }


        void swap(NaiveVector &) noexcept;

//        NaiveVector& operator=(const NaiveVector& rhs)
//        {
//            NaiveVector copy = rhs;
//            this->swap(copy);
//            return *this;
//        }

//        by-value assignment operator
        NaiveVector& operator=(NaiveVector copy)
        {
            this->swap(copy);
            return *this;
        }

//        NaiveVector& operator=(NaiveVector&& rhs) noexcept
//        {
//            NaiveVector copy(std::move(rhs));
//            this->swap(copy);
//            return *this;
//        }

        ~NaiveVector()
        {
            if(_ptr)
                delete[] _ptr;
            _ptr = nullptr;
            _size = 0;
        }


        const T& operator[](int idx) const
        {
            if(idx < 0 || idx >= _size) throw std::out_of_range("invalid index");
            return _ptr[idx];
        }

        T& operator[](int idx)
        {
            return const_cast<T&>(
                    const_cast<const NaiveVector&>(*this)[idx]
                    );
        }


        void push_back(const T& data)
        {
            NaiveVector new_vec(_size + 1);
            std::copy(_ptr, _ptr + _size, new_vec._ptr);
            new_vec[_size] = data;
            this->swap(new_vec);
        }


        void print();

        friend void swap(NaiveVector&, NaiveVector&) noexcept;
    };

    template<typename T>
    void NaiveVector<T>::print()
    {
        T* p = _ptr, *end = _ptr + _size;
        while(p != end)
        {
            cout << *p << " ";
            p++;
        }
        cout << std::endl;
    }

    template<typename T>
    void NaiveVector<T>::swap(NaiveVector<T> &rhs) noexcept
    {
        std::swap(_ptr , rhs._ptr);
        std::swap(_size, rhs._size);
    }

    template<typename T>
    void swap(NaiveVector<T> &lhs, NaiveVector<T> &rhs) noexcept
    {
        lhs.swap(rhs);
    }



//    template<typename T>
//    class ZeroNaiveVector
//    {
//    private:
//        size_t _size;
//        std::unique_ptr<T[]> _ptr;
//    public:
//
//        ZeroNaiveVector() = default;
//
//        explicit ZeroNaiveVector(int size) : _size(size)
//        {
//            _ptr = std::make_unique<T[]>(_size);
//        }
//
//        ZeroNaiveVector(const ZeroNaiveVector& znv)
//                : _size(znv._size)
//        {
//            _ptr = std::make_unique<T[]>(_size);
//            std::copy(znv._ptr, znv._ptr + znv._size, _ptr);
//        }
//
//        ZeroNaiveVector(ZeroNaiveVector&& znv) noexcept = default;
//
//
//        void swap(ZeroNaiveVector &) noexcept;
//
//
////        by-value assignment operator
//        ZeroNaiveVector& operator=(ZeroNaiveVector copy)
//        {
//            this->swap(copy);
//            return *this;
//        }
//
//        ~ZeroNaiveVector() = default;
//
//
//        const T& operator[](int idx) const
//        {
//            if(idx < 0 || idx >= _size) throw std::out_of_range("invalid index");
//            return _ptr[idx];
//        }
//
//        T& operator[](int idx)
//        {
//            return const_cast<T&>(
//                    const_cast<const ZeroNaiveVector&>(*this)[idx]
//            );
//        }
//
//
//        void push_back(const T& data)
//        {
//            ZeroNaiveVector new_vec(_size + 1);
//            std::copy(_ptr, _ptr + _size, new_vec._ptr);
//            new_vec[_size] = data;
//            this->swap(new_vec);
//        }
//
//
//        void print();
//
//        friend void swap(ZeroNaiveVector&, ZeroNaiveVector&) noexcept;
//    };
//
//    template<typename T>
//    void ZeroNaiveVector<T>::print()
//    {
//        T* p = _ptr, *end = _ptr + _size;
//        while(p != end)
//        {
//            cout << *p << " ";
//            p++;
//        }
//        cout << std::endl;
//    }
//
//    template<typename T>
//    void ZeroNaiveVector<T>::swap(ZeroNaiveVector<T> &rhs) noexcept
//    {
//        std::swap(_ptr , rhs._ptr);
//        std::swap(_size, rhs._size);
//    }
//
//    template<typename T>
//    void swap(ZeroNaiveVector<T> &lhs, ZeroNaiveVector<T> &rhs) noexcept
//    {
//        lhs.swap(rhs);
//    }
}

void test1()
{
    MySTL::NaiveVector<int> nv;
    nv.push_back(1);
    nv.push_back(2);
    nv.push_back(3);
    nv.print();
}

void test2()
{
    MySTL::NaiveVector<int> nv;
    nv.push_back(1);
    nv.push_back(2);
    nv.push_back(3);

    MySTL::NaiveVector<int> nv2 = nv;
    nv2.print();
}

void test3()
{
    MySTL::NaiveVector<int> nv1;
    nv1.push_back(1);
    nv1.push_back(2);
    nv1.push_back(3);

    MySTL::NaiveVector<int> nv2;
    nv2.push_back(3);
    nv2.push_back(4);
    nv2.push_back(5);

    nv1 = nv2;
    nv1.print();
    nv2.print();
}

void test4()
{
    MySTL::NaiveVector<int> nv;
    nv.push_back(1);
    nv.push_back(2);
    nv.push_back(3);
    nv = nv;
    nv.print();
}

void test5()
{
    MySTL::NaiveVector<int> nv;
    nv.push_back(1);
    nv.push_back(2);
    nv.push_back(3);

    std::cout << nv[3];
}

void test6()
{
    MySTL::NaiveVector<int> nv;
    nv.push_back(1);
    nv.push_back(2);
    nv.push_back(3);

    nv = std::move(nv);
    nv.print();
}

//int main()
//{
//    test1();
//}












