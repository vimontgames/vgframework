#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class Job : public ObjectPointer
    {
        public:
            Job(const string & _name, IObject * _parent);
            virtual ~Job();

            virtual void run() = 0;
    };
}