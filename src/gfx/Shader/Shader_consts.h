#pragma once

namespace vg::gfx
{
    class Shader;

    enum class ShaderProgramType
    {
        Vertex = 0,
        Hull,
        Domain,
        Geometry,
        Pixel,

        Compute,

        RayTracing
    };

    enum class GraphicsStage : core::u8
    {
        Vertex = 0,
        Hull,
        Domain,
        Geometry,
        Pixel,
    };

    enum class ComputeStage : core::u8
    {
        Compute = 0,
    };

    enum class RayTracingStage : core::u8
    {
        RayGen,
        AnyHit,
        ClosestHit,
        Miss,
        Intersection
    };

    enum class ShaderStage : core::u8
    {
        // [0..4]
        Vertex       = GraphicsStage::Vertex,                       // 0
        Hull         = GraphicsStage::Hull,                         // 1
        Domain       = GraphicsStage::Domain,                       // 2
        Geometry     = GraphicsStage::Geometry,                     // 3
        Pixel        = GraphicsStage::Pixel,                        // 4

        // [5]
        Compute      = 5 + (core::u8)ComputeStage::Compute,         // 5

        // [6..10]                      
        RayGen       = 6 + (core::u8)RayTracingStage::RayGen,       // 6
        AnyHit       = 6 + (core::u8)RayTracingStage::AnyHit,       // 7
        ClosestHit   = 6 + (core::u8)RayTracingStage::ClosestHit,   // 8
        Miss         = 6 + (core::u8)RayTracingStage::Miss,         // 9
        Intersection = 6 + (core::u8)RayTracingStage::Intersection  // 10
    };

    enum class ShaderStageFlags : core::u16
    {
        VS = 1 << core::asInteger(ShaderStage::Vertex),
        HS = 1 << core::asInteger(ShaderStage::Domain),
        DS = 1 << core::asInteger(ShaderStage::Hull),
        GS = 1 << core::asInteger(ShaderStage::Geometry),
        PS = 1 << core::asInteger(ShaderStage::Pixel),

        CS = 1 << core::asInteger(ShaderStage::Compute),

        RG = 1 << core::asInteger(ShaderStage::RayGen),
        AH = 1 << core::asInteger(ShaderStage::AnyHit),
        CH = 1 << core::asInteger(ShaderStage::ClosestHit),
        MI = 1 << core::asInteger(ShaderStage::Miss),
        IS = 1 << core::asInteger(ShaderStage::Intersection),

        AllGraphics    = VS | HS | DS | GS | PS,
        AllCompute     = CS,
        AllRayTracing  = RG | AH | CH | MI | IS,

        All = AllGraphics | AllCompute | AllRayTracing
    };
}