#pragma once

#include "core/Resource/Resource.h"

namespace vg::graphics::driver
{
    class ITexture;
}

namespace vg::engine
{
    class TextureResource : public core::Resource
    {
    public:
        TextureResource(IObject * _owner = nullptr);
        ~TextureResource();

        const core::vector<core::string>    getExtensions   () const final;
        void                                onPathChanged   (IObject * _owner, const core::string & _oldPath, const core::string & _newPath) final;

        bool                                cook            (const core::string & _file) final override;
        bool                                load            (const core::string & _file, core::IObject * _owner) final override;

        graphics::driver::ITexture *        getTexture      () const { return (graphics::driver::ITexture*)m_object; }
    };
}