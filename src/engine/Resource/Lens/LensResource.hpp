#include "LensResource.h"
#include "core/File/File.h"
#include "renderer/ICameraLens.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(LensResource, "Lens", 1);

    //--------------------------------------------------------------------------------------
    void * ResizeLensResourceVector(IObject * _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<LensResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool LensResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(LensResource, m_name, PropertyFlags::NotVisible, true);
        registerResizeVectorFunc(LensResource, ResizeMaterialResourceVector);

        registerPropertyObjectPtr(LensResource, m_cameraLens, "Lens");

        return true;
    }

    //--------------------------------------------------------------------------------------
    LensResource::LensResource(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    LensResource::~LensResource()
    {

    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> LensResource::GetExtensions() const
    {
        vector<string> ext;
        ext.push_back(".lens");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    core::IObject * LensResource::Load(const string & _path)
    {
        IFactory * factory = Kernel::getFactory();
        auto * object = (renderer::ICameraLens *)factory->CreateObject("CameraLens");
        if (nullptr != object)
        {
            object->SetParent(this);
            object->SetName(io::getFileName(_path));

            if (factory->LoadFromXML(object, _path))
                return object;
        }

        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    // a .mat file is an XML file that serializes a LensResource Object
    //--------------------------------------------------------------------------------------
    bool LensResource::CreateFile(const core::string & _path, core::IObject * _data)
    {
        const auto * factory = Kernel::getFactory();

        auto * resData = (renderer::ICameraLens *)factory->CreateObject("CameraLens");
        if (nullptr != resData)
        {
            factory->SaveToXML(resData, _path);
            VG_SAFE_RELEASE(resData);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool LensResource::SaveFile(const string & _path) const
    {
        IObject * object = GetObject();
        if (nullptr != object)
        {
            const auto * factory = Kernel::getFactory();
            return factory->SaveToXML(object, _path);
        }
        return false;
    }
}