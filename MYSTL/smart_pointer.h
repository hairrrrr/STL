//
// Created by 78172 on 2023/2/26.
//

#ifndef MEMORY_MANAGE_SMART_POINTER_H
#define MEMORY_MANAGE_SMART_POINTER_H

#include<iostream>
#include<thread>
#include<mutex>
#include<memory>

using std::cout;
using std::endl;

namespace MySTL
{
    template<class T> class weak_ptr;

    /*
     * shared_ptr 和 unique_ptr 的计数器
     */
    struct Counter
    {
        Counter() : shared_cnt(0), weak_cnt(0) {}

        int shared_cnt;
        int weak_cnt;
        int count() { return shared_cnt + weak_cnt; }
    };

    template<typename T>
    class shared_ptr
    {
    public:
        shared_ptr() = default;

        explicit shared_ptr(T *ptr)
                : _ptr(ptr), _pRefCnt(new Counter), _mutex(new std::mutex())
        { AddRef(); }

        shared_ptr(const shared_ptr& sp)
                : _ptr(sp._ptr), _pRefCnt(sp._pRefCnt), _mutex(sp._mutex)
        { AddRef(); }

        shared_ptr(shared_ptr&& sp) noexcept
                : _ptr(sp._ptr), _pRefCnt(sp._pRefCnt), _mutex(sp._mutex)
        {
            sp._ptr     = nullptr;
            sp._pRefCnt = nullptr;
            sp._mutex   = nullptr;
        }

        shared_ptr(weak_ptr<T>& wp)
        {
            if(wp._pRefCnt->shared_cnt)
            {
                _ptr     = wp._ptr;
                _mutex   = wp._mutex;
                _pRefCnt = wp._pRefCnt;
                AddRef();
            }
            else
            {
                _ptr = nullptr;
                _mutex = nullptr;
            }
        }


        shared_ptr& operator=(const shared_ptr& sp);

        shared_ptr& operator=(shared_ptr&& sp) noexcept;

        void Release();

        virtual ~shared_ptr() noexcept
        { Release(); }

        T& operator*() { return *_ptr; }

        T* operator->() { return _ptr; }

        T* get() { return _ptr; }

        int use_count() { return _pRefCnt->shared_cnt; }

        void print() const;

        void AddRef()
        {
            _mutex->lock();
            _pRefCnt->shared_cnt++;
            cout << "ref = " << _pRefCnt->shared_cnt << endl;
            _mutex->unlock();
        }

        explicit operator bool() const noexcept
        { return _ptr != nullptr; }

        friend class weak_ptr<T>;
    private:
        T* _ptr;
        Counter* _pRefCnt;
        std::mutex* _mutex;
    };

    template<typename T>
    shared_ptr<T>&
    shared_ptr<T>::operator=(const shared_ptr &sp)
    {
        // 直接用指针来判断
        if(_ptr != sp._ptr)
        {
            Release();
            _ptr     = sp._ptr;
            _pRefCnt = sp._pRefCnt;
            _mutex   = sp._mutex;
            AddRef();
        }
        return *this;
    }

    template<typename T>
    shared_ptr<T>&
    shared_ptr<T>::operator=(shared_ptr &&sp) noexcept
    {
        if(_ptr != sp._ptr)
        {
            Release();
            std::swap(_ptr,     sp._ptr);
            std::swap(_pRefCnt, sp._pRefCnt);
            std::swap(_mutex,   sp._mutex);
        }
        return *this;
    }

    template<typename T>
    void shared_ptr<T>::print() const
    {
        cout << "print: " << *_ptr << " " << _pRefCnt->shared_cnt << endl;
    }

    template<typename T>
    void shared_ptr<T>::Release()
    {

        cout <<  "~shared_ptr " << _ptr << endl;
        if(!_ptr) return;
        //std::unique_lock<std::mutex> ulock(*_mutex);
        _mutex->lock();
        if(--(_pRefCnt->shared_cnt) == 0 && _ptr)
        {
            // 为了让 unique_lock 不要再调用 unlock 函数造成空指针错误
            _mutex->unlock();
            cout << "Delete: " << _ptr << endl;
            delete _ptr;
            _ptr = nullptr;

            _mutex->lock();
            if(_pRefCnt->count() == 0)
            {
                _mutex->unlock();
                delete _pRefCnt;
                delete _mutex;
                _mutex   = nullptr;
                _pRefCnt = nullptr;
            }
            else
                _mutex->unlock();
        }
        else
            _mutex->unlock();
    }

