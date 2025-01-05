#pragma once

#include "core/Object/Object.h"
#include "FrameGraph_consts.h"
#include "FrameGraphResource.h"
#include "RenderPassContext.h"
#include "core/string/string.h"

namespace vg::gfx
{
	class Texture;
    class TextureDesc;
	class Buffer;
    class BufferDesc;
	class UserPass;
	class RenderPass;
	class FrameGraph;
    class CommandList;
    class RenderJob;

	enum class PixelFormat : core::u8;

    // TODO: separate .h/.hpp files for UserPassInfo and UserPassInfoNode?
    struct UserPassInfo
    {
        RenderPassContext	                            m_renderContext;
        UserPass *                                      m_userPass;
        core::vector<FrameGraphResourceTransitionDesc>  m_resourceTransitions;
    };

    struct UserPassInfoNode
    {
        UserPassInfoNode(UserPassInfoNode * _parent = nullptr);

        void cacheCostEstimate();
        inline core::u64 getCostEstimate() const { return m_cost; }

        core::string                                    m_name;
        core::u64                                       m_cost = 0;
        UserPassInfo *                                  m_userPassInfo = nullptr;
        RenderPass *                                    m_renderPass = nullptr;
        UserPassInfoNode *                              m_parent = nullptr;
        core::vector<UserPassInfoNode>                  m_children;
    };

	class FrameGraph : public core::Object
	{
	public:
        const char * GetClassName() const final { return "FrameGraph"; }

		FrameGraph();
		~FrameGraph();

		void                            destroyTransientResources   (bool _sync = false);

		void                            importRenderTarget          (const FrameGraphResourceID & _resID, Texture * _tex, core::float4 _clearColor = defaultOptimizedClearColor, FrameGraphResource::InitState _initState = FrameGraphResource::InitState::Clear);
		void                            setGraphOutput              (const FrameGraphResourceID & _destTexResID);

		void                            setup                       ();
		void                            build                       ();
		void                            render                      ();

        void                            pushPassGroup               (const core::string & _name);
        void                            popPassGroup                ();

        bool                            addUserPass                 (const RenderPassContext & _renderContext, UserPass * _userPass, const FrameGraphUserPassID & _renderPassID);

        template <class T> T *          getResource                 (FrameGraphResourceType _type, const FrameGraphResourceID & _resID, bool _mustExist);

        FrameGraphTextureResource *     getTextureResource          (const FrameGraphResourceID & _resID) const;
        FrameGraphBufferResource *      getBufferResource           (const FrameGraphResourceID & _resID) const;

        FrameGraphTextureResource *     addTextureResource          (const FrameGraphResourceID & _resID, const FrameGraphTextureResourceDesc & _texResDesc, Texture * _tex = nullptr);
        FrameGraphBufferResource *      addBufferResource           (const FrameGraphResourceID & _resID, const FrameGraphBufferResourceDesc & _bufResDesc, Buffer * _buf = nullptr);

		void                            setResized                  (); 

        void                            RenderNode                  (gfx::CommandList * _cmdList, const UserPassInfoNode * _node);

	private:
        void                            setupNode                   (UserPassInfoNode & _node);
        void                            buildNode                   (UserPassInfoNode & _node);
        void                            gatherNodes                 (UserPassInfoNode & _node, core::vector<UserPassInfoNode> & _nodes, core::u64 & _totalEstimatedCost);
        void                            gatherResources             (const UserPassInfoNode & _node); 
        void                            prepareNode                 (const UserPassInfoNode & _node);
        void                            renderNode                  (const UserPassInfoNode & _node, gfx::CommandList * _cmdList, bool _recur);

        Texture *                       createRenderTargetFromPool  (const core::string & _name, const FrameGraphTextureResourceDesc & _textureResourceDesc, core::uint _createPassIndex);
        Texture *                       createDepthStencilFromPool  (const core::string & _name, const FrameGraphTextureResourceDesc & _textureResourceDesc, core::uint _createPassIndex);
		Texture *                       createRWTextureFromPool     (const core::string & _name, const FrameGraphTextureResourceDesc & _textureResourceDesc, core::uint _createPassIndex);

        Texture *                       createTextureFromPool       (const core::string & _name, const FrameGraphTextureResourceDesc & _textureResourceDesc, core::uint _createPassIndex, bool _renderTarget, bool _depthStencil, bool _uav);
        void                            releaseTextureFromPool      (Texture *& _tex);

        Buffer *                        createRWBufferFromPool      (const core::string & _name, const FrameGraphBufferResourceDesc & _bufferResourceDesc, core::uint _createPassIndex);
        Buffer *                        createBufferFromPool        (const core::string & _name, const FrameGraphBufferResourceDesc & _bufferResourceDesc, core::uint _createPassIndex, bool _uav);
        void                            releaseBufferFromPool       (Buffer *& _buffer);

        void                            cleanup                     ();

	private:
        using resource_unordered_map = core::unordered_map<FrameGraphResourceID, FrameGraphResource*, core::hash<FrameGraphResourceID>>;
        resource_unordered_map		    m_resources;

		core::vector<UserPassInfo>	    m_userPassInfo;
		core::vector<RenderPass*>	    m_renderPasses;


        UserPassInfoNode                m_userPassInfoTree;
        UserPassInfoNode *              m_currentUserPass = &m_userPassInfoTree;

        struct SharedTexture
        {
            FrameGraphTextureResourceDesc desc;
            Texture * tex = nullptr;
            bool used = false;
        };
        core::vector<SharedTexture>     m_sharedTextures;

        struct SharedBuffer
        {
            FrameGraphBufferResourceDesc desc;
            Buffer * buffer = nullptr;
            bool used = false;
        };
        core::vector<SharedBuffer>      m_sharedBuffers;

		FrameGraphResourceID            m_outputResID;
        FrameGraphTextureResource *     m_outputRes = nullptr;
		bool							m_resized = false;

        core::vector<gfx::RenderJob *>  m_renderJobs;
	};
}