#pragma once

#include "core/Singleton/Singleton.h"
#include "Material_Consts.h"
#include "core/Pool/IndexPool.h"
#include "core/Scheduler/Mutex.h"

namespace vg::renderer
{
    class MaterialModel;
    class MaterialManager : public core::Singleton<MaterialManager>
    {
    public:
        GPUMaterialDataIndex                    allocMaterialDataGPUIndex   (MaterialModel * _material);
        void                                    freeMaterialDataGPUIndex    (GPUMaterialDataIndex & _handle);
        const core::vector<MaterialModel *> &   getMaterialsSafeCopy        ();

    private:
        core::IndexPool<GPUMaterialDataIndex, s_MaxMaterialCount>   m_handles;
        core::vector<MaterialModel*>                                m_materials;
        core::vector<MaterialModel *>                               m_materialsSafeCopy;
        core::mutex                                                 m_mutex;
    };
}