    template<class T>
    class weak_ptr
    {
    public:
        weak_ptr() noexcept
                :_ptr(nullptr), _pRefCnt(new Counter), _mutex(new std::mutex)
        {
            _pRefCnt->weak_cnt = 1;
        };

        void AddRef()
        {
            std::unique_lock<std::mutex> lk(*_mutex);
            _pRefCnt->weak_cnt++;
        }

        void Release()
        {
            //std::unique_lock<std::mutex> lk(*_mutex);
            _mutex->lock();
            if( (--_pRefCnt->weak_cnt + _pRefCnt->shared_cnt) == 0)
            {
                _mutex->unlock();
                cout << "release weak" << endl;
                delete _pRefCnt;
                delete _mutex;
                _pRefCnt = nullptr;
                _mutex = nullptr;
            }
            else
                _mutex->unlock();
        }

        explicit weak_ptr(const shared_ptr<T>& sp)
                :_ptr(sp._ptr), _pRefCnt(sp._pRefCnt), _mutex(sp._mutex)
        {
            if(sp._pRefCnt)
                AddRef();
        }

        weak_ptr(const weak_ptr& wp)
                :_ptr(wp._ptr), _pRefCnt(wp._pRefCnt), _mutex(wp._mutex)
        {
            AddRef();
        }

        weak_ptr(weak_ptr&& wp) noexcept
                :_ptr(wp._ptr), _pRefCnt(wp._pRefCnt), _mutex(wp._mutex)
        {
            wp._pRefCnt = wp._ptr = wp._pRefCnt = nullptr;
        }

        weak_ptr& operator=(const weak_ptr& wp)
        {
            _ptr = wp._ptr;
            _pRefCnt = wp._pRefCnt;
            _mutex = wp._mutex;
            AddRef();
        }

        weak_ptr& operator=(const shared_ptr<T>& sp)
        {
            if(_pRefCnt != sp._pRefCnt)
            {
                Release();
                _ptr = sp._ptr;
                _pRefCnt = sp._pRefCnt;
                _mutex = sp._mutex;
                AddRef();
            }
            return *this;
        }

        weak_ptr& operator=(weak_ptr&& wp) noexcept
        {
            if(_pRefCnt != wp._pRefCnt)
            {
                Release();
                _ptr = wp._ptr;
                _pRefCnt = wp._pRefCnt;
                _mutex = wp._mutex;
                wp._ptr = wp._pRefCnt = nullptr;
                wp._mutex = nullptr;
            }
            return *this;
        }

        int use_count() { return _pRefCnt->shared_cnt; }

        bool expired() { return _pRefCnt->count() == 0; }

        shared_ptr<T> lock()
        {
            return shared_ptr<T>(*this);
        }

        virtual ~weak_ptr() noexcept
        {
            cout << "~weak_ptr" << endl;
        }

        friend class shared_ptr<T>;
    private:
        T* _ptr;
        Counter* _pRefCnt;
        std::mutex* _mutex;
    };

    template<class T>
    struct default_delete
    {
        void operator()(T* _ptr) const
        {
            cout << "default_delete" << endl;
            if(_ptr)
                delete _ptr;
        }
    };

    template<class T>
    struct array_delete
    {
        void operator()(T* _ptr) const
        {
            cout << "array_delete" << endl;
            if(_ptr)
                delete[] _ptr;
        }
    };

    struct file_delete
    {
        void operator()(FILE* fp) const
        {
            cout << "file_delete" << endl;
            if(fp)
                fclose(fp);
        }
    };

    template<class T, class Deletor = default_delete<T>>
    class unique_ptr
    {
    public:
        unique_ptr() = default;

        explicit unique_ptr(T* t) : _ptr(t) {}

        unique_ptr(const unique_ptr&) = delete;
        explicit unique_ptr (unique_ptr&& up) noexcept
                :_ptr(up._ptr), _d(up._d)
        { up._ptr = nullptr; }

        unique_ptr& operator=(const unique_ptr& up) = delete;
        unique_ptr& operator=(unique_ptr&& up) noexcept
        {
            if(this != &up)
            {
                Release();
                _ptr = up._ptr;
                _d   = up._d;
                up._ptr = nullptr;
            }
            return *this;
        }

