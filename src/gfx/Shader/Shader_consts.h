#pragma once

// When enabled, the shader source files will be stored in memory and no filesystem access will be needed to compile. It makes it possible to "patch" shader source 
#define VG_SHADER_SOURCE_IN_MEMORY 1

vg_enum_class_ns(vg::gfx, ShaderStage, core::u8,
    Vertex = 0,
    Hull,
    Domain,
    Geometry,
    Pixel,
    Compute
);

vg_enum_class_ns(vg::gfx, ShaderStageFlags, core::u8,
    VS = 1 << core::asInteger(ShaderStage::Vertex),
    HS = 1 << core::asInteger(ShaderStage::Domain),
    DS = 1 << core::asInteger(ShaderStage::Hull),
    GS = 1 << core::asInteger(ShaderStage::Geometry),
    PS = 1 << core::asInteger(ShaderStage::Pixel),
    CS = 1 << core::asInteger(ShaderStage::Compute),

    All = 0x3F // VS | HS | DS | GS | PS | CS
);
