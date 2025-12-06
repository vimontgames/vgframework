#include "core/Precomp.h"
#include "Mutex.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    AssertMutex::AssertMutex(const char * _name)       
    {
        #if VG_MUTEX_DEBUG_NAME
        setName(_name);
        #endif

        #if VG_ASSERT_MUTEX_ENABLE 
        m_entered.store(false);
        #endif        
    }

    //--------------------------------------------------------------------------------------
    void AssertMutex::lock()
    {
        #if VG_ASSERT_MUTEX_ENABLE
        bool expected = false;
        if (!m_entered.compare_exchange_strong(expected, true))
        {
            auto * scheduler = Kernel::getScheduler();
            ThreadID currentThread = scheduler->GetCurrentThreadID();
            VG_ASSERT(false, "AssertMutex \"%s\" is entered by thread \"%s\" (0x%08X) but it was already entered by thread \"%s\" (0x%08X)", m_name, scheduler->GetThreadName(currentThread).c_str(), currentThread, scheduler->GetThreadName(m_threadID).c_str(), m_threadID);
        }
        else
        {
            auto * scheduler = Kernel::getScheduler();
            m_threadID = scheduler->GetCurrentThreadID();
        }
        #endif
    }

    //--------------------------------------------------------------------------------------
    void AssertMutex::unlock()
    {
        #if VG_ASSERT_MUTEX_ENABLE
        m_entered.store(false);
        #endif
    }
}