#pragma once
#include "core/IProfiler.h"

namespace vg::core
{
    using mutex = std::mutex;
    using recursive_mutex = std::recursive_mutex;

    #if 0
    template <typename T> using lock_guard = std::lock_guard<T>;
    #else

    template <class T> class lock_guard 
    { 
        public:
            using mutex_type = T;

            explicit lock_guard(T & _Mtx) : m_mutex(_Mtx) 
            { 
                VG_PROFILE_CPU("Mutex");
                m_mutex.lock();
            }

            ~lock_guard() noexcept
            {
                m_mutex.unlock();
            }

            lock_guard(const lock_guard &) = delete;
            lock_guard & operator=(const lock_guard &) = delete;

        private:
            T & m_mutex;
    };

    #endif
}