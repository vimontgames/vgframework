#pragma once

#include "core/Object/Object.h"
#include "FrameGraph_consts.h"
#include "FrameGraphResource.h"
#include "gfx/IView.h"

namespace vg::gfx
{
	class Texture;
    class TextureDesc;
	class Buffer;
    class BufferDesc;
	class UserPass;
	class RenderPass;
	class FrameGraph;

	enum class PixelFormat : core::u8;

    struct RenderPassContext
    {
        IView * m_view;

		const core::string getFrameGraphID(const core::string & _name) const
		{
			return MakeFrameGraphID(_name, m_view->GetViewID());
		}

        static const core::string MakeFrameGraphID(const core::string & _name, ViewID _viewID)
        {
            return _name + (core::string)"-" + core::asString(_viewID.target) + core::to_string(_viewID.index);
        }
    };

	class FrameGraph : public core::Object
	{
	public:
        struct UserPassInfo
        {
            RenderPassContext	                            m_renderContext;
            UserPass *			                            m_userPass;
            core::vector<FrameGraphResourceTransitionDesc>  m_resourceTransitions;
        };

        const char * getClassName() const final { return "FrameGraph"; }

		FrameGraph();
		~FrameGraph();

		void destroyTransientResources(bool _sync = false);

		void importRenderTarget(const FrameGraphResourceID & _resID, Texture * _tex, core::float4 _clearColor = core::float4(0, 0, 0, 0), FrameGraphResource::InitState _initState = FrameGraphResource::InitState::Clear);
		void setGraphOutput(const FrameGraphResourceID & _destTexResID);

		void setup(double _dt);
		void build();
		void render();

        bool addUserPass(const RenderPassContext & _renderContext, UserPass * _userPass, const FrameGraphUserPassID & _renderPassID);

        template <class T> T * getResource(FrameGraphResource::Type _type, const FrameGraphResourceID & _resID, bool _mustExist);

        FrameGraphTextureResource *   getTextureResource  (const FrameGraphResourceID & _resID) const;
        FrameGraphBufferResource *    getBufferResource   (const FrameGraphResourceID & _resID) const;

        FrameGraphTextureResource *   addTextureResource  (const FrameGraphResourceID & _resID, const FrameGraphTextureResourceDesc & _texResDesc, Texture * _tex = nullptr);
        FrameGraphBufferResource *    addBufferResource   (const FrameGraphResourceID & _resID, const FrameGraphBufferResourceDesc & _bufResDesc, Buffer * _buf = nullptr);

		void setResized(); 

	private:

        Texture * createRenderTargetFromPool(const FrameGraphTextureResourceDesc & _textureResourceDesc);
        Texture * createDepthStencilFromPool(const FrameGraphTextureResourceDesc & _textureResourceDesc);
		Texture * createRWTextureFromPool(const FrameGraphTextureResourceDesc & _textureResourceDesc);

        Texture * createTextureFromPool(const FrameGraphTextureResourceDesc & _textureResourceDesc, bool _depthStencil = false, bool _uav = false);

        void releaseTextureFromPool(Texture *& _tex);

		void reverseAndRemoveDuplicates();

        void cleanup();

	private:
        using resource_unordered_map = core::unordered_map<FrameGraphResourceID, FrameGraphResource*, core::hash<FrameGraphResourceID>>;
        resource_unordered_map		m_resources;

		core::vector<UserPassInfo>	m_userPassInfo;
		core::vector<RenderPass*>	m_renderPasses;

        struct SharedTexture
        {
            FrameGraphTextureResourceDesc desc;
            Texture * tex = nullptr;
            bool used = false;
        };
        core::vector<SharedTexture>     m_sharedTextures;

		FrameGraphResourceID            m_outputResID;
        FrameGraphTextureResource *     m_outputRes = nullptr;
		bool							m_resized = false;
	};
}