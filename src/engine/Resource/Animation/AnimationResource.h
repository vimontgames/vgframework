#pragma once

#include "core/Resource/Resource.h"

namespace vg::renderer
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

        const core::vector<core::string>        getExtensions           () const final;
        void                                    onResourcePathChanged   (const core::string & _oldPath, const core::string & _newPath) final;

        bool                                    cook                    (const core::string & _file) const final override;
        core::IObject *                         load                    (const core::string & _file) final override;

        void                                    onPropertyChanged       (core::IObject * _object, const core::IProperty & _prop) override;

        VG_INLINE const renderer::IAnimation *  getAnimation            () const { return (renderer::IAnimation *)m_object; }
        VG_INLINE renderer::IAnimation *        getAnimation            () { return (renderer::IAnimation *)m_object; }

        VG_INLINE bool                          isPlaying               () const { return m_play; }
        VG_INLINE float                         getTime                 () const { return m_time; }
        VG_INLINE float                         getSpeed                () const { return m_speed; }
        //VG_INLINE float                         getWeight               () const { return m_weight; }

        void                                    setTime                 (float _time);

    private:
        bool                                    m_play = false;
        float                                   m_time = 0.0f;
        float                                   m_speed = 1.0f;
        //float                                   m_weight = 1.0f;
    };
}