#pragma once

#include "core/Resource/Resource.h"

namespace vg::gfx
{
    class IAnimation;
}

namespace vg::engine
{
    class AnimationResource : public core::Resource
    {
    public:
        VG_CLASS_DECL(AnimationResource, core::Resource)

        AnimationResource(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~AnimationResource();

        const core::vector<core::string>    getExtensions() const final;
        void                                onResourcePathChanged(const core::string & _oldPath, const core::string & _newPath) final;

        bool                                cook(const core::string & _file) const final override;
        core::IObject *                     load(const core::string & _file) final override;

        gfx::IAnimation *                   getAnimation() const { return (gfx::IAnimation *)m_object; }

    private:
        bool                                m_playing = false;
        float                               m_time = 0.0f;
        float                               m_weight = 0.0f;
    };
}