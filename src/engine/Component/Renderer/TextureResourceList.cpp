#include "engine/Precomp.h"
#include "TextureResourceList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_CLASS(TextureResourceList, "Texture Resource List");

    //--------------------------------------------------------------------------------------
    bool TextureResourceList::registerProperties(vg::core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        return true;
    }

    //--------------------------------------------------------------------------------------
    TextureResourceList::TextureResourceList(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    TextureResourceList::~TextureResourceList()
    {

    }
}