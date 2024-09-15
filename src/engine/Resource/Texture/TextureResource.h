#pragma once

#include "core/Resource/Resource.h"

namespace vg::gfx
{
    class ITexture;
}

namespace vg::engine
{
    class TextureResource : public core::Resource
    {
    public:
        VG_CLASS_DECL(TextureResource, core::Resource);
        using super = core::Resource;

                                            TextureResource         (const core::string & _name = "", core::IObject * _parent = nullptr);
                                            ~TextureResource        ();

        const core::vector<core::string>    GetExtensions           () const final;

        bool                                Cook                    (const core::string & _file) const final override;
        core::IObject *                     Load                    (const core::string & _file) final override;

        core::IResourceMeta *               CreateResourceMeta      (const core::string & _path) const final override;

    private:
        gfx::ITexture *                     getTexture              () const { return (gfx::ITexture*)m_object; }
    };
}