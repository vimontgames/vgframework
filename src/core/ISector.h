#pragma once

#include "core/Object/Object.h"

namespace vg::core
{
    class IEntity;

    class ISector : public Object
    {
    public:
        virtual void            addChildSector      (ISector * _sector) = 0;
        virtual uint            getChildSectorCount () const = 0;
        virtual const ISector * getChildSector      (uint _index) = 0;

        virtual void            addEntity           (IEntity * _entity) = 0;
        virtual uint            getEntityCount      () const = 0;
        virtual const IEntity * getEntity           (uint _index) const = 0;
    };
}