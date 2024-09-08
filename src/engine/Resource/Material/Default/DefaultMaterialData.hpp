#include "DefaultMaterialData.h"

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(DefaultMaterialData, "Default Material Data");

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(DefaultMaterialData, UVSource, m_UVSource, "UV Source");
        setPropertyDescription(DefaultMaterialData, m_UVSource, "Select UV source to use in shader");

        registerProperty(DefaultMaterialData, m_tiling, "Tiling");
        setPropertyDescription(DefaultMaterialData, m_tiling, "Texture repetition rate");
        setPropertyRange(DefaultMaterialData, m_tiling, float2(0, 16));

        registerPropertyResource(DefaultMaterialData, m_albedoMap, "Albedo Map");
        setPropertyDescription(DefaultMaterialData, m_albedoMap, "Base color texture");

        registerPropertyEx(DefaultMaterialData, m_albedoColor, "Albedo Color", PropertyFlags::Color);
        setPropertyDescription(DefaultMaterialData, m_albedoColor, "Base color multiplier");

        registerPropertyResource(DefaultMaterialData, m_normalMap, "Normal Map"); 
        setPropertyDescription(DefaultMaterialData, m_normalMap, "Normal map is used to simulate fine details on a surface by altering the surface normals");

        registerPropertyEx(DefaultMaterialData, m_normalStrength, "Normal Strength", PropertyFlags::None);
        setPropertyDescription(DefaultMaterialData, m_normalStrength, "Multiplier for Normal Map intensity");
        setPropertyRange(DefaultMaterialData, m_normalStrength, float2(0.0f, 1.0f));

        registerPropertyResource(DefaultMaterialData, m_pbrMap, "PBR Map");
        setPropertyDescription(DefaultMaterialData, m_pbrMap, "This texture holds lighting parameters:\nR: Ambient Occlusion\nG: Roughness\nB: Metalness");

        registerPropertyEx(DefaultMaterialData, m_occlusion, "Occlusion", PropertyFlags::None);
        setPropertyDescription(DefaultMaterialData, m_occlusion, "Amount of Ambient Occlusion from PBR texture red channel");
        setPropertyRange(DefaultMaterialData, m_occlusion, float2(0.0f, 1.0f));

        registerPropertyEx(DefaultMaterialData, m_roughness, "Roughness", PropertyFlags::None);
        setPropertyDescription(DefaultMaterialData, m_roughness, "Amount of Roughness from PBR texture green channel");
        setPropertyRange(DefaultMaterialData, m_roughness, float2(0.0f, 1.0f));

        registerPropertyEx(DefaultMaterialData, m_metalness, "Metalness", PropertyFlags::None);
        setPropertyDescription(DefaultMaterialData, m_metalness, "Amount of Metalness from PBR texture blue channel");
        setPropertyRange(DefaultMaterialData, m_metalness, float2(0.0f, 1.0f));

        return true;
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialData::DefaultMaterialData(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        m_albedoMap.SetParent(this);
        m_normalMap.SetParent(this);
        m_pbrMap.SetParent(this);
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

        // This is generic code that could go to parent class and be shared by all material types in the future
        {
            const IClassDesc * srcClassDesc = GetClassDesc();
            const IClassDesc * dstClassDesc = material->GetClassDesc();

            for (uint i = 0; i < srcClassDesc->GetPropertyCount(); ++i)
            {
                const auto & srcProp = srcClassDesc->GetPropertyByIndex(i);
                if (auto * dstProp = dstClassDesc->GetPropertyByName(srcProp->GetName()))
                {
                    if (factory->CanCopyProperty(srcProp, dstProp))
                        factory->CopyProperty(srcProp, this, dstProp, material);
                }
            }
        }
        
        return material;
    }   
}