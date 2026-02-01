#include "ResourceMeta.h"

namespace vg::core
{
    VG_REGISTER_CLASS(ResourceMeta, "Resource Metadata");

    //--------------------------------------------------------------------------------------
    bool ResourceMeta::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(ResourceMeta, m_uid, PropertyFlags::Hidden | PropertyFlags::Transient, true);
        setPropertyFlag(ResourceMeta, m_originalUID, PropertyFlags::Hidden | PropertyFlags::Transient, true);
        setPropertyFlag(ResourceMeta, m_objectFlags, PropertyFlags::Hidden | PropertyFlags::Transient, true);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool ResourceMeta::Load(const core::string & _resourcePath)
    {
        SetName(io::getMetaPath(io::getFileName(_resourcePath)));
        SetFile(io::getMetaPath(_resourcePath));

        if (const auto * factory = Kernel::getFactory())
            return factory->LoadFromXML(this, GetFile());

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ResourceMeta::Save(const core::string & _resourcePath)
    {
        SetName(io::getMetaPath(io::getFileName(_resourcePath)));
        SetFile(io::getMetaPath(_resourcePath));

        if (const auto * factory = Kernel::getFactory())
            return factory->SaveToXML(this, GetFile());

        return false;
    }

    //--------------------------------------------------------------------------------------
    void ResourceMeta::OnPropertyChanged(IObject * _object, const IProperty & _prop, bool _notifyParent)
    {
        // Save .meta when any property changes
        if (const auto * factory = Kernel::getFactory())
            factory->SaveToXML(this, GetFile());

        // And reimport resources that depend on it
        auto rm = Kernel::getResourceManager();
        rm->UpdateResources();
    }

    //--------------------------------------------------------------------------------------
    io::FileAccessTime ResourceMeta::GetLastWriteTime() const
    {
        return m_lastWriteTime;
    }

    //--------------------------------------------------------------------------------------
    void ResourceMeta::SetLastWriteTime(io::FileAccessTime _lastWriteTime)
    {
        m_lastWriteTime = _lastWriteTime;
    }    
}