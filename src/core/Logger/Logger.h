#pragma once

#include "core/ILogger.h"

namespace vg::core
{
    class Logger : public ILogger
    {
    public:
        void                    Init        () final override;
        void                    Deinit      () final override;
        void                    Log         (Level _level, const char * _format, ...) final override;
        void                    Clear       () final override;

        void                    Lock        () final override;
        void                    Unlock      () final override;

        const uint              GetLogCount () const final override;
        const LogEntry &        GetLog      (uint _index) const final override;

    protected:
        void                    log         (Level _level, const char * _msg);

    private:
        core::vector<LogEntry>  m_entries;
        core::mutex             m_mutex;
    };    
}