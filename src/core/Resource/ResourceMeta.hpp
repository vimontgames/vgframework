#include "ResourceMeta.h"

namespace vg::core
{
    //--------------------------------------------------------------------------------------
    bool ResourceMeta::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        setPropertyFlag(ResourceMeta, m_uid, PropertyFlags::NotVisible | PropertyFlags::NotSaved, true);
        setPropertyFlag(ResourceMeta, m_originalUID, PropertyFlags::NotVisible | PropertyFlags::NotSaved, true);
        setPropertyFlag(ResourceMeta, m_objectFlags, PropertyFlags::NotVisible | PropertyFlags::NotSaved, true);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool ResourceMeta::Load(const core::string & _resourcePath)
    {
        setName(io::getMetaPath(io::getFileName(_resourcePath)));
        setFile(io::getMetaPath(_resourcePath));

        if (const auto * factory = Kernel::getFactory())
            return factory->LoadFromXML(this, getFile());

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ResourceMeta::Save(const core::string & _resourcePath)
    {
        setName(io::getMetaPath(io::getFileName(_resourcePath)));
        setFile(io::getMetaPath(_resourcePath));

        if (const auto * factory = Kernel::getFactory())
            return factory->SaveToXML(this, getFile());

        return false;
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