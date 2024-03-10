#pragma once

#include "core/Model/Model.h"
#include "renderer/Model/Material/Material_Consts.h"

namespace vg
{
    namespace gfx
    {
        class ITexture;
    }

    namespace renderer
    {
        class IMaterialModel : public core::Model
        {
        public:
            VG_CLASS_DECL_ABSTRACT(IMaterialModel, core::Model);

            virtual void SetSurfaceType(gfx::SurfaceType _surfaceType) = 0;
            virtual void SetColor(const core::string & _name, core::float4 _value) = 0;
            virtual void SetTexture(const core::string & _name, gfx::ITexture * _value) = 0;
            virtual void SetFloat(const core::string & _name, float _value) = 0;
            virtual void SetFloat2(const core::string & _name, core::float2 _value) = 0;
        };
    }
}