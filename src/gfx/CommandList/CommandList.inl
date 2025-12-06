#include "gfx/Resource/Buffer.h"
#include "gfx/Resource/Texture.h"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::enablePerSampleShading(bool _perSampleShading)
    {
        super::enablePerSampleShading(_perSampleShading);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setGraphicRootSignature(const RootSignatureHandle & _rsHandle)
    {
        super::setGraphicRootSignature(_rsHandle);
    }
                   
    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setRasterizerState(const gfx::RasterizerState & _rs) 
    {
        super::setRasterizerState(_rs);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setDepthStencilState(const gfx::DepthStencilState & _ds) 
    {
        super::setDepthStencilState(_ds);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setStencilRefValue(core::u8 _stencilRef)
    {
        super::setStencilRefValue(_stencilRef);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setBlendState(const gfx::BlendState & _bs) 
    {
        super::setBlendState(_bs);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setShader(const ShaderKey & _key) 
    {
        super::setShader(_key);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setPrimitiveTopology(PrimitiveTopology _topology)
    {
        super::setPrimitiveTopology(_topology);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setViewport(const core::uint4 & _viewport) 
    {
        super::setViewport(_viewport);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setScissor(const core::uint4 & _scissor) 
    {
        super::setScissor(_scissor);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setGraphicRootConstants(core::uint _startOffset, core::u32 * _values, core::uint _count)
    {
        super::setGraphicRootConstants(_startOffset,_values, _count);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setIndexBuffer(gfx::Buffer * _ib) 
    {
        super::setIndexBuffer(_ib);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setComputeRootSignature(const RootSignatureHandle & _rsHandle)
    {
        super::setComputeRootSignature(_rsHandle);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setComputeShader(const ComputeShaderKey & _computeKey)
    {
        super::setComputeShader(_computeKey);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::setComputeRootConstants(core::uint _startOffset, core::u32 * _values, core::uint _count)
    {
        super::setComputeRootConstants(_startOffset, _values, _count);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::transitionResource(gfx::Texture * _texture, ResourceState _before, ResourceState _after)
    {
        VG_ASSERT(_texture);
        if (_texture)
            super::transitionResource(_texture, _before, _after);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::transitionResource(gfx::Buffer * _buffer, ResourceState _before, ResourceState _after)
    {
        VG_ASSERT(_buffer);
        if (_buffer)
            super::transitionResource(_buffer, _before, _after);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::addRWTextureBarrier(gfx::Texture * _texture)
    {
        VG_ASSERT(_texture && _texture->getTexDesc().resource.testBindFlags(BindFlags::UnorderedAccess | BindFlags::RaytracingAccelerationStruct));
        if (_texture)
            super::addRWTextureBarrier(_texture);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::addRWBufferBarrier(gfx::Buffer * _buffer)
    {
        VG_ASSERT(_buffer && _buffer->getBufDesc().resource.testBindFlags(BindFlags::UnorderedAccess | BindFlags::RaytracingAccelerationStruct));
        if (_buffer)
            super::addRWBufferBarrier(_buffer);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::addRWGlobalBarrier()
    {
        super::addRWGlobalBarrier();
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE Map CommandList::map(gfx::Buffer * _buffer, size_t _size)
    {
        if ((size_t)-1 == _size)
            _size = _buffer->getBufDesc().getSize();

        VG_ASSERT(nullptr == m_curMapInfo.buffer && nullptr == m_curMapInfo.texture);
        VG_ASSERT(_size <= _buffer->getBufDesc().getSize(), "[Device] Trying to Map %u bytes but buffer size is only %u", _size, _buffer->getBufDesc().getSize());
        VG_ASSERT(_size > 0);

        Map map = super::map(_buffer, _size);

        m_curMapInfo.buffer = _buffer;
        m_curMapInfo.data = map.data;
        m_curMapInfo.size = _size;

        return map;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::unmap(gfx::Buffer * _buffer)
    {
        VG_ASSERT(m_curMapInfo.buffer == _buffer && nullptr == m_curMapInfo.texture);

        super::unmap(_buffer, m_curMapInfo.data, m_curMapInfo.size);

        m_curMapInfo.buffer = nullptr;
        m_curMapInfo.data = nullptr;
        m_curMapInfo.size = (size_t)-1;
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::copyTexture(gfx::Texture * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset)
    {
        VG_ASSERT(_srcOffset <= _src->getBufDesc().getSize(), "Offset is greater than size %u of the source Buffer \"%s\"", _srcOffset, _src->getBufDesc().getSize(), _src->GetName().c_str());
        
        super::copyTexture(_dst, _src, _srcOffset);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::copyBuffer(gfx::Buffer * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset, size_t _size)
    {
        if ((size_t)-1 == _size)
            _size = _dst->getBufDesc().getSize();

        VG_ASSERT(_size <= _dst->getBufDesc().getSize(), "Size %u is greater than size %u of the destination Buffer \"%s\"", _size, _dst->getBufDesc().getSize(), _src->GetName().c_str());
        VG_ASSERT(_srcOffset <= _src->getBufDesc().getSize(), "Offset %u is greater than size %u of the source Buffer \"%s\"", _srcOffset, _src->getBufDesc().getSize(), _src->GetName().c_str());

        super::copyBuffer(_dst, _src, _srcOffset, _size);
    }

    //--------------------------------------------------------------------------------------
    VG_INLINE void CommandList::clearRWBuffer(Buffer * _buffer, core::uint _clearValue)
    {
        VG_ASSERT(_buffer->getBufDesc().resource.testBindFlags(BindFlags::UnorderedAccess), "Buffer \"%s\" does not have the 'UnorderedAccess' flag");
        super::clearRWBuffer(_buffer, _clearValue);
    }
}