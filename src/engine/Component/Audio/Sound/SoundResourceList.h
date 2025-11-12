#pragma once

#include "core/Resource/ResourceList.h"
#include "engine/Resource/Audio/SoundResource.h"

namespace vg::engine
{
    class SoundResourceList final : public core::ResourceList<SoundResource>
    {
    public:
        VG_CLASS_DECL(SoundResourceList, ResourceList);

        SoundResourceList(const core::string & _name, core::IObject * _parent);
        ~SoundResourceList();
    };
}