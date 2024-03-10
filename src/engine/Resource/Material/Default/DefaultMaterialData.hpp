#include "DefaultMaterialData.h"

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(DefaultMaterialData, "Default Material Data");

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(DefaultMaterialData, m_tiling, "Tiling");
        setPropertyRange(DefaultMaterialData, m_tiling, float2(0, 16));

        registerPropertyResource(DefaultMaterialData, m_albedoMap, "Albedo Map");
        registerPropertyEx(DefaultMaterialData, m_albedoColor, "Albedo Color", IProperty::Flags::Color);

        registerPropertyResource(DefaultMaterialData, m_normalMap, "Normal Map"); 
        registerProperty(DefaultMaterialData, m_normalStrength, "Normal Strength");
        setPropertyRange(DefaultMaterialData, m_normalStrength, float2(0.0f, 1.0f));

        registerPropertyResource(DefaultMaterialData, m_pbr, "PBR Map");
        registerProperty(DefaultMaterialData, m_occlusion, "Occlusion");
        setPropertyRange(DefaultMaterialData, m_occlusion, float2(0.0f, 1.0f));
        registerProperty(DefaultMaterialData, m_roughness, "Roughness");
        setPropertyRange(DefaultMaterialData, m_roughness, float2(0.0f, 1.0f));
        registerProperty(DefaultMaterialData, m_metalness, "Metalness");
        setPropertyRange(DefaultMaterialData, m_metalness, float2(0.0f, 1.0f));

        return true;
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialData::DefaultMaterialData(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        m_albedoMap.setParent(this);
        m_normalMap.setParent(this);
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialData::~DefaultMaterialData()
    {

    }

    //--------------------------------------------------------------------------------------
    renderer::IMaterialModel * DefaultMaterialData::CreateRendererMaterialModel() const
    {
        IFactory * factory = Kernel::getFactory();
        auto * material = (renderer::IMaterialModel*)factory->createObject("DefaultMaterialModel", "Default");
        material->setParent((IObject*)this);

        material->SetSurfaceType(m_surfaceType);
        
        material->SetFloat2("Tiling", m_tiling);
        material->SetColor("AlbedoColor", m_albedoColor);
        material->SetFloat("NormalStrength", m_normalStrength);
        material->SetFloat("Occlusion", m_occlusion);
        material->SetFloat("Roughness", m_roughness);
        material->SetFloat("Metalness", m_metalness);
        
        return material;
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialData::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        auto material = getMaterialModel();

        if (!strcmp(_prop.getName(), "m_albedoColor"))
            material->SetColor("AlbedoColor", m_albedoColor);
        else if (!strcmp(_prop.getName(), "m_normalStrength"))
            material->SetFloat("NormalStrength", m_normalStrength);
        else if (!strcmp(_prop.getName(), "m_occlusion"))
            material->SetFloat("Occlusion", m_occlusion);
        else if (!strcmp(_prop.getName(), "m_roughness"))
            material->SetFloat("Roughness", m_roughness);
        else if (!strcmp(_prop.getName(), "m_metalness"))
            material->SetFloat("Metalness", m_metalness);
        else if (!strcmp(_prop.getName(), "m_tiling"))
            material->SetFloat2("Tiling", m_tiling);

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialData::onResourceLoaded(core::IResource * _resource)
    {
        auto material = getMaterialModel();

        if (_resource == &m_albedoMap)
            material->SetTexture("AlbedoMap", (gfx::ITexture *)_resource->getObject());
        else if (_resource == &m_normalMap)
            material->SetTexture("NormalMap", (gfx::ITexture *)_resource->getObject());
        else if (_resource == &m_pbr)
            material->SetTexture("PBRMap", (gfx::ITexture *)_resource->getObject());
    }

    //--------------------------------------------------------------------------------------
    void DefaultMaterialData::onResourceUnloaded(core::IResource * _resource)
    {
        auto material = getMaterialModel();

        if (_resource == &m_albedoMap)
            material->SetTexture("AlbedoMap", nullptr);
        else if (_resource == &m_normalMap)
            material->SetTexture("NormalMap", nullptr);
        else if (_resource == &m_pbr)
            material->SetTexture("PBRMap", nullptr);
    }
}