#include "Skeleton.h"

#if !VG_ENABLE_INLINE
#include "Skeleton.inl"
#endif

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
    uint Skeleton::GetNodeCount() const
    {
        return (uint)m_nodes.size();
    }
    //--------------------------------------------------------------------------------------
    const core::string Skeleton::GetNodeName(uint _index) const
    {
        VG_ASSERT(_index < m_nodes.size());
        return m_nodes[_index].name;
    }

    //--------------------------------------------------------------------------------------
    core::i16 Skeleton::GetParentIndex(core::uint _index) const 
    {
        VG_ASSERT(_index < m_nodes.size());
        return m_nodes[_index].parent_index;
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