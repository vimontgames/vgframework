#pragma once

#include "core/Object/Object.h"
#include "engine/Resource/Material/MaterialModelType.h"

namespace vg::renderer
{
    class IMaterialModel;
}

namespace vg::engine
{
    class MaterialData;

    //--------------------------------------------------------------------------------------
    // Shader name + custom data
    //--------------------------------------------------------------------------------------
    class MaterialResourceData : public core::Object
    {
    public:
        VG_CLASS_DECL(MaterialResourceData, core::Object);

        MaterialResourceData(const core::string & _name = "", IObject * _parent = nullptr);
        ~MaterialResourceData();
        
        void                        CreateRendererMaterial  ();

        void                        OnPropertyChanged       (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

        MaterialData *              getMaterialData         () const { return m_data; }

        MaterialModelType           m_materialModelType;
        MaterialData *              m_data = nullptr;
        renderer::IMaterialModel *  m_materialModel = nullptr;
    };
}