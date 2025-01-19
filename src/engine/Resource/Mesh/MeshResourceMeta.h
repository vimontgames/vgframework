#pragma once

#include "core/Resource/ResourceMeta.h"
#include "gfx/Importer/MeshImporterSettings.h"

namespace vg::engine
{
    class MeshResourceMeta : public core::ResourceMeta
    {
    public:
        VG_CLASS_DECL(MeshResourceMeta, core::ResourceMeta)

        MeshResourceMeta(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~MeshResourceMeta();

        const gfx::MeshImporterSettings & getMeshImportSettings() const { return m_importSettings; }

    private:
        gfx::MeshImporterSettings m_importSettings;
    };
}