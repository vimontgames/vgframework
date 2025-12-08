#pragma once

#include "core/Resource/Resource.h"

namespace vg::renderer
{
    class ICameraLens;
}

namespace vg::engine
{
    class LensResource : public core::Resource
    {
    public:
        VG_CLASS_DECL(LensResource, core::Resource);

        LensResource(const core::string & _name = "", IObject * _parent = nullptr);
        ~LensResource();

        const core::vector<core::string>    GetExtensions   () const final;

        core::LoadStatus                    Load            (const core::string & _file, core::IObject *& _object) final override;

        bool                                CreateFile      (const core::string & _path, core::IObject * _data) final override;
        bool                                SaveFile        (const core::string & _path) const override;

        bool                                CanCreateFile   () const final override { return true; };
        bool                                CanSaveFile     () const final override { return true; };

    private:
        renderer::ICameraLens *             m_cameraLens    = nullptr;
    };
}