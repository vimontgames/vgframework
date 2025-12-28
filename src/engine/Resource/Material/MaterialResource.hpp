#include "MaterialResource.h"
#include "core/File/File.h"
#include "renderer/IMeshModel.h"
#include "renderer/IMaterialModel.h"
#include "engine/Component/Renderer/MaterialResourceList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(MaterialResource, "Material", 150);

    //--------------------------------------------------------------------------------------
    void * ResizeMaterialResourceVector(IObject * _parent, uint _offset, uint _count)
    {
        auto vec = (core::vector<MaterialResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(MaterialResource, m_name, PropertyFlags::Hidden, false);
        registerResizeVectorFunc(MaterialResource, ResizeMaterialResourceVector);

        setPropertyFlag(MaterialResource, m_instanciate, PropertyFlags::Transient, false);

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
        UnregisterUID();
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::Instanciate()
    {
        if (super::Instanciate())
        {
            IFactory * factory = Kernel::getFactory();

            ((MaterialResourceData *)m_instance)->SetParent(this);
            ((MaterialResourceData *)m_instance)->m_data = new DefaultMaterialData("Default", m_instance);
            ((MaterialResourceData *)m_instance)->m_data->RegisterUID();
            ((MaterialResourceData *)m_instance)->CreateRendererMaterial();

            factory->CopyProperties(m_shared, m_instance);

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> MaterialResource::GetExtensions() const
    {
        vector<string> ext;
        ext.push_back(".mat");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::Cook(const string & _file) const
    {
        return super::Cook(_file);
    }

    //--------------------------------------------------------------------------------------
    core::LoadStatus MaterialResource::Load(const core::string & _file, core::IObject *& _object)
    {
        IFactory * factory = Kernel::getFactory();
        MaterialResourceData * object = (MaterialResourceData*)factory->CreateObject("MaterialResourceData");
        if (nullptr != object)
        {
            object->SetParent(this);
            object->SetName(io::getFileName(_file));

            if (factory->LoadFromXML(object, _file))
            {
                // Create the material, textures aren't loaded yet
                object->CreateRendererMaterial();
                object->RegisterUID();
                _object = object;
                return LoadStatus::Success;
            }
        }

        VG_SAFE_RELEASE(object);
        _object = nullptr;
        return LoadStatus::CannotCreateObject;
    }

    //--------------------------------------------------------------------------------------
    // a .mat file is an XML file that serializes a MaterialResourceData Object
    //--------------------------------------------------------------------------------------
    bool MaterialResource::CreateFile(const core::string & _path, core::IObject * _data)
    {
        const auto * factory = Kernel::getFactory();

        IObject * resData = factory->CreateObject("MaterialResourceData");
        if (nullptr != resData)
        {
            factory->SaveToXML(resData, _path);
            VG_SAFE_RELEASE(resData);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool MaterialResource::SaveFile(const string & _path) const
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