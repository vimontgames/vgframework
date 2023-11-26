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

        float               GetLength               () const final override;
        float               GetFramerate            () const final override;
        bool                SetTime                 (float _time) final override;

        static IAnimation * createFromImporterData  (const AnimImporterData & _data);

        VG_INLINE float     getTime                 () const { return m_time;}

    //private:
        float               m_time = 0.0f;
        AnimImporterData    m_animData;    
    };
}