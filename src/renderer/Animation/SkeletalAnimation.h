#pragma once

#include "renderer/ISkeletalAnimation.h"
#include "renderer/Importer/AnimImporterData.h"

namespace vg::renderer
{
    class SkeletalAnimation : public ISkeletalAnimation
    {
    public:
        VG_CLASS_DECL(SkeletalAnimation, ISkeletalAnimation)

                                            SkeletalAnimation       (const core::string & _name, core::IObject * _parent);
                                            ~SkeletalAnimation      ();

        float                               GetLength               () const final override;
        float                               GetFramerate            () const final override;

        bool                                SetTime                 (float _time) final override;
        bool                                SetWeight               (float _weight) final override;

        static IAnimation *                 createFromImporterData  (const AnimImporterData & _data);

        VG_INLINE float                     getTime                 () const;
        VG_INLINE float                     getWeight               () const;
        VG_INLINE const AnimImporterData &  getAnimationData        () const;

    private:
        float                               m_time = 0.0f;
        float                               m_weight = 0.0f;
        AnimImporterData                    m_animData;    
    };
}

#if VG_ENABLE_INLINE
#include "SkeletalAnimation.inl"
#endif