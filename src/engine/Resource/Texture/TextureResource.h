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

        const char *                        GetClassName            () const override { return "TextureResource"; }
        static bool                         registerClass           (core::IFactory & _factory);
        static bool                         registerProperties      (core::IClassDesc & _desc);

                                            TextureResource         (const core::string & _name = "", core::IObject * _parent = nullptr);
                                            ~TextureResource        ();

        const core::vector<core::string>    GetExtensions           () const final;
        void                                OnResourcePathChanged   (const core::string & _oldPath, const core::string & _newPath) final;

        bool                                Cook                    (const core::string & _file) const final override;
        core::IObject *                     Load                    (const core::string & _file) final override;

        gfx::ITexture *                     getTexture              () const { return (gfx::ITexture*)m_object; }
    };
}