        virtual ~unique_ptr()
        {
            Release();
        }

        void Release()
        {
            if(_ptr)
                _d(_ptr);
        }

    private:
        T* _ptr;
        Deletor _d;
    };
}

/*
 * 测试代码
 */
//void test1()
//{
//    MySTL::shared_ptr<int> sp1(new int(3));
//    MySTL::shared_ptr<int> sp2(sp1);
//    sp2.print();
//    MySTL::shared_ptr<int> sp3(std::move(sp1));
//    sp2.print();
//    sp3 = sp2;
//}
//
//void work(MySTL::shared_ptr<int> sp, int n, std::mutex& m)
//{
//    //cout << sp.get() << endl;
//    for(int i = 0; i < n; ++i)
//    {
//        //cout << std::this_thread::get_id() << " i = " << i << endl;
//        MySTL::shared_ptr<int> p(sp);
//        MySTL::shared_ptr<int> p2(new int);
//        p2 = p;
//
//        {
//            std::unique_lock<std::mutex> lk(m);
//            (*p)++;
//        }
//    }
//}
//
//void test2()
//{
//    MySTL::shared_ptr<int> sp(new int(0));
//    //cout << sp.get() << endl;
//    std::mutex m;
//    std::thread t1(work, sp, 100000, std::ref(m));
//    std::thread t2(work, sp, 100000, std::ref(m));
//    t1.join();
//    t2.join();
//    cout << "ref count: " << sp.use_count() << endl;
//    cout << "val = " << (*sp) << endl;
//}
//
//struct ListNode
//{
//    explicit ListNode(int v = 0)
//            :val(v), next(nullptr), prev(nullptr)
//    {}
//    int val;
//    MySTL::shared_ptr<ListNode> next;
//    MySTL::shared_ptr<ListNode> prev;
//
//    ~ListNode()
//    {
//        cout << "~ListNode()" << endl;
//    }
//};
//
//void test3()
//{
//    MySTL::shared_ptr<ListNode> n1(new ListNode(1));
//    MySTL::shared_ptr<ListNode> n2(new ListNode(2));
//
//    cout << "n1 = " << n1.get() << endl;
//    cout << "n2 = " << n2.get() << endl;
//
//    n1->next = n2;
//    //n2->prev = n1;
//}
//
//struct ListNode2
//{
//    explicit ListNode2(int v = 0)
//            :val(v), next(), prev()
//    {}
//    int val;
//    std::weak_ptr<ListNode2> next;
//    std::weak_ptr<ListNode2> prev;
//
//    ~ListNode2()
//    {
//        cout << "~ListNode()" << endl;
//    }
//};
//
//void test4()
//{
//    std::shared_ptr<ListNode2> n1(new ListNode2(1));
//    std::shared_ptr<ListNode2> n2(new ListNode2(2));
//
//    cout << "n1 = " << n1.get() << endl;
//    cout << "n2 = " << n2.get() << endl;
//
//    n1->next = n2;
//    n2->prev = n1;
//
//}
//
//struct ListNode3
//{
//    explicit ListNode3(int v = 0) :val(v) {}
//
//    int val;
////    MySTL::weak_ptr<ListNode3> next;
////    MySTL::weak_ptr<ListNode3> prev;
//
//    ~ListNode3()
//    {
//        cout << "~ListNode3()" << endl;
//    }
//
//};
//
//void test5()
//{
////    MySTL::shared_ptr<ListNode3> n1(new ListNode3(1));
////    MySTL::shared_ptr<ListNode3> n2(new ListNode3(2));
////
////    cout << "n1 = " << n1.get() << endl;
////    cout << "n2 = " << n2.get() << endl;
////
////    n1->next = n2;
////    n2->prev = n1;
//
//
//    MySTL::weak_ptr<ListNode3> wp1;
//    {
//        MySTL::shared_ptr<ListNode3> sp1(new ListNode3(10));
//        wp1 = sp1;
//
//        MySTL::shared_ptr<ListNode3> sp2 = wp1.lock();
//
//        if(sp2)
//        {
//            cout << sp2->val << endl;
//        }
//
//    }
//
//    MySTL::shared_ptr<ListNode3> sp3 = wp1.lock();
//    if(sp3)
//    {
//        cout << sp3->val << endl;
//    }
//    else
//        cout << "wp1 expired" << endl;
//
//}

#endif //MEMORY_MANAGE_SMART_POINTER_H
