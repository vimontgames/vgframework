#include "MaterialData.h"
#include "gfx/PipelineState/Graphic/RasterizerState_consts.h"

#if !VG_ENABLE_INLINE
#include "MaterialData.inl"
#endif

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    bool IsDepthFadePropertyHidden(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        const MaterialData * mat = VG_SAFE_STATIC_CAST(const MaterialData, _object);
        return gfx::SurfaceType::AlphaBlend != mat->getSurfaceType();
    }

    //--------------------------------------------------------------------------------------
    bool MaterialData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(MaterialData, m_name, PropertyFlags::Hidden | PropertyFlags::NotSaved, true);

        registerPropertyEnum(MaterialData, gfx::SurfaceType, m_surfaceType, "Surface");
        setPropertyDescription(MaterialData, m_surfaceType, "Surface type determines how the material deal with alpha transparency");

        //registerPropertyGroupBegin(MaterialData, "AlphaBlend");
        //setPropertyHiddenCallback(MaterialData, AlphaBlend, IsDepthFadePropertyHidden);
        {
            registerProperty(MaterialData, m_depthFade, "Depth fade");
            setPropertyDescription(MaterialData, m_depthFade, "Distance used to fade out transparent objects intersecting opaque");
            setPropertyRange(MaterialData, m_depthFade, float2(0.001f, 10.0f));
            setPropertyHiddenCallback(MaterialData, m_depthFade, IsDepthFadePropertyHidden);
        }
        //registerPropertyGroupEnd(MaterialData);

        registerPropertyEnum(MaterialData, gfx::CullMode, m_cullMode, "Cull");
        setPropertyDescription(MaterialData, m_cullMode, "Change culling to hide backfaces, frontfaces, or none");
        
        return true;
    }

    //--------------------------------------------------------------------------------------
    MaterialData::MaterialData(const core::string & _name, IObject * _parent) :
        super(_name, _parent),
        m_surfaceType(gfx::SurfaceType::Opaque),
        m_depthFade(0.1f),
        m_cullMode(gfx::CullMode::Back)
    {
        SetName(_parent->GetName());
    }

    //--------------------------------------------------------------------------------------
    MaterialData::~MaterialData()
    {

    }

    //--------------------------------------------------------------------------------------
    renderer::IMaterialModel * MaterialData::getMaterialModel() const
    {
        MaterialResourceData * parent = dynamic_cast<MaterialResourceData *>(GetParent());
        VG_ASSERT(parent);
        renderer::IMaterialModel * materialModel = parent->m_materialModel;
        if (nullptr == materialModel)
            materialModel = parent->m_materialModel;
        return materialModel;
    }

    //--------------------------------------------------------------------------------------
    void MaterialData::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        auto * material = getMaterialModel();

        const IClassDesc * srcClassDesc = GetClassDesc();
        const IClassDesc * dstClassDesc = material->GetClassDesc();

        IFactory * factory = Kernel::getFactory();

        if (auto * dstProp = dstClassDesc->GetPropertyByName(_prop.GetName()))
        {
            if (_prop.GetType() != PropertyType::LayoutElement)
            {
                if (factory->CanCopyProperty(&_prop, dstProp))
                    factory->CopyProperty(&_prop, this, dstProp, material);
            }
        }

        super::OnPropertyChanged(_object, _prop, _notifyParent);
    }

    //--------------------------------------------------------------------------------------
    const core::IProperty * MaterialData::findTextureResourceProperty(core::IResource * _resource) const
    {
        auto * material = getMaterialModel();

        const IClassDesc * dataClassDesc = GetClassDesc();
        const IClassDesc * resourceClassDesc = _resource->GetClassDesc();
        const IClassDesc * matClassDesc = material->GetClassDesc();

        for (uint i = 0; i < dataClassDesc->GetPropertyCount(); ++i)
        {
            const auto & dataProp = dataClassDesc->GetPropertyByIndex(i);

            if (dataProp->IsResourceProperty())
            {
                if (!strcmp(resourceClassDesc->GetClassName(), "TextureResource")) // dynamic_cast<TextureResource*>(dataProp->GetPropertyResource(this)) ?
                {
                    VG_ASSERT(PropertyType::Resource == dataProp->GetType());

                    if (_resource == dataProp->GetPropertyResource(this))
                    {
                        // Bind 'TextureResource' Object to 'Texture' slot with the same name
                        if (const auto * matProp = matClassDesc->GetPropertyByName(dataProp->GetName()))
                            return matProp;
                    }
                }
            }
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void MaterialData::OnResourceLoaded(core::IResource * _resource)
    {
        if (auto material = getMaterialModel())
        {
            if (auto * matProp = findTextureResourceProperty(_resource))
            {
                if (material)
                    material->SetTexture(matProp->GetName(), (gfx::ITexture *)_resource->GetObject());
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void MaterialData::OnResourceUnloaded(core::IResource * _resource)
    {
        auto material = getMaterialModel();

        if (auto * matProp = findTextureResourceProperty(_resource))
            material->SetTexture(matProp->GetName(), nullptr);
    }
}