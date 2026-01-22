#include "DefaultMaterialData.h"

#if !VG_ENABLE_INLINE
#include "DefaultMaterialData.inl"
#endif

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS(DefaultMaterialData, "Default Material Data");
    
    //--------------------------------------------------------------------------------------
    bool IsNonOpaquePropertyHidden(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        const DefaultMaterialData * mat = VG_SAFE_STATIC_CAST(const DefaultMaterialData, _object);
        return SurfaceType::Opaque == mat->getSurfaceType();
    }

    //--------------------------------------------------------------------------------------
    bool IsDepthFadePropertyHidden(const IObject * _object, const IProperty * _prop, core::uint _index)
    {
        const DefaultMaterialData * mat = VG_SAFE_STATIC_CAST(const DefaultMaterialData, _object);
        return SurfaceType::AlphaBlend != mat->getSurfaceType();
    }

    //--------------------------------------------------------------------------------------
    bool IsTilingAndOffsetHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const DefaultMaterialData * mat = VG_SAFE_STATIC_CAST(const DefaultMaterialData, _object);
        return mat->getUVSource() == UVSource::FlipBook;
    }

    //--------------------------------------------------------------------------------------
    bool IsAtlasSizeAndFrameHidden(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const DefaultMaterialData * mat = VG_SAFE_STATIC_CAST(const DefaultMaterialData, _object);
        return mat->getUVSource() != UVSource::FlipBook;
    }

    //--------------------------------------------------------------------------------------
    float2 getFlipBoolFrameRange(const IObject * _object, const IProperty * _prop, uint _index)
    {
        const DefaultMaterialData * mat = VG_SAFE_STATIC_CAST(const DefaultMaterialData, _object);
        return float2(0, mat->getFrameCount());
    }

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialData::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        // Alpha
        setPropertyFlag(MaterialData, m_name, PropertyFlags::Hidden | PropertyFlags::Transient, true);

        registerPropertyEnum(DefaultMaterialData, SurfaceType, m_surfaceType, "Surface");
        setPropertyDescription(DefaultMaterialData, m_surfaceType, "Surface type determines how the material deal with alpha transparency");
        
        registerPropertyEnum(DefaultMaterialData, AlphaSource, m_alphaSource, "Alpha");
        setPropertyDescription(DefaultMaterialData, m_alphaSource, "Value used for alpha transparency");
        setPropertyHiddenCallback(DefaultMaterialData, m_alphaSource, IsNonOpaquePropertyHidden);

        registerProperty(DefaultMaterialData, m_depthFade, "Depth fade");
        setPropertyDescription(DefaultMaterialData, m_depthFade, "Distance used to fade out transparent objects intersecting opaque");
        setPropertyRange(DefaultMaterialData, m_depthFade, float2(0.001f, 10.0f));
        setPropertyHiddenCallback(DefaultMaterialData, m_depthFade, IsDepthFadePropertyHidden);

        // Instance & vertex colors/alpha
        registerPropertyEnumBitfield(DefaultMaterialData, InstanceColorMask, m_instanceColorMask, "Instance Color");
        setPropertyDescription(DefaultMaterialData, m_instanceColorMask, "Instance color usage");

        registerPropertyEnumBitfield(DefaultMaterialData, VertexColorMask, m_vertexColorMask, "Vertex Color");
        setPropertyDescription(DefaultMaterialData, m_vertexColorMask, "Vertex color usage");

        // Culling
        registerPropertyEnum(DefaultMaterialData, CullMode, m_cullMode, "Cull");
        setPropertyDescription(DefaultMaterialData, m_cullMode, "Change culling to hide backfaces, frontfaces, or none");

        // UV
        registerPropertyEnum(DefaultMaterialData, UVSource, m_UVSource, "UV Source");
        setPropertyDescription(DefaultMaterialData, m_UVSource, "Select UV source to use in shader");

        registerProperty(DefaultMaterialData, m_tiling, "Tiling");
        setPropertyDescription(DefaultMaterialData, m_tiling, "Texture coordordinates repetition rate");
        setPropertyRange(DefaultMaterialData, m_tiling, float2(0, 16));
        setPropertyHiddenCallback(DefaultMaterialData, m_tiling, IsTilingAndOffsetHidden);

        registerProperty(DefaultMaterialData, m_offset, "Offset");
        setPropertyDescription(DefaultMaterialData, m_offset, "Texture coordinates offset");
        setPropertyRange(DefaultMaterialData, m_offset, float2(-8, 8));
        setPropertyHiddenCallback(DefaultMaterialData, m_offset, IsTilingAndOffsetHidden);

        registerPropertyAlias(DefaultMaterialData, float2, m_tiling, "Atlas size");
        setPropertyDescription(DefaultMaterialData, m_tiling, "FlipBook atlas size");
        setPropertyRange(DefaultMaterialData, m_tiling, float2(0, 256));
        setPropertyHiddenCallback(DefaultMaterialData, m_tiling, IsAtlasSizeAndFrameHidden);

        registerPropertyAlias(DefaultMaterialData, float, m_offset, "Frame");
        setPropertyDescription(DefaultMaterialData, m_offset, "FlipBook frame");
        setPropertyRange(DefaultMaterialData, m_offset, float2(0, 256));
        setPropertyRangeCallback(CameraSettings, m_offset, getFlipBoolFrameRange); // Range depends on atlas size
        setPropertyHiddenCallback(DefaultMaterialData, m_offset, IsAtlasSizeAndFrameHidden);
        
        // Albedo
        registerPropertyEx(DefaultMaterialData, m_enableAlbedo, "Albedo", PropertyFlags::Hidden);
        registerPropertyOptionalGroupBegin(DefaultMaterialData, m_enableAlbedo, "Albedo");
        {
            registerPropertyResource(DefaultMaterialData, m_albedoMap, "Albedo Map");
            setPropertyDescription(DefaultMaterialData, m_albedoMap, "Base color texture");

            registerPropertyEx(DefaultMaterialData, m_albedoColor, "Albedo Color", PropertyFlags::Color);
            setPropertyDescription(DefaultMaterialData, m_albedoColor, "Base color multiplier");
        }
        registerPropertyOptionalGroupEnd(DefaultMaterialData);

        // Normal
        registerPropertyEx(DefaultMaterialData, m_enableNormal, "Enable Normal", PropertyFlags::Hidden);
        registerPropertyOptionalGroupBegin(DefaultMaterialData, m_enableNormal, "Normal");
        {
            registerPropertyResource(DefaultMaterialData, m_normalMap, "Normal Map"); 
            setPropertyDescription(DefaultMaterialData, m_normalMap, "Normal map is used to simulate fine details on a surface by altering the surface normals");

            registerPropertyEx(DefaultMaterialData, m_normalStrength, "Normal Strength", PropertyFlags::None);
            setPropertyDescription(DefaultMaterialData, m_normalStrength, "Multiplier for Normal Map intensity");
            setPropertyRange(DefaultMaterialData, m_normalStrength, float2(0.0f, 1.0f));
        }
        registerPropertyOptionalGroupEnd(DefaultMaterialData);

        // PBR
        registerPropertyEx(DefaultMaterialData, m_enablePbr, "Enable PBR", PropertyFlags::Hidden);
        registerPropertyOptionalGroupBegin(DefaultMaterialData, m_enablePbr, "PBR");
        {
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
        }
        registerPropertyOptionalGroupEnd(DefaultMaterialData);

        // Emissive
        registerPropertyEx(DefaultMaterialData, m_enableEmissive, "Emissive", PropertyFlags::Hidden);
        registerPropertyOptionalGroupBegin(DefaultMaterialData, m_enableEmissive, "Emissive");
        {
            registerPropertyResource(DefaultMaterialData, m_emissiveMap, "Emissive Map");
            setPropertyDescription(DefaultMaterialData, m_emissiveMap, "Emissive texture");

            registerPropertyEx(DefaultMaterialData, m_emissiveColor, "Emissive Color", PropertyFlags::Color);
            setPropertyDescription(DefaultMaterialData, m_emissiveColor, "Emissive Color");

            registerProperty(DefaultMaterialData, m_emissiveIntensity, "Emissive Intensity");
            setPropertyDescription(DefaultMaterialData, m_emissiveIntensity, "Emissive Color");
            setPropertyRange(DefaultMaterialData, m_emissiveIntensity, float2(0.0f, 1000.0f));
        }
        registerPropertyOptionalGroupEnd(DefaultMaterialData);

        return true;
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialData::DefaultMaterialData(const core::string & _name, IObject * _parent) :
        super(_name, _parent),
        m_surfaceType(SurfaceType::Opaque),
        m_cullMode(CullMode::Back), 
        m_alphaSource(AlphaSource::Albedo),
        m_depthFade(0.1f),
        m_instanceColorMask(InstanceColorMask::Albedo),
        m_vertexColorMask(VertexColorMask::Albedo),
        m_UVSource(UVSource::UV0),
        m_tiling(core::float2(1, 1)),
        m_offset(core::float2(0, 0)),
        m_enableAlbedo(true),
        m_albedoColor(core::float4(1, 1, 1, 1)),
        m_enableNormal(true), 
        m_normalStrength(1.0f),             
        m_enablePbr(true),
        m_occlusion(1.0f),
        m_roughness(1.0f),
        m_metalness(1.0f),
        m_enableEmissive(true),
        m_emissiveColor(core::float4(1, 1, 1, 1)),
        m_emissiveIntensity(1.0f)
    {
        m_albedoMap.SetParent(this);
        m_normalMap.SetParent(this);
        m_pbrMap.SetParent(this);
        m_emissiveMap.SetParent(this);
    }

    //--------------------------------------------------------------------------------------
    DefaultMaterialData::~DefaultMaterialData()
    {

    }

    //--------------------------------------------------------------------------------------
    renderer::IMaterialModel * DefaultMaterialData::CreateRendererMaterialModel() const
    {
        IFactory * factory = Kernel::getFactory();
        auto * material = (renderer::IMaterialModel*)factory->CreateObject("DefaultMaterialModel", "Default");

        // This is generic code that could go to parent class and be shared by all material types in the future
        {
            const IClassDesc * srcClassDesc = GetClassDesc();
            const IClassDesc * dstClassDesc = material->GetClassDesc();

            for (uint i = 0; i < srcClassDesc->GetPropertyCount(); ++i)
            {
                const auto & srcProp = srcClassDesc->GetPropertyByIndex(i);

                if (srcProp->GetType() == PropertyType::LayoutElement)
                    continue;

                if (auto * dstProp = dstClassDesc->GetPropertyByName(srcProp->GetName()))
                {
                    if (factory->CanCopyProperty(srcClassDesc, srcProp, dstClassDesc, dstProp))
                        factory->CopyProperty(srcProp, this, dstProp, material);
                }
            }
        }
        
        return material;
    }   

    //--------------------------------------------------------------------------------------
    bool DefaultMaterialData::RegisterUID()
    {
        bool result = super::RegisterUID();

        result &= m_albedoMap.RegisterUID();
        result &= m_normalMap.RegisterUID();
        result &= m_pbrMap.RegisterUID();
        result &= m_emissiveMap.RegisterUID();

        return result;
    }
}