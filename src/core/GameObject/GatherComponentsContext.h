#pragma once

#include "core/IScheduler.h"
#include "core/Component/Component.h"

namespace vg::core
{
    class GameObject;
    class Component;

    struct GatherComponentsContext
    {
        struct Pair
        {
            GameObject * m_gameObject;
            Component * m_component;
            
        };

        struct JobSyncInfo
        {
            core::atomic<bool> started = false;
            core::JobSync id;
        };

        struct ComponentUpdateList
        {
            core::vector<Pair> m_pairs;
            JobSyncInfo m_jobSync;
        };

        void reset()
        {
            for (uint g = 0; g < enumCount<ComponentGroup>(); ++g)
            {
                for (uint p = 0; p < enumCount<ComponentPriority>(); ++p)
                {
                    for (uint m = 0; m < enumCount<ComponentMultithreadType>(); ++m)
                    {
                        m_componentsToUpdate[g][p][m].m_pairs.clear();
                    }
                }
            }
        }

        UpdateFlags m_flags;
        ComponentUpdateList m_componentsToUpdate[enumCount<ComponentGroup>()][enumCount<ComponentPriority>()][enumCount<ComponentMultithreadType>()];
    };
}