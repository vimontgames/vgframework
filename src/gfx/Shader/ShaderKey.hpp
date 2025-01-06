#include "ShaderKey.h"
#include "gfx/Shader/ShaderManager.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    ShaderKey::ShaderKey()
    {
        clear();
    }

    //--------------------------------------------------------------------------------------
    void ShaderKey::clear()
    {
        file = ShaderKey::File(-1);
        vs = ShaderKey::VS(-1);
        hs = ShaderKey::HS(-1);
        ds = ShaderKey::DS(-1);
        gs = ShaderKey::GS(-1);
        ps = ShaderKey::PS(-1);
        flags = ShaderKey::Flags(0x0);
    }

    //--------------------------------------------------------------------------------------
    ShaderKey::ShaderKey(const core::string & _file, const core::string & _technique)
    {
        clear();
        init(_file, _technique);
    }

    //--------------------------------------------------------------------------------------
    ShaderKey::~ShaderKey()
    {

    }

    //--------------------------------------------------------------------------------------
    void ShaderKey::init(const core::string & _file, const core::string & _technique)
    {
        ShaderManager::get()->initShaderKey(*this, _file, _technique);
    }

    //--------------------------------------------------------------------------------------
    #if VG_VERIFY_SHADERKEY_FLAGS
    void ShaderKey::verifyShaderKeyFlags(core::uint _index, core::uint _value)
    {
        const HLSLDesc * desc = ShaderManager::get()->getHLSLDescriptor(file);
        VG_ASSERT(nullptr != desc, "Could not get HLSL descriptor for shader file %u", file);
        if (desc)
        {
            VG_ASSERT(desc->isValidFlagIndex(_index), "%u is not a valid bit index for shader \"%s\"", _index, desc->getFile().c_str());
            VG_ASSERT(desc->isValidFlagValue(_index, _value), "%u is not a valid value at index %u for shader \"%s\"", _value, _index, desc->getFile().c_str());
        }
    }
    #endif

    //--------------------------------------------------------------------------------------
    void ShaderKey::setFlags(core::uint _index, core::uint _value)
    {
        #if VG_VERIFY_SHADERKEY_FLAGS
        verifyShaderKeyFlags(_index, _value);
        #endif

        const HLSLDesc * desc = ShaderManager::get()->getHLSLDescriptor(file);
        VG_ASSERT(nullptr != desc, "Could not get HLSL descriptor for shader file %u", file);
        if (desc)
        {
            const uint mask = desc->getBitMask(_index);
            flags = (flags & ~(mask << _index)) | _value << _index;
        }
    }

}