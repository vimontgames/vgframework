#if GPUDATAOFFSET_FRAME_COUNTER
#include "renderer/Renderer.h"
#endif

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Returns true if the flag was just set
    //--------------------------------------------------------------------------------------
    VG_INLINE bool GraphicInstance::setAtomicFlags(AtomicFlags _flags)
    {
        return 0 == (_flags & m_atomicFlags.fetch_or(_flags));
    }

    //--------------------------------------------------------------------------------------
    // Returns true if the flag was just removed
    //--------------------------------------------------------------------------------------
    VG_INLINE bool GraphicInstance::removeAtomicFlags(AtomicFlags _flags)
    {
        return 0 != (_flags & m_atomicFlags.fetch_and(~_flags));
    }

    //--------------------------------------------------------------------------------------
    // Returns true if the flag was just removed
    //--------------------------------------------------------------------------------------
    VG_INLINE bool GraphicInstance::testAtomicFlags(AtomicFlags _flags) const
    {
        return 0 != (_flags & m_atomicFlags.load());
    }

    //--------------------------------------------------------------------------------------
    // Offset in Buffer 'RESERVEDSLOT_BUFSRV_INSTANCEDATA' with Instance and its material data
    // Filled once during InstanceDataUpdatePass. 
    //--------------------------------------------------------------------------------------
    VG_INLINE void GraphicInstance::setGPUInstanceDataOffset(core::uint _offset)
    {
        #if GPUDATAOFFSET_FRAME_COUNTER
        m_gpuInstanceDataFrameIndex = Renderer::get()->getFrameCounter();
        #endif

        m_gpuInstanceDataHandle = _offset;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint GraphicInstance::getGPUInstanceDataOffset() const
    {
        #if GPUDATAOFFSET_FRAME_COUNTER
        VG_ASSERT(m_gpuInstanceDataFrameIndex == Renderer::get()->getFrameCounter());
        #endif

        return m_gpuInstanceDataHandle;
    }  

    //--------------------------------------------------------------------------------------
    VG_INLINE const core::BitMask & GraphicInstance::getBatchMask() const
    {
        return m_batchMask;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::Buffer * GraphicInstance::getInstanceIndexBuffer() const
    {
        return m_instanceIndexBuffer;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint GraphicInstance::getInstanceIndexBufferOffset() const
    {
        return m_instanceIndexBufferOffset;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE gfx::Buffer * GraphicInstance::getInstanceVertexBuffer() const
    {
        return m_instanceVertexBuffer;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE core::uint GraphicInstance::getInstanceVertexBufferOffset() const
    {
        return m_instanceVertexBufferOffset;
    }
}