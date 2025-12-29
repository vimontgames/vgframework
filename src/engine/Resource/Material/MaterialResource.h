#pragma once

#include "engine/IMaterialResource.h"
#include "engine/Resource/Texture/TextureResource.h"
#include "renderer/Model/Material/Material_Consts.h"

namespace vg::renderer
{
    class IMaterialModel;
}

namespace vg::engine
{
    class MaterialData;

    class MaterialResource final : public IMaterialResource
    {
    public:
        VG_CLASS_DECL(MaterialResource, IMaterialResource);

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

        MatPropIndex                        GetPropertyIndex(const core::string & _propertyName) const final override;
        void                                SetProperty     (MatPropIndex _propertyIndex, float _value) final override;

    private:
        MaterialData *                      getMaterialData () const;
    };
}