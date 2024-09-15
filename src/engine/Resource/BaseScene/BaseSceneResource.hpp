#include "BaseSceneResource.h"
#include "core/File/File.h"
#include "core/IBaseScene.h"

using namespace vg::core;

namespace vg::engine
{
    //VG_REGISTER_RESOURCE_CLASS(ObjectHierarchyResource, "ObjectHierarchy");

    //--------------------------------------------------------------------------------------
    bool BaseSceneResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        //registerProperty(BaseSceneResource, m_name, "Name");

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