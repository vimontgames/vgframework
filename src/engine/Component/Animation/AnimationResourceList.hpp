#include "AnimationResourceList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(AnimationResourceList);

    //--------------------------------------------------------------------------------------
    AnimationResourceList::AnimationResourceList(const core::string & _name, core::IObject * _parent) :
        IResourceList(_name, _parent)
    {
        // resize of vector not supported because owner changes its address
        m_animationResources.reserve(256);
    }

    //--------------------------------------------------------------------------------------
    AnimationResourceList::~AnimationResourceList()
    {

    }

    //--------------------------------------------------------------------------------------
    bool AnimationResourceList::registerClass(vg::core::IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(AnimationResourceList, "Animation Resource List", IClassDesc::Flags::None))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResourceList::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObjectVectorEx(AnimationResourceList, m_animationResources, AnimationResource, "Animation", IProperty::Flags::Resource);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResourceList::Add()
    {
        AnimationResource & animRes = m_animationResources.push_empty();
        
        // Update resource owners
        for (auto & animRes : m_animationResources)
            animRes.setParent(this);
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResourceList::Remove()
    {
        if (m_animationResources.size() > 0)
        {
            m_animationResources.pop_back();
        
            // Update resource owners
            for (auto & animRes : m_animationResources)
                animRes.setParent(this);
        
            return true;
        }

        return false;
    }
}