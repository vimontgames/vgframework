#pragma once

#include "gfx/RootSignature/RootSignature_consts.h"
#include "gfx/Shader/ShaderKey.h"

#include "renderer/IMaterialModel.h"
#include "renderer/Model/Material/Material_Consts.h"

struct RootConstants3D;
struct GPUMaterialData;

enum class SurfaceType : vg::core::u8;
enum class AlphaSource : vg::core::u8;

namespace vg::gfx
{
    class CommandList;
    class Texture;
    class Buffer;

    enum class CullMode : core::u8;
}

namespace vg::renderer
{
    class RenderContext;
    class MaterialImporterData;

    vg_enum_class(vg::renderer, ShaderPass, core::u8,
        ZOnly = 0,
        Forward,
        Deferred,
        Transparent,
        Outline
    );

    class MaterialModel : public IMaterialModel
    {
        using super = IMaterialModel;

    public:

        const char *                    GetClassName            () const = 0;

        static bool                     registerProperties      (core::IClassDesc & _desc);

                                        MaterialModel           (const core::string & _name, core::IObject * _parent = nullptr);
                                        ~MaterialModel          ();

        virtual void                    FillGPUMaterialData     (GPUMaterialData * _data) const = 0;

        virtual void                    Setup                   (const RenderContext & _renderContext, gfx::CommandList * _cmdList, RootConstants3D * _root3DConstants, core::uint _index) const = 0;

        virtual SurfaceType             GetSurfaceType          () const = 0;
        virtual SurfaceTypeFlags        GetSurfaceTypeFlags     () const = 0;

        VG_INLINE GPUMaterialDataIndex  getGPUMaterialDataIndex () const { return m_gpuMaterialDataIndex; }

    protected:
        GPUMaterialDataIndex            m_gpuMaterialDataIndex = (GPUMaterialDataIndex)-1;
        gfx::RootSignatureHandle        m_rootSignature;
        gfx::ShaderKey                  m_shaderKey[core::enumCount<ShaderPass>()];
    };
}
