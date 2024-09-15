#pragma once

#include "engine/IAnimationResource.h"

namespace vg::renderer
{
    class IAnimation;
    class IMeshInstance;
}

namespace vg::engine
{
    class AnimationResource : public IAnimationResource
    {
    public:
        VG_CLASS_DECL(AnimationResource, IAnimationResource)

        AnimationResource(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~AnimationResource();

        const core::vector<core::string>        GetExtensions           () const final;

        bool                                    Cook                    (const core::string & _file) const final override;
        core::IObject *                         Load                    (const core::string & _file) final override;

        void                                    OnPropertyChanged       (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

        void                                    OnEnable                () final override;
        void                                    OnDisable               () final override;

        void                                    OnStop                  () override;

        bool                                    IsPlaying               () const final override;
        bool                                    IsFinished              () const final override;

        bool                                    PlayLoop                () final override;
        bool                                    PlayOnce                () final override;

        bool                                    Stop                    () final override;
        float                                   GetWeight               () const final override;

        VG_INLINE const renderer::IAnimation *  getAnimation            () const { return (renderer::IAnimation *)m_object; }
        VG_INLINE renderer::IAnimation *        getAnimation            () { return (renderer::IAnimation *)m_object; }

        VG_INLINE bool                          isPlaying               () const { return m_play; }
        VG_INLINE float                         getTime                 () const { return m_time; }
        VG_INLINE float                         getWeight               () const { return m_weight; }
        VG_INLINE float                         getSpeed                () const { return m_speed; }

        void                                    setTime                 (float _time);
        void                                    setWeight               (float _weight);

        renderer::IMeshInstance *               getMeshInstance         ();
        VG_INLINE void                          setPlay                 (bool _play) { m_play = _play;; }
        VG_INLINE void                          setLoop                 (bool _loop) { m_loop = _loop; }

    private:
        void                                    reset                   ();

        static bool			                    playAnim                (IObject * _object);
        static bool			                    stopAnim                (IObject * _object);

    private:
        bool                                    m_play      = false;
        bool                                    m_loop      = false;
        float                                   m_time      = 0.0f;
        float                                   m_weight    = 0.0f;
        float                                   m_speed     = 1.0f;

    };
}