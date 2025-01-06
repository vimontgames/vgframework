#pragma once

namespace vg::core
{
    class AssertMutex 
    {
        public:
            inline AssertMutex(const char * _name) :
                m_name(_name),
                m_isEntered(false)
            {
            }

            inline void lock()
            {
                bool expected = false;
                if (!m_isEntered.compare_exchange_strong(expected, true))
                    VG_ASSERT(false, "AssertMutex \"%s\" already entered", m_name);
            }

            inline void unlock() 
            {
                m_isEntered.store(false);
            }

        private:
            const char * m_name;
            core::atomic<bool> m_isEntered;
    };
}