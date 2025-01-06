#include "ComputeShaderKey.h"
#include "gfx/Shader/ShaderManager.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    ComputeShaderKey::ComputeShaderKey()
    {
        clear();
    }

    //--------------------------------------------------------------------------------------
    void ComputeShaderKey::clear()
    {
        file = ComputeShaderKey::File(-1);
        cs = ComputeShaderKey::CS(-1);
        flags = ComputeShaderKey::File(0x0000);
    }

    //--------------------------------------------------------------------------------------
    ComputeShaderKey::ComputeShaderKey(const core::string & _file, const core::string & _technique)
    {
        clear();
        init(_file, _technique);
    }

    //--------------------------------------------------------------------------------------
    ComputeShaderKey::~ComputeShaderKey()
    {

    }

    //--------------------------------------------------------------------------------------
    void ComputeShaderKey::init(const core::string & _file, const core::string & _technique)
    {
        ShaderManager::get()->initComputeShaderKey(*this, _file, _technique);
    }

    //--------------------------------------------------------------------------------------
    #if VG_VERIFY_COMPUTESHADERKEY_FLAGS
    void ComputeShaderKey::verifyComputeShaderKeyFlags(core::uint _index, core::uint _value)
    {
        const HLSLDesc * desc = ShaderManager::get()->getHLSLDescriptor(file);
        VG_ASSERT(nullptr != desc, "Could not get HLSL descriptor for shader file %u", file);
        if (desc)
        {
            VG_ASSERT(desc->isValidFlagIndex(_index), "%u is not a valid index for shader \"%s\"", _index, desc->getFile().c_str());
            VG_ASSERT(desc->isValidFlagValue(_index, _value), "%u is not a valid value at index %u for shader \"%s\"", _value, _index, desc->getFile().c_str());
        }
    }
    #endif

    //--------------------------------------------------------------------------------------
    void ComputeShaderKey::setFlags(core::uint _index, core::uint _value)
    {
        #if VG_VERIFY_COMPUTESHADERKEY_FLAGS
        verifyComputeShaderKeyFlags(_index, _value);
        #endif

        const HLSLDesc * desc = ShaderManager::get()->getHLSLDescriptor(file);
        VG_ASSERT(nullptr != desc, "Could not get HLSL descriptor for shader file %u", file);
        if (desc)
        {
            const uint mask = desc->getBitMask(_index);
            flags = (flags & ~(mask<<_index)) | _value<<_index;
        }
    }
}