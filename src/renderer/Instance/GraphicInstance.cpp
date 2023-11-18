#include "renderer/Precomp.h"
#include "GraphicInstance.h"

#include "Mesh/MeshInstance.hpp"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    bool GraphicInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerPropertyObjectRefVectorHelper(GraphicInstance, m_materials, "Materials", IProperty::Flags::NotSaved);

        return true;
    }

    //--------------------------------------------------------------------------------------
    GraphicInstance::GraphicInstance(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    GraphicInstance::~GraphicInstance()
    {
        for (uint i = 0; i < m_materials.size(); ++i)
            VG_SAFE_RELEASE(m_materials[i]);
        m_materials.clear();
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
        if (_index < m_materials.size() + 1)
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