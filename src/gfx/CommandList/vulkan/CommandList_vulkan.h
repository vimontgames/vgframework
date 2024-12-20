#pragma once

#include "gfx/CommandList/CommandList.h"

namespace vg::gfx::vulkan
{
	class CommandList : public base::CommandList
	{
		using super = base::CommandList;

	public:
		                                    CommandList                 (gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
		                                    ~CommandList                ();

		void								reset                       ();
		void								close                       ();

        // Graphic
		void								beginRenderPass             (gfx::RenderPass * _renderPass);
		void								endRenderPass               ();

		void								beginSubPass                (core::uint _subPassIndex, gfx::SubPass * _subPass);
		void								endSubPass                  ();

        void                                enablePerSampleShading      (bool _perSampleShading);

        void                                bindGraphicRootSignature    (gfx::RootSignature * _rootSig);
        void                                bindGraphicPipelineState    (gfx::GraphicPipelineState * _pso);
        void                                bindStencilRefValue         (core::u8 _stencilRef);
        void                                bindViewport                (const core::uint4 & _viewport);
        void                                bindScissor                 (const core::uint4 & _scissor);
        void                                bindGraphicRootConstants    (core::uint(&_constants)[max_root_constants]);
        void                                bindIndexBuffer             (gfx::Buffer * _ib);

        void								clear                       (const core::float4 & _color);

        void                                draw                        (core::uint _vertexCount, core::uint _startOffset);
        void                                drawIndexed                 (core::uint _indexCount, core::uint _startIndex, core::uint _baseVertex);
        
        // Compute
        void                                bindComputeRootSignature	(gfx::RootSignature * _rootSig);
        void                                bindComputePipelineState	(gfx::ComputePipelineState * _pso);
		void                                bindComputeRootConstants	(core::uint(&_constants)[max_root_constants]);

        void                                dispatch                    (core::uint3 _threadGroupCount);

        // Copy
        void                                copyBuffer                  (gfx::Buffer * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset, size_t _size);
        void                                copyTexture                 (gfx::Texture * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset);

        // Misc
        Map                                 map                         (gfx::Buffer * _buffer, size_t _size);
        void                                unmap                       (gfx::Buffer * _buffer, void * VG_RESTRICT _data, size_t _size);

        void                                transitionResource          (gfx::Texture * _texture, ResourceState _before, ResourceState _after);
        void                                transitionResource          (gfx::Buffer * _buffer, ResourceState _before, ResourceState _after);

        void								addRWTextureBarrier         (gfx::Texture * _texture);
        void								addRWBufferBarrier          (gfx::Buffer * _buffer);

        void								clearRWBuffer               (gfx::Buffer * _buffer, core::uint _clearValue);

        void                                beginGPUEvent               (const char * _name, core::u32 _color);
        void                                endGPUEvent                 ();

        // KHR_Acceleration_Structure extension device funcs
        void							    buildAccelerationStructures(core::u32 _infoCount, const VkAccelerationStructureBuildGeometryInfoKHR * _infos, const VkAccelerationStructureBuildRangeInfoKHR * const * _buildRangeInfos);

		VkCommandBuffer &					getVulkanCommandBuffer      () { return m_vkCommandBuffer;  }

	private:
		VkCommandBuffer						m_vkCommandBuffer;
	};
}