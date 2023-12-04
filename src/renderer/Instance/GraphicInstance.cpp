#include "renderer/Precomp.h"
#include "GraphicInstance.h"

#include "Mesh/MeshInstance.hpp"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    bool GraphicInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyObjectPtrVectorEx(GraphicInstance, m_materials, "Materials", IProperty::Flags::NotSaved);

        return true;
    }

    //--------------------------------------------------------------------------------------
    GraphicInstance::GraphicInstance(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
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
        VG_ASSERT((PickingID)0 == m_pickingID, "GraphicInstance \"%s\" already uses PickingID %u", getName().c_str(), m_pickingID);
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
    bool GraphicInstance::setMaterial(core::uint _index, MaterialModel * _materialModel)
    {
        if (m_materials.size() < _index + 1)
            m_materials.resize(_index + 1);

        if (m_materials[_index] != _materialModel)
        {
            VG_SAFE_RELEASE(m_materials[_index]);
            VG_SAFE_INCREASE_REFCOUNT(_materialModel);
            m_materials[_index] = _materialModel;
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