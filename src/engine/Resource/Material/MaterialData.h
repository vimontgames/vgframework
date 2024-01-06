#pragma once

#include "core/Object/Object.h"

namespace vg::gfx
{
    enum class SurfaceType : core::u8;
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

        void                                OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent) override;

    protected:
        renderer::IMaterialModel *          getMaterialModel() const;

    protected:
        gfx::SurfaceType                    m_surfaceType;
    };
}