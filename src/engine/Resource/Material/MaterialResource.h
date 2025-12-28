#pragma once

#include "core/Resource/Resource.h"
#include "engine/Resource/Texture/TextureResource.h"
#include "renderer/Model/Material/Material_Consts.h"

namespace vg::renderer
{
    class IMaterialModel;
}

namespace vg::engine
{
    class MaterialResource final : public core::Resource
    {
    public:
        VG_CLASS_DECL(MaterialResource, core::Resource);

        MaterialResource                    (const core::string & _name = "", IObject * _parent = nullptr);
        ~MaterialResource                   ();

        const core::vector<core::string>    GetExtensions   () const final;

        bool                                Cook            (const core::string & _file) const final override;
        core::LoadStatus                    Load            (const core::string & _file, core::IObject *& _object) final override;

        bool                                CreateFile      (const core::string & _path, core::IObject * _data) final override;
        bool                                SaveFile        (const core::string & _path) const override;

        bool                                CanCreateFile   () const final override { return !IsInstanciated(); };
        bool                                CanSaveFile     () const final override { return !IsInstanciated(); };
        bool                                CanInstanciate  () const final override { return true; };

        bool                                Instanciate     () final override;
    };
}