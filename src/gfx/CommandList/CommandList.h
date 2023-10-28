#pragma once

#include "core/Object/Object.h"
#include "CommandList_consts.h"
#include "gfx/Device/Device_consts.h"
#include "gfx/PipelineState/Graphic/GraphicPipelineStateKey.h"

namespace vg::gfx
{
	class CommandPool;
	class RenderPass;
	class SubPass;
    class GraphicPipelineState;
    class RasterizerState;
    class BlendState;
    class Texture;
    class Buffer;

	namespace base
	{
		class CommandList : public core::Object
		{
		public:
			CommandList(gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
			~CommandList();

			CommandListType			getType			        () const;
			CommandPool *			getCommandPool	        () const;

			void					beginRenderPass	        (gfx::RenderPass * _renderPass);
			void					endRenderPass	        ();

			void					beginSubPass	        (core::uint _subPassIndex, gfx::SubPass * _subPass);
			void					endSubPass		        ();

			const RenderPass *		getRenderPass	        () const;
            const SubPass *         getSubPass              () const;
			const core::uint		getSubPassIndex	        () const;

            void                    setGraphicRootSignature (const RootSignatureHandle & _rsHandle);

            void                    setRasterizerState      (const gfx::RasterizerState & _rs);
            void                    setDepthStencilState    (const gfx::DepthStencilState & _ds);
            void                    setBlendState           (const gfx::BlendState & _bs);
            void                    setShader               (const ShaderKey & _key);
            void                    setPrimitiveTopology    (PrimitiveTopology _topology);
            void                    setViewport             (const core::uint4 & _viewport);
            void                    setScissor              (const core::uint4 & _scissor);
            void                    setRootConstants        (core::uint _startOffset, core::u32 * _values, core::uint _count);
            void                    setInlineRootConstants  (const void * _value, core::uint _count);
            void                    setIndexBuffer          (gfx::Buffer * _ib);

		protected:
			CommandListType			m_type;
			core::u8				m_frame;
			core::u8				m_index;
			gfx::CommandPool *	m_cmdPool;

			RenderPass *		    m_renderPass   = nullptr;
			SubPass *			    m_subPass	   = nullptr;
			core::uint				m_subPassIndex = -1;

        protected:
            struct StateCache
            {
                StateCache() = default;

                enum class DirtyFlags : core::u32
                {
                    RootSignature        = 0x00000001,
                    GraphicPipelineState = 0x00000002,
                    Viewport             = 0x00000004,
                    Scissor              = 0x00000008,
                    RootConstants        = 0x00000010,
                    IndexBuffer          = 0x00000020
                };
                DirtyFlags              dirtyFlags;
                GraphicPipelineStateKey graphicPipelineKey;
                core::uint4             viewport;
                core::uint4             scissor;
                core::u32               rootConstants[max_root_constants] = {};
                Buffer *                indexBuffer = nullptr;
            };
            StateCache                  m_stateCache;
            RootSignature *             m_currentGraphicRootSignature = nullptr;

            core::unordered_map<gfx::GraphicPipelineStateKey, gfx::GraphicPipelineState*, gfx::GraphicPipelineStateKey::hash> m_graphicPipelineStateHash; // PSO should not be in command list !
		};
	}
}

#include VG_GFXAPI_HEADER(CommandList)

namespace vg::gfx
{
	class CommandList : public VG_GFXAPI::CommandList
	{
		using super = VG_GFXAPI::CommandList;

	public:
        const char *                getClassName                () const final { return "CommandList"; }

		                            CommandList                 (gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index);
		                            ~CommandList                ();

        void                        reset();
        void                        close();

        bool                        applyGraphicPipelineState   ();

        void                        draw                        (core::uint _vertexCount, core::uint _startOffset = 0);
        void                        drawIndexed                 (core::uint _indexCount, core::uint _startIndex = 0, core::uint _baseVertex = 0);

        void                        resetShaders                (ShaderKey::File _file);

    private:
        bool                        getGraphicPipelineState     (const GraphicPipelineStateKey & _key, GraphicPipelineState *& _graphicPipelineState);
	};
}