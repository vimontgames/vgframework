#include "MeshResourceMeta.h"
#include "gfx/Importer/MeshImporterSettings.h"
#include "core/string/string.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_RESOURCE_CLASS(MeshResourceMeta, "MeshResourceMeta", 199);

    //--------------------------------------------------------------------------------------
    bool MeshResourceMeta::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(MeshResourceMeta, m_importSettings.m_physicsCollisionData, "Collision data");

        return true;
    }

    //--------------------------------------------------------------------------------------
    MeshResourceMeta::MeshResourceMeta(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
       
    }

    //--------------------------------------------------------------------------------------
    MeshResourceMeta::~MeshResourceMeta()
    {

    }
}