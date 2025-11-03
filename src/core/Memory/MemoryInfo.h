#pragma once

namespace vg::core
{
    struct CPUMemoryInfo
    {
        core::u64 m_totalPhys     = (core::u64)-1;
        core::u64 m_availPhys     = (core::u64)-1;
        core::u64 m_usedPhys      = (core::u64)-1;

        core::u64 m_totalPageFile = (core::u64)-1;
        core::u64 m_availPageFile = (core::u64)-1;
        core::u64 m_usedPageFile  = (core::u64)-1;
    };

    struct GPUMemoryInfo
    {
        core::u64 m_budgetLocal                    = (core::u64)-1;
        core::u64 m_currentUsageLocal              = (core::u64)-1;
        core::u64 m_availableForReservationLocal   = (core::u64)-1;
        core::u64 m_currentReservationLocal        = (core::u64)-1;

        core::u64 m_budgetShared                   = (core::u64)-1;
        core::u64 m_currentUsageShared             = (core::u64)-1;
        core::u64 m_availableForReservationShared  = (core::u64)-1;
        core::u64 m_currentReservationShared       = (core::u64)-1;
    };
}