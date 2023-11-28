#pragma once

#include "renderer/Instance/GraphicInstance.h"

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
        virtual bool ShowSkeleton   () const = 0;

        virtual bool AddAnimation(ISkeletalAnimation * _animation) = 0;
        virtual bool RemoveAnimation(ISkeletalAnimation * _animation) = 0;
    };
}