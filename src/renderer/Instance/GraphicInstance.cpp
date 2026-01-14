#include "renderer/Precomp.h"
#include "GraphicInstance.h"

#if !VG_ENABLE_INLINE
#include "GraphicInstance.inl"
#endif

#include "core/Object/AutoRegisterClass.h"
#include "core/Math/Math.h"
#include "gfx/FrameGraph/RenderPassContext.h"

#include "Mesh/MeshInstance.hpp"
#include "ParticleSystem/ParticleEmitter.hpp"
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
    void GraphicInstance::OnLoad()
    {
        RegisterUID();
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
    bool GraphicInstance::SetMaterialCount(core::uint _count)
    {
        return setMaterialCount(_count);
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
        OnMaterialChanged();
    }

    //--------------------------------------------------------------------------------------
    SurfaceTypeFlags GraphicInstance::computeSurfaceTypeFlags() const
    {
        SurfaceTypeFlags flags = (SurfaceTypeFlags)0x0;

        auto * defaultMaterial = Renderer::get()->getDefaultMaterial(DefaultMaterialType::Opaque);

        const auto & materials = getMaterials();
        const auto & batchMask = getBatchMask();

        const bool useBatchMask = batchMask.getBitCount() >= materials.size();
        
        // batchmask size can be greater if materials are not yet loaded, but should not be smaller
        //if (batchMask.getBitCount() < materials.size())
        //    VG_WARNING_ONCE("[GraphicInstance] Material count of %s \"%s\" (%u) is greater than batch mask bit count (%u)", asString(GetGraphicIntanceType()).c_str(), GetName().c_str(), materials.size(), batchMask.getBitCount());

        if (materials.size() > 0)
        {
            for (uint i = 0; i < materials.size(); ++i)
            {
                auto * material = materials[i];

                if (!material)
                    material = defaultMaterial;

                if (useBatchMask && batchMask.getBitValue(i))
                {
                    auto surf = material->GetSurfaceType();

                    switch (surf)
                    {
                        case SurfaceType::Opaque:
                            flags |= SurfaceTypeFlags::Opaque;
                            break;

                        case SurfaceType::AlphaTest:
                            flags |= SurfaceTypeFlags::AlphaTest;
                            break;

                        case SurfaceType::AlphaBlend:
                            flags |= SurfaceTypeFlags::AlphaBlend;
                            break;

                        case SurfaceType::Decal:
                            flags |= SurfaceTypeFlags::Decal;
                            break;
                    }
                }
                else
                {
                    flags |= SurfaceTypeFlags::AlphaBlend;
                }
            }
        }
        else
        {
            flags |= SurfaceTypeFlags::Opaque;
        }

        return flags;
    }

    //--------------------------------------------------------------------------------------
    void GraphicInstance::OnMaterialChanged()
    {
        
    }

    //--------------------------------------------------------------------------------------
    bool GraphicInstance::setMaterialCount(core::uint _count)
    {
        if (_count != m_materials.size())
        {
            if (_count < m_materials.size())
            {
                for (uint i = _count; i < m_materials.size(); ++i)
                {
                    VG_SAFE_RELEASE(m_materials[i]);
                }
                m_materials.resize(_count);
                OnMaterialChanged();
            }
            return true;
        }
        return false;
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
            OnMaterialChanged();
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

    //--------------------------------------------------------------------------------------
    void GraphicInstance::setInstanceIndexBuffer(gfx::Buffer * _buffer, uint _offset)
    {
        m_instanceIndexBuffer = _buffer;
        m_instanceIndexBufferOffset = _offset;
    }

    //--------------------------------------------------------------------------------------
    void GraphicInstance::setInstanceVertexBuffer(gfx::Buffer * _buffer, uint _offset)
    {
        m_instanceVertexBuffer = _buffer;
        m_instanceVertexBufferOffset = _offset;
    }
}