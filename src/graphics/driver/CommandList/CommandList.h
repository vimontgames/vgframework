#pragma once

#include "core/Object/Object.h"
#include "CommandList_consts.h"
#include "graphics/driver/Device/Device_consts.h"

namespace vg::graphics::driver
{
	class CommandPool;

	class RenderPass;
	class SubPass;

	namespace base
	{
		class CommandList : public core::Object
		{
		public:
			CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
			~CommandList();

			CommandListType			getType			() const;
			CommandPool *			getCommandPool	();

			void					beginRenderPass	(const driver::RenderPass * _renderPass);
			void					endRenderPass	();

			void					beginSubPass	(core::uint _subPassIndex, const driver::SubPass * _subPass);
			void					endSubPass		();

			const RenderPass *		getRenderPass	() const;
			const core::uint		getSubPassIndex	() const;

		private:
			CommandListType			m_type;
			core::u8				m_frame;
			core::u8				m_index;
			driver::CommandPool *	m_cmdPool;

			const RenderPass *		m_renderPass   = nullptr;
			const SubPass *			m_subPass	   = nullptr;
			core::uint				m_subPassIndex = -1;
		};
	}
}

#include VG_GRAPHICSAPI_HEADER(CommandList)

namespace vg::graphics::driver
{
	class CommandList : public VG_GRAPHICSAPI::CommandList
	{
		using super = VG_GRAPHICSAPI::CommandList;

	public:
		CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
		~CommandList();

        void    reset();
        void    close();
	};
}