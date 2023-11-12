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
        using super = core::Resource;

        const char *                        getClassName            () const override { return "TextureResource"; }
        static bool                         registerClass           (core::IFactory & _factory);
        static bool                         registerProperties      (core::IClassDesc & _desc);

                                            TextureResource         (const core::string & _name = "", core::IObject * _parent = nullptr);
                                            ~TextureResource        ();

        const core::vector<core::string>    getExtensions           () const final;
        void                                onResourcePathChanged   (const core::string & _oldPath, const core::string & _newPath) final;

        bool                                cook                    (const core::string & _file) const final override;
        core::IObject *                     load                    (const core::string & _file) final override;

        gfx::ITexture *                     getTexture              () const { return (gfx::ITexture*)m_object; }
    };
}