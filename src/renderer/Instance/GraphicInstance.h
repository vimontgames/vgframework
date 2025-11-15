#pragma once

#include "renderer/IGraphicInstance.h"
#include "core/Misc/BitMask/BitMask.h"

enum class VertexFormat : vg::core::uint;

namespace vg::core
{
    class AABB;
}

namespace vg::gfx
{
    struct BindlessBufferHandle;
    class Buffer;
}

namespace vg::renderer
{
    class MaterialModel;
    class View;
    class Frustum;
    struct CullingResult;

    class GraphicInstance : public IGraphicInstance
    {
    public:
        enum AtomicFlags : core::u32
        {
            InstanceList = 0x00000001,  // Has been added to instance list
            SkinList     = 0x00000002,  // Has been added to skin list
            ParticleList = 0x00000004   // Has been added to particle system list
        };

        VG_CLASS_DECL(GraphicInstance, IGraphicInstance);

        GraphicInstance(const core::string & _name, core::IObject * _parent);
        ~GraphicInstance();

        // IGraphicInstance default implementations
        bool                                            IsSkinned                   () const override { return false; }
        bool                                            TryGetAABB                  (core::AABB & _aabb) const override { return false; }
        bool                                            SetMaterialCount            (core::uint _count) override;
        bool                                            SetMaterial                 (core::uint _index, IMaterialModel * _materialModel) override;
        IMaterialModel *                                GetMaterial                 (core::uint _index) const override;
        void                                            SetBatchMask                (const core::BitMask & _batchMask) override;
        void                                            ClearPickingID              () override;
        void                                            SetPickingID                (PickingID _id) override;
        PickingID                                       GetPickingID                () const override;

        // New virtual functions added to GraphicInstance
        virtual bool                                    Cull                        (CullingResult * _cullingResult, View * _view) const = 0;
        virtual void                                    OnMaterialChanged           (core::uint _index) {}
        virtual bool                                    OnUpdateRayTracing          (gfx::CommandList * _cmdList, View * _view, core::uint _index) { return false; }

        // Size returned by 'GetGPUInstanceDataSize' and written by 'FillGPUInstanceData' must match
        virtual core::uint                              GetGPUInstanceDataSize      () const { return 0; }
        virtual core::uint                              FillGPUInstanceData         (const core::u8 * VG_RESTRICT _data) const { return 0; }

        // Size returned by 'GetGPURenderDataSize' and written by 'FillGPURenderData' must match
        virtual core::uint                              GetGPURenderDataSize        () const { return 0; }
        virtual core::uint                              FillGPURenderData           (const core::u8 * VG_RESTRICT _data) { return 0; }

        virtual bool                                    GetIndexBuffer              (gfx::BindlessBufferHandle & _vb, core::uint & _offset, core::uint & _indexSize) const { return false; }
        virtual bool                                    GetVertexBuffer             (gfx::BindlessBufferHandle & _vb, core::uint & _offset) const { return false; }
        virtual bool                                    GetVertexFormat             (VertexFormat & _vertexFormat) const { return false; }
        virtual core::uint                              GetBatchCount               () const { return 1; }
        virtual core::uint                              GetBatchOffset              (core::uint _index) const { return 0; }

        // internal
        bool                                            setMaterialCount            (core::uint _count);
        bool                                            setMaterial                 (core::uint _index, MaterialModel * _materialModel);
        MaterialModel *                                 getMaterial                 (core::uint _index) const;

        void                                            setDynamicBuffer            (const gfx::Buffer * _buffer, core::uint _offset);
  
        VG_INLINE const core::vector<MaterialModel *> & getMaterials                () const { return m_materials;}
        VG_INLINE bool                                  setAtomicFlags              (AtomicFlags _flag) const;
        VG_INLINE bool                                  removeAtomicFlags           (AtomicFlags _flag);
        VG_INLINE void                                  setGPUInstanceDataOffset    (core::uint _offset);
        VG_INLINE core::uint                            getGPUInstanceDataOffset    () const;
        VG_INLINE const core::BitMask &                 getBatchMask                () const;
        VG_INLINE const gfx::Buffer *                   getSkinnedMeshBuffer        () const;
        VG_INLINE const core::uint                      getSkinnedMeshBufferOffset  () const;   

    private:
        mutable core::atomic<core::u32>                 m_atomicFlags;
        core::uint                                      m_gpuInstanceDataHandle = -1;
        PickingID                                       m_pickingID;
        core::vector<MaterialModel *>                   m_materials;
        core::BitMask                                   m_batchMask;
        const gfx::Buffer *                             m_dynamicMeshBuffer = nullptr;
        core::uint                                      m_dynamicMeshBufferOffset = -1;
    };
}

#if VG_ENABLE_INLINE
#include "GraphicInstance.inl"
#endif