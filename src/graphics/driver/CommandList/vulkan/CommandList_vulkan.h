#pragma once

#include "graphics/driver/CommandList/CommandList.h"

namespace vg::graphics::driver::vulkan
{
	class CommandList : public base::CommandList
	{
		using super = base::CommandList;

	public:
		                                    CommandList                 (driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
		                                    ~CommandList                ();

		void								reset                       ();
		void								close                       ();

		void								beginRenderPass             (const driver::RenderPass * _renderPass);
		void								endRenderPass               ();

		void								beginSubPass                (core::uint _subPassIndex, const driver::SubPass * _subPass);
		void								endSubPass                  ();

        void                                bindRootSignature           (driver::RootSignature * _rootSig);
        void                                bindGraphicPipelineState    (driver::GraphicPipelineState * _pso);
        void                                bindPrimitiveTopology       (PrimitiveTopology _topology);
        void                                bindViewport                (const core::uint4 & _viewport);
        void                                bindScissor                 (const core::uint4 & _scissor);
        void                                bindRootConstants           (core::uint(&_constants)[max_root_constants]);

        void								clear                       (const core::float4 & _color);
        void                                draw                        (core::uint _vertexCount, core::uint _startOffset);

        void                                copyBuffer                  (driver::Buffer * _dst, core::u32 _from);
        void                                copyTexture                 (driver::Texture * _dst, core::u32 _from);

		VkCommandBuffer &					getVulkanCommandBuffer      () { return m_vkCommandBuffer;  }

	private:
		VkCommandBuffer						m_vkCommandBuffer;
	};
}