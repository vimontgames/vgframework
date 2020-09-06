#pragma once

#include "graphics/driver/CommandList/CommandList.h"

namespace vg::graphics::driver::dx12
{
	class CommandList : public base::CommandList
	{
		using super = base::CommandList;

	public:
											CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
											~CommandList();

		void								reset();
		void								close();

		void								beginRenderPass(const driver::RenderPass * _renderPass);
		void								endRenderPass();

		void								beginSubPass(core::uint _subPassIndex, const driver::SubPass * _subPass);
		void								endSubPass();

		void								clear(const core::float4 & _color);

		D3D12GraphicsCommandList *		getd3d12GraphicsCommandList();
		ID3D12CommandList *					getd3d12CommandList() { return m_d3d12cmdList; }

	private:
		union
		{
			D3D12GraphicsCommandList *	m_d3d12graphicsCmdList;
			ID3D12CommandList *				m_d3d12cmdList;
		};
	};
}