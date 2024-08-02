#pragma once

#include "core/Object/Object.h"
#include "renderer/Model/Material/Material_Consts.h"

namespace vg::gfx
{
    enum class SurfaceType : core::u8;
    enum class CullMode : core::u8;
}

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
        VG_CLASS_DECL(MaterialData, core::Object);

        MaterialData(const core::string & _name = "", IObject * _parent = nullptr);
        ~MaterialData();

        virtual renderer::IMaterialModel *  CreateRendererMaterialModel () const = 0;

        void                                OnPropertyChanged           (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

        void                                onResourceLoaded            (core::IResource * _resource) override;
        void                                onResourceUnloaded          (core::IResource * _resource) override;

    protected:
        renderer::IMaterialModel *          getMaterialModel            () const;
        const core::IProperty *             findTextureResourceProperty (core::IResource * _resource) const;

    protected:
        gfx::SurfaceType                    m_surfaceType;
        gfx::CullMode                       m_cullMode;
    };
}