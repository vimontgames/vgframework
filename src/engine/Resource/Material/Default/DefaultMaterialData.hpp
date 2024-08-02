#include "DefaultMaterialData.h"

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(DefaultMaterialData, "Default Material Data");

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(DefaultMaterialData, UVSource, m_UVSource, "UV Source");

        registerProperty(DefaultMaterialData, m_tiling, "Tiling");
        setPropertyRange(DefaultMaterialData, m_tiling, float2(0, 16));

        registerPropertyResource(DefaultMaterialData, m_albedoMap, "Albedo Map");
        registerPropertyEx(DefaultMaterialData, m_albedoColor, "Albedo Color", IProperty::Flags::Color);

        registerPropertyResource(DefaultMaterialData, m_normalMap, "Normal Map"); 
        registerProperty(DefaultMaterialData, m_normalStrength, "Normal Strength");
        setPropertyRange(DefaultMaterialData, m_normalStrength, float2(0.0f, 1.0f));

        registerPropertyResource(DefaultMaterialData, m_pbrMap, "PBR Map");
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
        m_pbrMap.setParent(this);
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
                if (auto * dstProp = dstClassDesc->GetPropertyByName(srcProp->getName()))
                {
                    if (factory->CanCopyProperty(srcProp, dstProp))
                        factory->CopyProperty(srcProp, this, dstProp, material);
                }
            }
        }
        
        return material;
    }   
}