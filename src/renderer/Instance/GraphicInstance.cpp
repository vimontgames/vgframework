#include "renderer/Precomp.h"
#include "GraphicInstance.h"

#if !VG_ENABLE_INLINE
#include "GraphicInstance.inl"
#endif

#include "core/Object/AutoRegisterClass.h"
#include "core/Math/Math.h"
#include "gfx/FrameGraph/RenderPassContext.h"

#include "Mesh/MeshInstance.hpp"
#include "ParticleSystem/ParticleSystemInstance.hpp"
#include "Light/LightInstance.hpp"
#include "Camera/CameraInstance.hpp"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    bool GraphicInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(GraphicInstance, m_flags, PropertyFlags::ReadOnly | PropertyFlags::Hidden, true);
        setPropertyFlag(GraphicInstance, m_color, PropertyFlags::ReadOnly | PropertyFlags::Hidden, true);
        setPropertyFlag(GraphicInstance, m_local, PropertyFlags::ReadOnly | PropertyFlags::Hidden, true);

        return true;
    }

    //--------------------------------------------------------------------------------------
    GraphicInstance::GraphicInstance(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent),
        m_atomicFlags(0x0)
    {
        ClearPickingID();
    }

    //--------------------------------------------------------------------------------------
    GraphicInstance::~GraphicInstance()
    {
        for (uint i = 0; i < m_materials.size(); ++i)
            VG_SAFE_RELEASE(m_materials[i]);
        m_materials.clear();
    }

    //--------------------------------------------------------------------------------------
    void GraphicInstance::ClearPickingID()
    {
        m_pickingID = (PickingID)0;
    }

    //--------------------------------------------------------------------------------------
    void GraphicInstance::SetPickingID(PickingID _id)
    {
        VG_ASSERT((PickingID)0 == m_pickingID, "GraphicInstance \"%s\" already uses PickingID %u", GetName().c_str(), m_pickingID);
        m_pickingID = _id;
    }

    //--------------------------------------------------------------------------------------
    PickingID GraphicInstance::GetPickingID() const
    {
        return m_pickingID;
    }

    //--------------------------------------------------------------------------------------
    bool GraphicInstance::SetMaterial(core::uint _index, IMaterialModel * _materialModel)
    {
        return setMaterial(_index, (MaterialModel*)_materialModel);
    }

    //--------------------------------------------------------------------------------------
    IMaterialModel * GraphicInstance::GetMaterial(core::uint _index) const
    {
        return (IMaterialModel *)getMaterial(_index);
    }

    //--------------------------------------------------------------------------------------
    void GraphicInstance::SetBatchMask(const core::BitMask & _batchMask)
    {
        m_batchMask = _batchMask;
    }

    //--------------------------------------------------------------------------------------
    bool GraphicInstance::setMaterial(core::uint _index, MaterialModel * _materialModel)
    {
        if (m_materials.size() < _index + 1)
            m_materials.resize(_index + 1);

        if (m_materials[_index] != _materialModel)
        {
            VG_SAFE_RELEASE(m_materials[_index]);
            VG_SAFE_INCREASE_REFCOUNT(_materialModel);
            m_materials[_index] = _materialModel;
            OnMaterialChanged(_index);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    MaterialModel * GraphicInstance::getMaterial(core::uint _index) const
    {
        if (_index < m_materials.size())
            return m_materials[_index];
        else
            return nullptr;
    }
}