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

        registerProperty(BaseSceneResource, m_name, "Name");

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
        ResourceManager::get()->unloadResource(this, GetResourcePath());
    }

    //--------------------------------------------------------------------------------------
    void BaseSceneResource::onResourcePathChanged(const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _oldPath, _newPath);
    }

    //--------------------------------------------------------------------------------------
    bool BaseSceneResource::cook(const string & _file) const
    {
        // Cooked file is same format as source file for now (TODO : serializeFrom/ToBinary)
        const string cookedPath = io::getCookedPath(_file);

        string data;
        if (io::readFile(_file, data))
        {
            if (io::writeFile(cookedPath, data))
            {
                return true;
            }
        }

        return false;
    }
}