#include "Skeleton.h"

#if !VG_ENABLE_INLINE
#include "Skeleton.inl"
#endif

using namespace vg::core;

namespace vg::renderer
{
    VG_REGISTER_CLASS(Skeleton, "Skeleton");

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
    const core::string Skeleton::GetNodeName(NodeIndex _index) const
    {
        VG_ASSERT(_index < m_nodes.size());
        return m_nodes[_index].name;
    }

    //--------------------------------------------------------------------------------------
    BodyPartFlags Skeleton::GetNodeFlags(NodeIndex _index) const
    {
        VG_ASSERT(_index < m_nodes.size());
        return m_nodes[_index].flags;
    }

    //--------------------------------------------------------------------------------------
    const core::float4x4 & Skeleton::GetNodeWorldMatrix(NodeIndex _index) const
    {
        VG_ASSERT(_index < m_nodes.size());
        return m_nodes[_index].node_to_world;
    }

    //--------------------------------------------------------------------------------------
    core::i16 Skeleton::GetParentIndex(NodeIndex _index) const
    {
        VG_ASSERT(_index < m_nodes.size());
        return m_nodes[_index].parent_index;
    }

    //--------------------------------------------------------------------------------------
    void Skeleton::SelectNode(NodeIndex _index, bool _selected)
    {
        VG_ASSERT(_index < m_nodes.size());
        if (m_nodes[_index].selected != _selected)
            m_nodes[_index].selected = _selected;
    }

    //--------------------------------------------------------------------------------------
    bool Skeleton::IsNodeSelected(NodeIndex _index) const
    {
        VG_ASSERT(_index < m_nodes.size());
        return m_nodes[_index].selected;
    }

    //--------------------------------------------------------------------------------------
    NodeIndex Skeleton::FindNodeIndex(const core::string & _name) const
    {
        for (uint i = 0; i < m_nodes.size(); ++i)
        {
            const auto & node = m_nodes[i];
            if (node.name == _name)
                return i;
        }
        return -1;
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