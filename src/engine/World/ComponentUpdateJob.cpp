#include "engine/Precomp.h"

#include "ComponentUpdateJob.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    ComponentUpdateJob::ComponentUpdateJob(const core::string & _name) :
        Job(_name, nullptr)
    {

    }

    //--------------------------------------------------------------------------------------
    void ComponentUpdateJob::init(ComponentGroup _group, ComponentPriority _priority, const ComponentUpdateContext & _context, span<GatherComponentsContext::Pair> & _pairs)
    {
        m_group = _group;
        m_priority =_priority;
        m_context =_context;
        m_pairs = _pairs;
    }

    //--------------------------------------------------------------------------------------
    void ComponentUpdateJob::Run()
    {
        //VG_PROFILE_CPU(GetStaticName());
        
        ComponentUpdateContext componentUpdateContext = m_context;
        for (uint i = 0; i < m_pairs.size(); ++i)
        {
            auto & entry = m_pairs[i];
            VG_PROFILE_CPU(entry.m_component->GetClassName());
            componentUpdateContext.m_gameObject = (IGameObject*)entry.m_gameObject;
            entry.m_component->Update(componentUpdateContext);
        }
    }
}