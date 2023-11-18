#pragma once

#include "core/Object/Object.h"

namespace vg::renderer
{
    enum MaterialFlags : core::u32;
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

    protected:
        renderer::IMaterialModel * getMaterialModel() const;

    private:
        renderer::MaterialFlags m_flags;
    };
}