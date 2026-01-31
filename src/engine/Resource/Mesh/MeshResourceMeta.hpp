#include "MeshResourceMeta.h"
#include "gfx/Importer/MeshImporterSettings.h"
#include "core/string/string.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_CLASS(MeshResourceMeta, "MeshResourceMeta");

    //--------------------------------------------------------------------------------------
    bool MeshResourceMeta::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerProperty(MeshResourceMeta, m_importSettings.m_physicsCollisionData, "Collision data");
        setPropertyDescription(MeshResourceMeta, m_importSettings.m_physicsCollisionData, "Generate a CPU version of the mesh that will be used to generate a collision mesh for physics");

        registerProperty(MeshResourceMeta, m_importSettings.m_ignoreScale, "Ignore scale");
        setPropertyDescription(MeshResourceMeta, m_importSettings.m_ignoreScale, "Ignore file units when importing the mesh");

        registerProperty(MeshResourceMeta, m_importSettings.m_rotateYUpAABB, "YUp Bounding Box");
        setPropertyDescription(MeshResourceMeta, m_importSettings.m_rotateYUpAABB, "Rotate YUp bounding box to match world orientation");

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