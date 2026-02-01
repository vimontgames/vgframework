#pragma once

#include "core/Object/Object.h"
#include "renderer/Model/Material/Material_Consts.h"

namespace vg::renderer
{
    class IMaterialModel;
}

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    // Base class for serialized material data
    //--------------------------------------------------------------------------------------
    class MaterialData : public core::Object
    {
    public:
        VG_CLASS_DECL(MaterialData, core::Object); // Should be abstract but has custom ctor

        MaterialData(const core::string & _name = "", IObject * _parent = nullptr);
        ~MaterialData();

        virtual renderer::IMaterialModel *  CreateRendererMaterialModel () const = 0;

        void                                OnPropertyChanged           (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

        void                                OnLoad                      () override;

        void                                OnResourceLoaded            (core::IResource * _resource) override;
        void                                OnResourceUnloaded          (core::IResource * _resource) override;

        renderer::IMaterialModel *          getMaterialModel            () const;

    protected:
        const core::IProperty *             findTextureResourceProperty (core::IResource * _resource) const;
    };
}

#if VG_ENABLE_INLINE
#include "MaterialData.inl"
#endif