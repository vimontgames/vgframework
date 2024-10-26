#pragma once

#include "renderer/ISkeleton.h"
#include "renderer/Importer/MeshImporterData.h"

namespace vg::renderer
{
    class Skeleton final : public ISkeleton
    {
    public:
        VG_CLASS_DECL_PASSTHROUGH(Skeleton, ISkeleton)

        core::uint                                          GetNodeCount        () const final override;
        const core::string                                  GetNodeName         (NodeIndex _index) const final override;
        BodyPartFlags                                       GetNodeFlags        (NodeIndex _index) const final override;
        const core::float4x4 &                              GetNodeWorldMatrix  (NodeIndex _index) const final override;
        NodeIndex                                           GetParentIndex      (NodeIndex _index) const final override;
        void                                                SelectNode          (NodeIndex _index, bool _selected) final override;
        bool                                                IsNodeSelected      (NodeIndex _index) const final override;
        NodeIndex                                           FindNodeIndex       (const core::string & _name) const final override;

        void                                                setScale            (float _scale);
        void                                                setNodes            (const core::vector<MeshImporterNode> & _nodes);
        void                                                setBoneIndices      (const core::vector<core::u32> & _bonesIndices);
        void                                                setBoneMatrices     (const core::vector<core::float4x4> & _bonesMatrices);

        VG_INLINE float                                     getScale            () const;
        VG_INLINE const core::vector<MeshImporterNode> &    getNodes            () const;
        VG_INLINE       core::vector<MeshImporterNode> &    getNodes            ();

        VG_INLINE core::uint                                getBoneCount        () const;

        VG_INLINE const core::vector<core::u32> &           getBoneIndices      () const;
        VG_INLINE const core::vector<core::float4x4> &      getBoneMatrices     () const;

    private:
        float                                               m_scale;
        core::vector<MeshImporterNode>                      m_nodes;
        core::vector<core::u32>                             m_boneIndices;
        core::vector<core::float4x4>                        m_boneMatrices;
    };
}

#if VG_ENABLE_INLINE
#include "Skeleton.inl"
#endif