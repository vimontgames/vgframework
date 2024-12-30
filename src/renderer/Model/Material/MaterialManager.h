#pragma once

#include "core/Singleton/Singleton.h"
#include "Material_Consts.h"
#include "core/Pool/IndexPool.h"

namespace vg::renderer
{
    class MaterialModel;
    class MaterialManager : public core::Singleton<MaterialManager>
    {
    public:
        GPUMaterialDataIndex   allocMaterialDataGPUIndex  (MaterialModel * _material);
        void                    freeMaterialDataGPUIndex   (GPUMaterialDataIndex & _handle);

        VG_INLINE const core::vector<MaterialModel *> & getMaterials() const { return m_materials; }

    private:
        core::IndexPool<GPUMaterialDataIndex, s_MaxMaterialCount>   m_handles;
        core::vector<MaterialModel*>                                m_materials;
    };
}