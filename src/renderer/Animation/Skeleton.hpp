#include "Skeleton.h"

//#if !VG_ENABLE_INLINE
//#include "Skeleton.inl"
//#endif

using namespace vg::core;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS(Skeleton, "Skeleton");

    //--------------------------------------------------------------------------------------
    bool Skeleton::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void Skeleton::setNodes(const core::vector<MeshImporterNode> & _nodes)
    {
        m_nodes = _nodes;
    }

    //--------------------------------------------------------------------------------------
    void Skeleton::setBoneIndices(const core::vector<core::u32> & _bonesIndices)
    {
        m_boneIndices = _bonesIndices;
    }

    //--------------------------------------------------------------------------------------
    void Skeleton::setBoneMatrices(const core::vector<core::float4x4> & _bonesMatrices)
    {
        m_boneMatrices = _bonesMatrices;
    }
}