#pragma once

#include "core/Object/Object.h"

namespace vg::graphics::renderer
{
    class IGraphicInstance;
}

namespace vg::core
{
    class IEntity;

    class ISector : public Object
    {
    public:
        ISector(const string & _name, IObject * _parent) :
            Object(_name, _parent)
        {

        }

        virtual void            addChildSector          (ISector * _sector) = 0;
        virtual uint            getChildSectorCount     () const = 0;
        virtual const ISector * getChildSector          (uint _index) = 0;

        virtual void            addEntity               (IEntity * _entity) = 0;
        virtual uint            getEntityCount          () const = 0;
        virtual const IEntity * getEntity               (uint _index) const = 0;

        virtual void            addGraphicInstance      (graphics::renderer::IGraphicInstance * _graphicInstance) = 0;
        virtual void            removeGraphicInstance   (graphics::renderer::IGraphicInstance * _graphicInstance) = 0;
    };
}