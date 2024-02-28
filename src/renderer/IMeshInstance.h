#pragma once
#include "renderer/Instance/GraphicInstance.h"

namespace vg::core
{
    class IWorld;
}

namespace vg::renderer
{
    class ISkeletalAnimation;

    class IMeshInstance : public GraphicInstance
    {
    public:
        IMeshInstance(const core::string & _name, core::IObject * _parent) :
            GraphicInstance(_name, _parent)
        {
            
        }
        
        virtual bool UpdateSkeleton () = 0;
        virtual bool DrawSkeleton   (const core::IWorld * _world) const = 0;

        virtual bool AddAnimation(ISkeletalAnimation * _animation) = 0;
        virtual bool RemoveAnimation(ISkeletalAnimation * _animation) = 0;

        virtual bool SetAnimationTime(ISkeletalAnimation * _animation, float _time) = 0;
        virtual bool SetAnimationWeight(ISkeletalAnimation * _animation, float _weight) = 0;

        virtual core::string GetBatchName(core::uint _batchIndex) const = 0;
    };
}