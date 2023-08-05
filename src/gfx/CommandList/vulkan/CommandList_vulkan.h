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

		void								beginRenderPass             (gfx::RenderPass * _renderPass);
		void								endRenderPass               ();

		void								beginSubPass                (core::uint _subPassIndex, gfx::SubPass * _subPass);
		void								endSubPass                  ();

        void                                bindRootSignature           (gfx::RootSignature * _rootSig);
        void                                bindGraphicPipelineState    (gfx::GraphicPipelineState * _pso);
        void                                bindPrimitiveTopology       (PrimitiveTopology _topology);
        void                                bindViewport                (const core::uint4 & _viewport);
        void                                bindScissor                 (const core::uint4 & _scissor);
        void                                bindRootConstants           (core::uint(&_constants)[max_root_constants]);
        void                                bindIndexBuffer             (gfx::Buffer * _ib);

        void								clear                       (const core::float4 & _color);

        void                                draw                        (core::uint _vertexCount, core::uint _startOffset);
        void                                drawIndexed                 (core::uint _indexCount, core::uint _startIndex, core::uint _baseVertex);

        void                                copyBuffer                  (gfx::Buffer * _dst, core::uint_ptr _from);
        void                                copyTexture                 (gfx::Texture * _dst, core::uint_ptr _from);

        void                                beginGPUEvent               (const char * _name, core::u32 _color);
        void                                endGPUEvent                 ();

        //void *                              map                         (gfx::Buffer * _buffer);
        //void                                unmap                       (gfx::Buffer * _buffer);

		VkCommandBuffer &					getVulkanCommandBuffer      () { return m_vkCommandBuffer;  }

	private:
		VkCommandBuffer						m_vkCommandBuffer;
	};
}