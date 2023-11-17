#include "MaterialResource.h"
#include "core/File/File.h"
#include "renderer/IMeshModel.h"
#include "renderer/IMaterialModel.h"
#include "engine/Component/Mesh/MaterialResourceList.h"

#include "MaterialResourceData.hpp"

using namespace vg::core;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(MaterialResource);

    //--------------------------------------------------------------------------------------
    bool MaterialResource::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(MaterialResource, "Material Resource", IClassDesc::Flags::Resource))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void * ResizeMaterialResourceVector(IObject * _parent, uint _offset, uint _count, uint & _elementSize)
    {
        auto vec = (core::vector<MaterialResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        _elementSize = sizeof(MaterialResource);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        
        _desc.registerResizeVectorFunc(MaterialResource, ResizeMaterialResourceVector);
        
        return true;
    }   

    //--------------------------------------------------------------------------------------
    MaterialResource::MaterialResource(const core::string & _name, IObject * _parent) :
        Resource(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    MaterialResource::~MaterialResource()
    {
        ResourceManager::get()->unloadResource(this, GetResourcePath());
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> MaterialResource::getExtensions() const
    {
        vector<string> ext;
        ext.push_back(".mat");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void MaterialResource::onResourcePathChanged(const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _oldPath, _newPath);
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::cook(const string & _file) const
    {
        // Cooked file is same format as source file for now
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

    //--------------------------------------------------------------------------------------
    core::IObject * MaterialResource::load(const string & _path)
    {
        IFactory * factory = Kernel::getFactory();
        IObject * object = factory->createObject("MaterialResourceData");
        object->setParent(this);

        if (object)
        {
            factory->loadFromXML(object, _path);
            return object;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    // a .mat file is an XML file that serializes a MaterialResourceData Object
    //--------------------------------------------------------------------------------------
    bool MaterialResource::CreateFile(const core::string & _path)
    {
        const auto * factory = Kernel::getFactory();
        
        IObject * resData = factory->createObject("MaterialResourceData");
        if (nullptr != resData)
        {
            factory->saveToXML(resData, _path);
            VG_SAFE_RELEASE(resData);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::SaveFile(const string & _path) const
    {
        IObject * object = getObject();
        VG_ASSERT(!object || dynamic_cast<MaterialResourceData *>(object));
        if (nullptr != object)
        {
            const auto * factory = Kernel::getFactory();
            return factory->saveToXML(object, _path);
        }
        return false;
    }
}