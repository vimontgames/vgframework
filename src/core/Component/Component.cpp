#include "core/Precomp.h"
#include "Component.h"
#include "core/IProperty.h"
#include "core/IGameObject.h"
#include "core/IFactory.h"
#include "core/IClassDesc.h"
#include "core/Object/EnumHelper.h"

#if !VG_ENABLE_INLINE
#include "Component.inl"
#endif

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Component::Component(const core::string & _name, IObject * _parent) :
        IComponent(_name, _parent),
        m_flags(Flags::Enabled)
    {
        VG_ASSERT(nullptr == _parent || dynamic_cast<IGameObject*>(_parent));
    }

    //--------------------------------------------------------------------------------------
    Component::~Component()
    {
        
    }

    //--------------------------------------------------------------------------------------
    const IGameObject * Component::GetGameObject() const 
    { 
        return (const IGameObject*)getGameObject();
    }

    //--------------------------------------------------------------------------------------
    bool Component::registerProperties(IClassDesc & _desc)
    {
        _desc.registerPropertyEnumBitfield(Component, Flags, m_flags, "Flags");

        return true;
    }

    //--------------------------------------------------------------------------------------
    IComponent::Flags Component::GetFlags() const
    {
        return getFlags();
    }

    //--------------------------------------------------------------------------------------
    void Component::SetFlags(Flags _flags, bool _enabled)
    {
        setFlags(_flags, _enabled);
    }
}