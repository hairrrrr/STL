//
// Created by 78172 on 2023/3/11.
//

#include<iostream>
#include<mutex>

namespace MySTL
{
    template<typename Mutex>
    class unique_unlock
    {
    public:
        explicit unique_unlock(std::unique_lock<Mutex>& lock)
                : _lock(lock)
        { _lock.unlock(); }

        ~unique_unlock() { _lock.lock(); };

        unique_unlock(const unique_unlock&) = delete;
        unique_unlock(unique_unlock&&) = delete;
        unique_unlock& operator=(const unique_unlock&) = delete;
        unique_unlock& operator=(unique_unlock&&) = delete;
    private:
        std::unique_lock<Mutex>& _lock;
    };
}
