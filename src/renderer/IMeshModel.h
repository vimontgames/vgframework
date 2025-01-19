#pragma once

#include "IGraphicModel.h"
#include "renderer/Importer/CollisionImporterData.h"

namespace vg::renderer
{
    class IMeshModel : public IGraphicModel
    {
    public:
        IMeshModel(const core::string & _name, core::IObject * _parent) :
            IGraphicModel(_name, _parent)
        {

        }

        virtual const core::vector<ColliderTriangle> & GetColliderTriangles() const = 0;
    };
}