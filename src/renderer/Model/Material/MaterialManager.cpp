#include "renderer/Precomp.h"
#include "MaterialManager.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    MaterialDataGPUHandle MaterialManager::allocMaterialDataGPUHandle(MaterialModel * _material)
    {
        MaterialDataGPUHandle handle = m_handles.alloc();
        if (m_materials.size() < handle + 1)
            m_materials.resize(handle + 1);
        m_materials[handle] = _material;
        return handle;
    }

    //--------------------------------------------------------------------------------------
    void MaterialManager::freeMaterialDataGPUHandle(MaterialDataGPUHandle & _handle)
    {
        m_materials[_handle] = nullptr;
        m_handles.dealloc(_handle);
    }
}