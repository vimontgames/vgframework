#include "core/Precomp.h"
#include "Job.h"
#include "Scheduler.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    Job::Job(const string & _name, IObject * _parent) :
        ObjectPointer(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Job::~Job()
    {

    }
}