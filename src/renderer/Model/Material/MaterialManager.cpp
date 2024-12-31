#include "renderer/Precomp.h"
#include "MaterialManager.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    GPUMaterialDataIndex MaterialManager::allocMaterialDataGPUIndex(MaterialModel * _material)
    {
        lock_guard<mutex> lock(m_mutex);

        GPUMaterialDataIndex handle = m_handles.alloc();
        if (m_materials.size() < handle + 1)
            m_materials.resize(handle + 1);
        m_materials[handle] = _material;
        return handle;
    }

    //--------------------------------------------------------------------------------------
    void MaterialManager::freeMaterialDataGPUIndex(GPUMaterialDataIndex & _handle)
    {
        lock_guard<mutex> lock(m_mutex);

        m_materials[_handle] = nullptr;
        m_handles.dealloc(_handle);
    }

    //--------------------------------------------------------------------------------------
    void MaterialManager::getMaterialsSafeCopy(core::vector<MaterialModel *> & _materials)
    {
        lock_guard<mutex> lock(m_mutex);

        VG_PROFILE_CPU("MaterialsSafeCopy");
        _materials = m_materials;
    }
}