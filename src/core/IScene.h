#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class ISector;

    class IScene : public ObjectPointer
    {
    public:
        virtual void        setRoot (ISector * _sector) = 0;
        virtual ISector *   getRoot () const  = 0;
    };
}