#pragma once
#include "core/IProfiler.h"

#ifdef VG_FINAL
#define VG_MUTEX_DEBUG_NAME 0
#else
#define VG_MUTEX_DEBUG_NAME 1
#endif

namespace vg::core
{
    template <typename T> class NamedMutex
    {
    public:
        void lock()
        {
            #if VG_MUTEX_DEBUG_NAME
            VG_ASSERT(nullptr != m_name);
            VG_PROFILE_CPU(m_name);
            #endif

            m_mutex.lock();
        }

        void unlock()
        {
            m_mutex.unlock();
        }

        void setName(const char *_name)
        {
            m_name = _name;
        }

        const char * getName() const
        {
            return m_name;
        }

    protected:
        const char *        m_name;
        T                   m_mutex;
    };

    class Mutex : public NamedMutex<std::mutex>
    {
    public:
        Mutex(const char * _name) :
            NamedMutex<std::mutex>()
        {
            m_name = _name;
        }
    };

    class RecursiveMutex : public NamedMutex<std::recursive_mutex>
    {
    public:
        RecursiveMutex(const char * _name) :
            NamedMutex<std::recursive_mutex>()
        {
            m_name = _name;
        }
    };

    template <class T> class lock_guard 
    { 
        public:
            explicit lock_guard(T & _Mtx) : 
                m_mutex(_Mtx) 
            { 
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
}