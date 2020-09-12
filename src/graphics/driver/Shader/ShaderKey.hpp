#include "ShaderKey.h"
#include "graphics/driver/Shader/ShaderManager.h"

namespace vg::graphics::driver
{
    //--------------------------------------------------------------------------------------
    ShaderKey::ShaderKey()
    {
        clear();
    }

    //--------------------------------------------------------------------------------------
    void ShaderKey::clear()
    {
        vs = ShaderKey::VS(-1);
        hs = ShaderKey::HS(-1);
        ds = ShaderKey::DS(-1);
        gs = ShaderKey::GS(-1);
        ps = ShaderKey::PS(-1);
        flags = ShaderKey::File(0x0000);
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

}