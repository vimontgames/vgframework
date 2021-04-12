#pragma once

#include "core/ISector.h"

namespace vg::core
{
    class Entity;

    class Sector : public ISector
    {
    public:
        using super = ISector;

        Sector(const string & _name, IObject * _parent);
        ~Sector();

        const char *                                            getClassName            () const final { return "Sector"; }

        static bool                                             registerClass           (IObjectFactory & _factory);
        static bool                                             registerProperties      (IObjectDescriptor & _desc);

        void                                                    addChildSector          (ISector * _sector) override;
        uint                                                    getChildSectorCount     () const override;
        const ISector *                                         getChildSector          (uint _index) override;

        void                                                    addEntity               (IEntity * _entity) override;
        uint                                                    getEntityCount          () const override;
        const IEntity *                                         getEntity               (uint _index) const override;

        const vector<Sector*>                                   getChildSectors         () const { return m_sectors; }
        const vector<Entity*>                                   getEntities             () const { return m_entities; }

        void                                                    addGraphicInstance      (graphics::renderer::IGraphicInstance * _graphicInstance) final;
        void                                                    removeGraphicInstance   (graphics::renderer::IGraphicInstance * _graphicInstance) final;
        const vector<graphics::renderer::IGraphicInstance*> &   getGraphicInstances     () const final;

    private:
        vector<Sector*>                                         m_sectors;
        vector<Entity*>                                         m_entities;
        vector<graphics::renderer::IGraphicInstance*>           m_graphicInstances;
    };
}