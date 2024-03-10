#pragma once

#include "core/Object/Object.h"
#include "core/Misc/AABB/AABB.h"

namespace vg::renderer
{
    class Geometry : public core::Object
    {
    public:
        VG_CLASS_DECL(Geometry, core::Object);

        Geometry(const core::string & _name, core::IObject * _parent = nullptr) :
            core::Object(_name, _parent)
        {
    
        }

        VG_INLINE void setAABB(const core::AABB & _aabb);
        VG_INLINE const core::AABB & getAABB() const;

    private:
        core::AABB m_aabb;
    };
}

#if VG_ENABLE_INLINE
#include "Geometry.inl"
#endif