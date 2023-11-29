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

        static IAnimation *                 createFromImporterData  (const AnimImporterData & _data);

        VG_INLINE const AnimImporterData &  getAnimationData        () const;

    private:
        AnimImporterData                    m_animData;    
    };
}

#if VG_ENABLE_INLINE
#include "SkeletalAnimation.inl"
#endif