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
        void                                onResourcePathChanged   (IObject * _owner, const core::string & _oldPath, const core::string & _newPath) final;

        bool                                cook                    (const core::string & _file) final override;
        bool                                load                    (const core::string & _file, core::IObject * _owner) final override;

        gfx::ITexture *        getTexture              () const { return (gfx::ITexture*)m_object; }

        core::uint                          getSubResourceCount     () const final { return 0;}
        IResource *                         getSubResource          (core::uint _index) final { return nullptr;}
    };
}