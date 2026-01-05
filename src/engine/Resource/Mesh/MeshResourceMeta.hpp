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
        setPropertyDescription(MeshResourceMeta, m_importSettings.m_physicsCollisionData, "Generate a CPU version of the mesh that will be used to generate a collision mesh for physics");

        registerProperty(MeshResourceMeta, m_importSettings.m_ignoreScale, "Ignore scale");
        setPropertyDescription(MeshResourceMeta, m_importSettings.m_ignoreScale, "Ignore file units when importing the mesh");

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