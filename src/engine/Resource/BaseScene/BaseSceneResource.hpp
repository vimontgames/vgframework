#include "BaseSceneResource.h"
#include "core/File/File.h"
#include "core/IBaseScene.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    bool BaseSceneResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    BaseSceneResource::BaseSceneResource(const core::string & _name, IObject * _parent) :
        Resource(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    BaseSceneResource::~BaseSceneResource()
    {
       
    }

    //--------------------------------------------------------------------------------------
    bool BaseSceneResource::Cook(const string & _file) const
    {
        return super::Cook(_file);
    }
}