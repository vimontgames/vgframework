#pragma once

namespace vg::renderer
{
    vg_enum_class(PBRFlags, core::u32,
        GenerateIrradianceCubemap         = 0x00000001,
        GenerateSpecularReflectionCubemap = 0x00000002,
        GenerateSpecularBRDF              = 0x00000004
    );
}