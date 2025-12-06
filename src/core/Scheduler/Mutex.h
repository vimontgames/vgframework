#pragma once
#include "core/IProfiler.h"
#include "core/IScheduler.h"

#ifdef VG_FINAL
#define VG_MUTEX_DEBUG_NAME 0
#define VG_ASSERT_MUTEX_ENABLE 0
#else
#define VG_MUTEX_DEBUG_NAME 1
#define VG_ASSERT_MUTEX_ENABLE 1
#endif

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    // Base class for name mutexes
    //--------------------------------------------------------------------------------------
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

        #if VG_MUTEX_DEBUG_NAME
        void setName(const char *_name)
        {
            m_name = _name;
        }

        const char * getName() const
        {
            return m_name;
        }
        #endif

    protected:
        #if VG_MUTEX_DEBUG_NAME
        const char *        m_name;
        #endif
        T                   m_mutex;
    };

    //--------------------------------------------------------------------------------------
    // A classic mutex that will prevent the same code to run concurrently
    //--------------------------------------------------------------------------------------
    class Mutex : public NamedMutex<std::mutex>
    {
    public:
        Mutex(const char * _name) :
            NamedMutex<std::mutex>()
        {
            #if VG_MUTEX_DEBUG_NAME
            setName(_name);
            #endif
        }
    };

    //--------------------------------------------------------------------------------------
    // A recursive mutex that allows the same thread to lock multiple times
    //--------------------------------------------------------------------------------------
    class RecursiveMutex : public NamedMutex<std::recursive_mutex>
    {
    public:
        RecursiveMutex(const char * _name) :
            NamedMutex<std::recursive_mutex>()
        {
            #if VG_MUTEX_DEBUG_NAME
            setName(_name);
            #endif
        }
    };

    //--------------------------------------------------------------------------------------
    // Non-blocking mutex only used to assert if two different codes run concurrently
    //--------------------------------------------------------------------------------------
    class AssertMutex
    {
    public:
        AssertMutex(const char * _name);

        void lock();
        void unlock();

        #if VG_MUTEX_DEBUG_NAME
        inline void setName(const char * _name)
        {
            m_name = _name;
        }

        inline const char * getName()
        {
            return m_name;
        }
        #endif

    private:
        #if VG_ASSERT_MUTEX_ENABLE
        #if VG_MUTEX_DEBUG_NAME
        const char * m_name;
        #endif
        core::atomic<bool> m_entered;
        ThreadID m_threadID;
        #endif
    };

    //--------------------------------------------------------------------------------------
    // a lock_guard that can be used with any mutex type
    //--------------------------------------------------------------------------------------
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