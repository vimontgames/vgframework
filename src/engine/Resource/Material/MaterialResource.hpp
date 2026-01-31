#include "MaterialResource.h"
#include "core/File/File.h"
#include "renderer/IMeshModel.h"
#include "renderer/IMaterialModel.h"
#include "engine/Component/Renderer/MaterialResourceList.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(MaterialResource, "Material", ResourcePriority::Material);

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
        super(_name, _parent)
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

            // TODO: when different material types are supported, this should check the actual MaterialData type to create the right one
            MaterialResourceData *& materialResourceData = ((MaterialResourceData *&)m_instance);

            materialResourceData->SetParent(this);
            materialResourceData->m_data = new DefaultMaterialData("Default (Instance)", materialResourceData);
            materialResourceData->m_data->RegisterUID();
            materialResourceData->CreateRendererMaterial();

            factory->CopyProperties(m_shared, materialResourceData);

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

    //--------------------------------------------------------------------------------------
    MaterialData * MaterialResource::getMaterialData() const
    {
        MaterialResourceData * matResData = VG_SAFE_STATIC_CAST(MaterialResourceData, GetObject());
        if (matResData)
            return matResData->getMaterialData();
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    MatPropIndex MaterialResource::GetPropertyIndex(const core::string & _propertyName) const
    {
        MaterialData * matData = getMaterialData();

        if (matData)
            return matData->GetClassDesc()->GetPropertyIndex(_propertyName.c_str());
        else
            return -1;
    }

    //--------------------------------------------------------------------------------------
    void MaterialResource::SetProperty(MatPropIndex _propertyIndex, float _value)
    {
        MaterialData * matData = getMaterialData();

        if (const IProperty * prop = matData->GetClassDesc()->GetPropertyByIndex(_propertyIndex))
        {
            // set property
            float * value = prop->GetPropertyFloat(matData);
            *value = _value;

            // copy to renderer
            if (renderer::IMaterialModel * matModel = matData->getMaterialModel())
                matModel->SetFloat(prop->GetName(), _value); // TODO: use index/handle instead of string!
        }
    }
}