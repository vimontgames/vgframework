#include "core/Precomp.h"
/*
#include "Sector.h"
#include "core/Entity/Entity.h"
#include "core/Object/AutoRegisterClass.h"
#include "graphics/renderer/IGraphicInstance.h"

namespace vg::core
{
    VG_AUTO_REGISTER_CLASS(Sector);

    //--------------------------------------------------------------------------------------
    Sector::Sector(const string & _name, IObject * _parent) :
        ISector(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Sector::~Sector()
    {
        for (uint i = 0; i < m_sectors.size(); ++i)
            VG_SAFE_RELEASE(m_sectors[i]);
        m_sectors.clear();

        for (uint i = 0; i < m_entities.size(); ++i)
            VG_SAFE_RELEASE(m_entities[i]);
        m_entities.clear();

        for (uint i = 0; i < m_graphicInstances.size(); ++i)
            VG_SAFE_RELEASE(m_graphicInstances[i]);
        m_graphicInstances.clear();
    }

    //--------------------------------------------------------------------------------------
    bool Sector::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(Sector, "Sector", IClassDesc::Flags::SceneNode))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool Sector::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);
        
        _desc.registerPropertyObjectPointerVectorHelper(Sector, m_entities, "Entities", IProperty::Flags::None);
        _desc.registerPropertyObjectPointerVectorHelper(Sector, m_sectors, "Sectors", IProperty::Flags::None);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Sector::addChildSector(ISector * _sector)
    {
        VG_ASSERT(m_sectors.end() == std::find(m_sectors.begin(), m_sectors.end(), _sector));
        VG_SAFE_INCREASE_REFCOUNT(_sector);
        m_sectors.push_back((Sector*)_sector);
    }

    //--------------------------------------------------------------------------------------
    uint Sector::getChildSectorCount() const
    {
        return (uint)m_sectors.size();
    }

    //--------------------------------------------------------------------------------------
    const ISector * Sector::getChildSector(uint _index) const
    {
        VG_ASSERT(_index < getChildSectorCount());
        return m_sectors[_index];
    }

    //--------------------------------------------------------------------------------------
    uint Sector::getEntityCount() const
    {
        return (uint)m_entities.size();
    }

    //--------------------------------------------------------------------------------------
    void Sector::addEntity(IEntity * _entity)
    {
        VG_ASSERT(m_entities.end() == std::find(m_entities.begin(), m_entities.end(), _entity));
        _entity->addRef();
        m_entities.push_back((Entity*)_entity);
    }

    //--------------------------------------------------------------------------------------
    const IEntity * Sector::getEntity(uint _index) const
    {
        VG_ASSERT(_index < getEntityCount());
        return static_cast<IEntity*>(m_entities[_index]);
    }

    //--------------------------------------------------------------------------------------
    void Sector::addGraphicInstance(graphics::renderer::IGraphicInstance * _graphicInstance)
    {
        VG_ASSERT(m_graphicInstances.end() == std::find(m_graphicInstances.begin(), m_graphicInstances.end(), _graphicInstance));
        _graphicInstance->addRef();
        m_graphicInstances.push_back(_graphicInstance);
    }

    //--------------------------------------------------------------------------------------
    void Sector::removeGraphicInstance(graphics::renderer::IGraphicInstance * _graphicInstance)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
    }

    //--------------------------------------------------------------------------------------
    const vector<graphics::renderer::IGraphicInstance*> & Sector::getGraphicInstances() const
    {
        return m_graphicInstances;
    }
}
*/