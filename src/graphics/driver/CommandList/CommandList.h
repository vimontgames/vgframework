#pragma once

#include "core/Object/Object.h"
#include "CommandList_consts.h"
#include "graphics/driver/Device/Device_consts.h"
#include "graphics/driver/PipelineState/GraphicPipelineStateKey.h"

namespace vg::graphics::driver
{
	class CommandPool;
	class RenderPass;
	class SubPass;
    class GraphicPipelineState;
    class RasterizerState;

	namespace base
	{
		class CommandList : public core::Object
		{
		public:
			CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
			~CommandList();

			CommandListType			getType			    () const;
			CommandPool *			getCommandPool	    ();

			void					beginRenderPass	    (const driver::RenderPass * _renderPass);
			void					endRenderPass	    ();

			void					beginSubPass	    (core::uint _subPassIndex, const driver::SubPass * _subPass);
			void					endSubPass		    ();

			const RenderPass *		getRenderPass	    () const;
			const core::uint		getSubPassIndex	    () const;

            void                    setRootSignature    (const RootSignatureHandle & _rsHandle);
            void                    setRasterizerState  (const driver::RasterizerState & _rsState);
            void                    setShader           (const ShaderKey & _key);
            void                    setPrimitiveTopology(PrimitiveTopology _topology);
            void                    setViewport         (const core::uint4 & _viewport);
            void                    setScissor          (const core::uint4 & _scissor);

		private:
			CommandListType			m_type;
			core::u8				m_frame;
			core::u8				m_index;
			driver::CommandPool *	m_cmdPool;

			const RenderPass *		m_renderPass   = nullptr;
			const SubPass *			m_subPass	   = nullptr;
			core::uint				m_subPassIndex = -1;

        protected:
            struct StateCache
            {
                StateCache() = default;

                enum class DirtyFlags : core::u32
                {
                    RootSignature        = 0x00000001,
                    GraphicPipelineState = 0x00000002,
                    PrimitiveTopology    = 0x00000004,
                    Viewport             = 0x00000008,
                    Scissor              = 0x00000010
                };
                DirtyFlags              dirtyFlags;
                GraphicPipelineStateKey graphicPipelineKey;
                PrimitiveTopology       primitiveTopology;
                core::uint4             viewport;
                core::uint4             scissor;
            };
            StateCache              m_stateCache;

            std::unordered_map<driver::GraphicPipelineStateKey, driver::GraphicPipelineState*, driver::GraphicPipelineStateKey::hash> m_graphicPipelineStateHash;
		};
	}
}

#include VG_GFXAPI_HEADER(CommandList)

namespace vg::graphics::driver
{
	class CommandList : public VG_GFXAPI::CommandList
	{
		using super = VG_GFXAPI::CommandList;

	public:
		CommandList(driver::CommandListType _type, driver::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
		~CommandList();

        void    reset();
        void    close();

        void    flush();
        void    draw(core::uint _vertexCount, core::uint _startOffset = 0);
	};
}