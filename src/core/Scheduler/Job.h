#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class Job : public Object
    {
        public:
            Job(const string & _name);
            virtual ~Job();

            virtual void run() = 0;
    };
}