#include "DefaultMaterialData.h"

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(DefaultMaterialData, "Default Material Data");

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

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