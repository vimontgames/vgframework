#pragma once

#include "core/Object/Object.h"
#include "CommandList_consts.h"
#include "gfx/Device/Device_consts.h"
#include "gfx/PipelineState/Graphic/GraphicPipelineStateCache.h"
#include "gfx/PipelineState/Compute/ComputePipelineStateCache.h"

namespace vg::gfx
{
	class CommandPool;
	class RenderPass;
	class SubPass;
    class GraphicPipelineState;
    class RasterizerState;
    class DepthStencilState;
    class BlendState;
    class ComputePipelineState;
    class Texture;
    class Buffer;
    class UploadBuffer;

	namespace base
	{
		class CommandList : public core::Object
		{
		public:
			CommandList(gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
			~CommandList();

			CommandListType			    getType			        () const;
			CommandPool *			    getCommandPool	        () const;

            // Graphic
			void					    beginRenderPass	            (gfx::RenderPass * _renderPass);
			void					    endRenderPass	            ();

			void					    beginSubPass	            (core::uint _subPassIndex, gfx::SubPass * _subPass);
			void					    endSubPass		            ();

			const RenderPass *		    getRenderPass	            () const;
            const SubPass *             getSubPass                  () const;
			const core::uint		    getSubPassIndex	            () const;

            void                        setGraphicRootSignature     (const RootSignatureHandle & _rsHandle);

            void                        setRasterizerState          (const gfx::RasterizerState & _rs);
            void                        setDepthStencilState        (const gfx::DepthStencilState & _ds);
            void                        setStencilRefValue          (core::u8 _stencilRef);
            void                        setBlendState               (const gfx::BlendState & _bs);
            void                        setShader                   (const ShaderKey & _key);
            void                        setPrimitiveTopology        (PrimitiveTopology _topology);
            void                        setViewport                 (const core::uint4 & _viewport);
            void                        setScissor                  (const core::uint4 & _scissor);
            void                        setGraphicRootConstants     (core::uint _startOffset, core::u32 * _values, core::uint _count);
            void                        setIndexBuffer              (gfx::Buffer * _ib);

            // Compute
            void                        setComputeRootSignature     (const RootSignatureHandle & _rsHandle);
            void                        setComputeShader            (const ComputeShaderKey & _computeKey);
            void                        setComputeRootConstants     (core::uint _startOffset, core::u32 * _values, core::uint _count);

            void                        setCurrentRenderPassType    (RenderPassType _renderPassType);

		protected:
			CommandListType			    m_type;
			core::u8				    m_frame;
			core::u8				    m_index;
			gfx::CommandPool *	        m_cmdPool      = nullptr;
            UploadBuffer *              m_uploadBuffer = nullptr;
			RenderPass *		        m_renderPass   = nullptr;
			SubPass *			        m_subPass	   = nullptr;
			core::uint				    m_subPassIndex = -1;
            RenderPassType              m_currentRenderPassType = (RenderPassType)-1;
            GraphicPipelineStateCache   m_graphicStateCache;
            ComputePipelineStateCache   m_computeStateCache;
		};
	}
}

#ifdef VG_DX12
#include "dx12/CommandList_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/CommandList_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
	class CommandList : public VG_GFXAPI::CommandList
	{
		using super = VG_GFXAPI::CommandList;

	public:
        const char *                    GetClassName                () const final { return "CommandList"; }

		                                CommandList                 (gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
		                                ~CommandList                ();

        void                            reset();
        void                            close();

        void                            clearStateCache             ();

        // Graphic
        void					        beginRenderPass             (gfx::RenderPass * _renderPass);
        void					        endRenderPass               ();
        
        VG_INLINE void                  setGraphicRootSignature     (const RootSignatureHandle & _rsHandle);
         
        VG_INLINE void                  setRasterizerState          (const gfx::RasterizerState & _rs);
        VG_INLINE void                  setDepthStencilState        (const gfx::DepthStencilState & _ds);
        VG_INLINE void                  setStencilRefValue          (core::u8 _stencilRef);
        VG_INLINE void                  setBlendState               (const gfx::BlendState & _bs);
        VG_INLINE void                  setShader                   (const ShaderKey & _key);
        VG_INLINE void                  setPrimitiveTopology        (PrimitiveTopology _topology);
        VG_INLINE void                  setViewport                 (const core::uint4 & _viewport);
        VG_INLINE void                  setScissor                  (const core::uint4 & _scissor);
        VG_INLINE void                  setGraphicRootConstants     (core::uint _startOffset, core::u32 * _values, core::uint _count);
        VG_INLINE void                  setIndexBuffer              (gfx::Buffer * _ib);
        VG_INLINE void                  enablePerSampleShading      (bool _perSampleShading);

        bool                            applyGraphicPipelineState   ();

        void                            draw                        (core::uint _vertexCount, core::uint _startOffset = 0);
        void                            drawIndexed                 (core::uint _indexCount, core::uint _startIndex = 0, core::uint _baseVertex = 0);

        // Compute
        VG_INLINE void                  setComputeRootSignature     (const RootSignatureHandle & _rsHandle);
        VG_INLINE void                  setComputeShader            (const ComputeShaderKey & _computeKey);
        VG_INLINE void                  setComputeRootConstants     (core::uint _startOffset, core::u32 * _values, core::uint _count);

        bool                            applyComputePipelineState   ();

        void                            dispatch                    (core::uint3 _threadGroupCount);

        // Copy
        VG_INLINE void                  copyTexture                 (gfx::Texture * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset);
        VG_INLINE void                  copyBuffer                  (gfx::Buffer * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset, size_t _size = (size_t) - 1);

        VG_INLINE void                  transitionResource          (gfx::Texture * _texture, ResourceState _before, ResourceState _after);
        VG_INLINE void                  transitionResource          (gfx::Buffer * _buffer, ResourceState _before, ResourceState _after);

        VG_INLINE void                  addRWTextureBarrier         (gfx::Texture * _texture);
        VG_INLINE void                  addRWBufferBarrier          (gfx::Buffer * _buffer);

        VG_INLINE Map                   map                         (gfx::Buffer * _buffer, size_t _size);
        VG_INLINE void                  unmap                       (gfx::Buffer * _buffer);

        // Misc
        VG_INLINE void                  clearRWBuffer               (Buffer * _buffer, core::uint _clearValue);
        void                            resetShaders                (ShaderKey::File _file);

    private:
        struct MapInfo
        {
            Buffer *    buffer = nullptr;
            Texture *   texture = nullptr;
            void *      data = nullptr;
            size_t      size = (size_t)-1;
        };
        MapInfo                         m_curMapInfo;
	};
}

#if VG_ENABLE_INLINE
#include "CommandList.inl"
#endif