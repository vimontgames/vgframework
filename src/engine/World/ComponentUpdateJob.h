#pragma once
#include "core/Scheduler/Job.h"
#include "core/GameObject/GatherComponentsContext.h"
#include "core/Container/Span.h"

namespace vg::engine
{
    class ComponentUpdateJob : public core::Job
    {
    public:
        const char * GetClassName() const final { return "ComponentUpdateJob"; }

        ComponentUpdateJob(const core::string & _name);
        void init(core::ComponentGroup _group, core::ComponentPriority _priority, const core::ComponentUpdateContext & _context, core::span<core::GatherComponentsContext::Pair> & _pairs);
        void Run() override;

    private:
        core::ComponentGroup m_group;
        core::ComponentPriority m_priority;
        core::ComponentUpdateContext m_context;
        core::span<core::GatherComponentsContext::Pair> m_pairs;

    //public:
    //    static inline std::atomic<core::uint> s_counter = 0;
    };
}