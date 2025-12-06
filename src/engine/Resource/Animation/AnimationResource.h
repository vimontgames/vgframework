#pragma once

#include "engine/IAnimationResource.h"

namespace vg::renderer
{
    class IAnimation;
    class IMeshInstance;
}

namespace vg::renderer
{
    enum class BodyPartFlags : core::u32;
}

namespace vg::engine
{
    class AnimationResource final : public IAnimationResource
    {
    public:
        VG_CLASS_DECL(AnimationResource, IAnimationResource)

        AnimationResource(const core::string & _name = "", core::IObject * _parent = nullptr);
        ~AnimationResource();

        const core::vector<core::string>        GetExtensions           () const final;

        bool                                    Cook                    (const core::string & _file) const final override;
        core::IObject *                         Load                    (const core::string & _file) final override;

        void                                    OnLoad                  () override;
        void                                    OnPropertyChanged       (core::IObject * _object, const core::IProperty & _prop, bool _notifyParent) override;

        void                                    OnEnable                () final override;
        void                                    OnDisable               () final override;

        void                                    OnStop                  () override;

        bool                                    IsPlaying               () const final override;
        bool                                    IsFinished              () const final override;

        bool                                    PlayLoop                () final override;
        bool                                    PlayOnce                () final override;

        bool                                    Stop                    () final override;

        float                                   GetTime                 () const final override;
        float                                   GetLength               () const final override;
        float                                   GetWeight               () const final override;

        void                                    setTime                 (float _time);
        void                                    setWeight               (float _weight);
        void                                    setLayer                (core::uint _layer);
        void                                    setBodyParts            (renderer::BodyPartFlags _flags);

        renderer::IMeshInstance *               getMeshInstance         ();

        VG_INLINE const renderer::IAnimation *  getAnimation            () const;
        VG_INLINE renderer::IAnimation *        getAnimation            () ;

        VG_INLINE bool                          isPlaying               () const;
        VG_INLINE float                         getTime                 () const;
        VG_INLINE float                         getWeight               () const;
        VG_INLINE float                         getSpeed                () const;

        VG_INLINE void                          setPlay                 (bool _play);
        VG_INLINE void                          setLoop                 (bool _loop);
        VG_INLINE core::uint                    getLayer                () const;
        VG_INLINE renderer::BodyPartFlags       getBodyParts            () const;

    private:
        void                                    reset                   ();

        static bool			                    playAnim                (IObject * _object);
        static bool			                    stopAnim                (IObject * _object);

    private:
        bool                                    m_play          = false;
        bool                                    m_loop          = false;
        float                                   m_time          = 0.0f;
        float                                   m_weight        = 0.0f;
        float                                   m_speed         = 1.0f;
        core::uint                              m_layer         = 0;
        bool                                    m_useBodyParts  = false;
        renderer::BodyPartFlags                 m_bodyParts     = (renderer::BodyPartFlags)0x0;
    };
}

#if VG_ENABLE_INLINE
#include "AnimationResource.inl"
#endif