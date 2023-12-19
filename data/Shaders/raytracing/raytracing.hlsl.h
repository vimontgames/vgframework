#pragma once
#include "gfx/Shader/HLSLDesc.h"

namespace vg::gfx
{
    class RayTracingHLSLDesc : public HLSLDesc
    {
    public:
        RayTracingHLSLDesc()
        {
            enum Flags : ShaderKey::Flags
            {

            };

            setFile("raytracing/raytracing.hlsl");

            auto & raytracingTechnique = addRayTracingTechnique("RayTracing");
            {
                raytracingTechnique.setRayGenShader("RayGen");
                raytracingTechnique.setClosestHitShader("ClosestHit");
                raytracingTechnique.setMissShader("MissU2");
            }
        }
    };
}