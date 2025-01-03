#include "gfx/Precomp.h"
#include "FrameGraph.h"
#include "gfx/Device/Device.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Texture.h"
#include "gfx/Resource/Buffer.h"
#include "gfx/FrameGraph/RenderPass.h"
#include "gfx/FrameGraph/SubPass.h"
#include "gfx/FrameGraph/UserPass.h"
#include "gfx/Profiler/Profiler.h"
#include "gfx/IFrameGraphView.h"
#include "core/IScheduler.h"

using namespace vg::core;

#include "shaders/system/shared_consts.hlsli"

#include "FrameGraphResource.hpp"
#include "RenderPassContext.hpp"
#include "RenderPass.hpp"
#include "SubPass.hpp"
#include "UserPass.hpp"
#include "RenderJob.hpp"

namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
    // UserPassInfoNode
    //--------------------------------------------------------------------------------------
    UserPassInfoNode::UserPassInfoNode(UserPassInfoNode * _parent) :
        m_parent(_parent)
    {
        m_children.reserve(16);
    }

    //--------------------------------------------------------------------------------------
    core::u64 UserPassInfoNode::getCostEstimate() const
    {
        return m_userPassInfo->m_userPass->GetCostEstimate();
    }

	//--------------------------------------------------------------------------------------
	// FrameGraph
	//--------------------------------------------------------------------------------------
	FrameGraph::FrameGraph()
	{
        m_userPassInfo.reserve(1024);
        m_renderPasses.reserve(1024);
	}

	//--------------------------------------------------------------------------------------
	FrameGraph::~FrameGraph()
	{
        for (uint i = 0; i < m_renderJobs.size(); ++i)
            VG_SAFE_RELEASE(m_renderJobs[i]);
        m_renderJobs.clear();

        cleanup();
        destroyTransientResources(true);        
	}

    //--------------------------------------------------------------------------------------
    void FrameGraph::destroyTransientResources(bool _sync)
    {
        if (_sync)
        {
            for (SharedTexture & shared : m_sharedTextures)
                VG_SAFE_RELEASE(shared.tex);

            for (SharedBuffer & shared : m_sharedBuffers)
                VG_SAFE_RELEASE(shared.buffer);
        }
        else
        {
            for (SharedTexture & shared : m_sharedTextures)
                VG_SAFE_RELEASE_ASYNC(shared.tex);

            for (SharedBuffer & shared : m_sharedBuffers)
                VG_SAFE_RELEASE_ASYNC(shared.buffer);
        }
        m_sharedTextures.clear();
        m_sharedBuffers.clear();
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::cleanup()
	{
        for (auto & userPassInfo : m_userPassInfo)
            VG_SAFE_RELEASE(userPassInfo.m_userPass);
        m_userPassInfo.clear();

		for (auto & pair : m_resources)
			VG_SAFE_DELETE(pair.second);
		m_resources.clear();

		for (auto * renderPass : m_renderPasses)
			VG_SAFE_RELEASE_ASYNC(renderPass);
		m_renderPasses.clear();

        m_userPassInfoTree.m_children.clear();
        m_currentUserPass = &m_userPassInfoTree;
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::importRenderTarget(const FrameGraphResourceID & _resID, Texture * _tex, float4 _clearColor, FrameGraphResource::InitState _initState)
	{
        const TextureDesc & texDesc = _tex->getTexDesc(); 

        FrameGraphTextureResourceDesc desc;
                                        desc.width = texDesc.width;
                                        desc.height = texDesc.height;
                                        desc.format = texDesc.format;
                                        desc.clearColor = _clearColor;
                                        desc.initState = _initState;

		FrameGraphTextureResource * res = addTextureResource(_resID, desc, _tex);
        VG_ASSERT(res);
	}

    //--------------------------------------------------------------------------------------
    FrameGraphTextureResource * FrameGraph::addTextureResource(const FrameGraphResourceID & _resID, const FrameGraphTextureResourceDesc & _texResDesc, Texture * _tex)
    {
        FrameGraphTextureResource * texRes = getResource<FrameGraphTextureResource>(FrameGraphResourceType::Texture, _resID, false);
        if (texRes)
        {
            texRes->setTextureResourceDesc(_texResDesc);
            texRes->setTexture(_tex);
        }
        return texRes;
    }

    //--------------------------------------------------------------------------------------
    FrameGraphBufferResource * FrameGraph::addBufferResource(const FrameGraphResourceID & _resID, const FrameGraphBufferResourceDesc & _bufResDesc, Buffer * _buf)
    {
        FrameGraphBufferResource * bufRes = getResource<FrameGraphBufferResource>(FrameGraphResourceType::Buffer, _resID, false);
        if (bufRes)
            bufRes->setBufferResourceDesc(_bufResDesc, _buf);
        return bufRes;
    }

    //--------------------------------------------------------------------------------------
    FrameGraphTextureResource * FrameGraph::getTextureResource(const FrameGraphResourceID & _resID) const
    {
        return const_cast<FrameGraph*>(this)->getResource<FrameGraphTextureResource>(FrameGraphResourceType::Texture, _resID, true);
    }

    //--------------------------------------------------------------------------------------
    FrameGraphBufferResource * FrameGraph::getBufferResource(const FrameGraphResourceID & _resID) const
    {
        return const_cast<FrameGraph*>(this)->getResource<FrameGraphBufferResource>(FrameGraphResourceType::Buffer, _resID, true);
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * FrameGraph::getResource(FrameGraphResourceType _type, const FrameGraphResourceID & _resID, bool _mustExist)
    {
        auto it = m_resources.find(_resID);
        if (m_resources.end() == it)
        {
            VG_ASSERT(!_mustExist, "%s resource \"%s\" does not exist in FrameGraph", core::asString(_type).c_str(), _resID.c_str());
            if (!_mustExist)
            {
                T * newResource = new T();
                m_resources[_resID] = newResource;

                newResource->setType(_type);
                newResource->setName(_resID);

                return newResource;
            }
        }
        else
        {
            VG_ASSERT(_type == it->second->getType(), "Resource \"%s\" has type \"%s\" but \"%s\" is requested", _resID.c_str(), asString(it->second->getType()).c_str(), asString(_type).c_str());
            return static_cast<T*>(it->second);
        }

        return nullptr;
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::setGraphOutput(const FrameGraphResourceID & _destTexResID)
	{
		m_outputResID = _destTexResID;	
        m_outputRes = getTextureResource(m_outputResID);
	}

    //--------------------------------------------------------------------------------------
    void FrameGraph::pushPassGroup(const core::string & _name)
    {
        UserPassInfoNode & node = m_currentUserPass->m_children.emplace_back();
        node.m_name = _name;
        node.m_parent = m_currentUserPass;

        m_currentUserPass = &node;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::popPassGroup()
    {
        m_currentUserPass = m_currentUserPass->m_parent;
    }

    //--------------------------------------------------------------------------------------
    bool FrameGraph::addUserPass(const RenderPassContext & _renderContext, UserPass * _userPass, const FrameGraphUserPassID & _renderPassID)
    {
        VG_ASSERT(nullptr != _userPass, "Adding NULL UserPass to FrameGraph");

        if (nullptr != _userPass)
        {
            _userPass->AddRef();
            _userPass->SetName(_renderPassID);
            _userPass->setFrameGraph(this);

            m_userPassInfo.push_back({ _renderContext, _userPass });

            UserPassInfoNode info(m_currentUserPass);
            info.m_userPassInfo = &m_userPassInfo[m_userPassInfo.size() - 1];
            info.m_name = _userPass->GetName();
            m_currentUserPass->m_children.push_back(info);

            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::setupNode(UserPassInfoNode & _node)
    {
        VG_PROFILE_CPU(_node.m_name.c_str());

        for (auto & child : _node.m_children)
            setupNode(child);

        if (_node.m_userPassInfo)
        {
            _node.m_userPassInfo->m_userPass->reset();
            _node.m_userPassInfo->m_userPass->Setup(_node.m_userPassInfo->m_renderContext);
        }
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::setup()
	{
        VG_PROFILE_CPU("setup");

        for (UserPassInfoNode & node : m_userPassInfoTree.m_children)
            setupNode(node);
	}  

    //--------------------------------------------------------------------------------------
    void FrameGraph::buildNode(UserPassInfoNode & _node)
    {
        VG_PROFILE_CPU(_node.m_name.c_str());

        for (auto & child : _node.m_children)
            buildNode(child);

        if (!_node.m_userPassInfo)
            return;

        auto userPassInfo = *_node.m_userPassInfo;

        const auto * userPass = userPassInfo.m_userPass;
        auto & renderTargets = userPass->getRenderTargets();

        core::vector<FrameGraphTextureResource *> colorAttachments;

        RenderPassKey renderPassKey;
        renderPassKey.m_subPassCount = 1;

        vector<FrameGraphResourceTransitionDesc> resourceTransitions;

        bool invalidMSAAState = false;

        // build all list with attachments from all passes
        // for (...)
        {
            auto & rwTextures = userPass->getRWTextures();
            for (uint i = 0; i < rwTextures.size(); ++i)
            {
                FrameGraphTextureResource * res = rwTextures[i];

                #ifdef VG_VULKAN
                // On Vulkan, we have to init UAV in the "Undefined" state, so we need to initialize them to "UnorderedAccess" before 1st use
                //if (res->getCurrentState() == ResourceState::Undefined)
                {
                    if (res->isFirstWrite(userPass))
                    {
                        FrameGraphResourceTransitionDesc resTrans;
                        resTrans.m_resource = res;
                        resTrans.m_transitionDesc.flags = (ResourceTransitionFlags)0;
                        resTrans.m_transitionDesc.begin = ResourceState::Undefined;
                        resTrans.m_transitionDesc.end = ResourceState::UnorderedAccess;

                        resourceTransitions.push_back(resTrans);
                        res->setCurrentState(ResourceState::UnorderedAccess);
                    }
                }
                #endif

                bool read = res->needsWriteToReadTransition(userPass);

                if (read)
                {
                    FrameGraphResourceTransitionDesc resTrans;
                    resTrans.m_resource = res;
                    resTrans.m_transitionDesc.flags = (ResourceTransitionFlags)0;
                    resTrans.m_transitionDesc.begin = ResourceState::UnorderedAccess;
                    resTrans.m_transitionDesc.end = ResourceState::ShaderResource;

                    resourceTransitions.push_back(resTrans);
                    res->setCurrentState(ResourceState::ShaderResource);
                }
            }

            auto & rwBuffers = userPass->getRWBuffers();
            for (uint i = 0; i < rwBuffers.size(); ++i)
            {
                FrameGraphBufferResource * res = rwBuffers[i];
                bool read = res->needsWriteToReadTransition(userPass);

                if (read)
                {
                    FrameGraphResourceTransitionDesc resTrans;
                    resTrans.m_resource = res;
                    resTrans.m_transitionDesc.flags = (ResourceTransitionFlags)0;
                    resTrans.m_transitionDesc.begin = ResourceState::UnorderedAccess;
                    resTrans.m_transitionDesc.end = ResourceState::ShaderResource;

                    resourceTransitions.push_back(resTrans);
                    res->setCurrentState(ResourceState::ShaderResource);
                }
            }

            auto & renderTargets = userPass->getRenderTargets();
            renderPassKey.m_msaa = (MSAA)0x0;

            for (uint i = 0; i < renderTargets.size(); ++i)
            {
                FrameGraphTextureResource * res = renderTargets[i];
                const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                renderPassKey.m_colorFormat[i] = textureResourceDesc.format;

                if ((MSAA)0x0  != renderPassKey.m_msaa && renderPassKey.m_msaa != textureResourceDesc.msaa)
                    invalidMSAAState = true;
                renderPassKey.m_msaa = textureResourceDesc.msaa;
    
                // add or get index of attachment
                uint attachmentIndex = -1;

                if ((uint)-1 == attachmentIndex)
                {
                    VG_ASSERT(textureResourceDesc.transient || res->getTexture());
                    res->setTextureResourceDesc(textureResourceDesc);
                    colorAttachments.push_back(res);
                    attachmentIndex = uint(colorAttachments.size() - 1);
                }

                ResourceTransitionFlags flags = ResourceTransitionFlags::RenderTarget;

                ResourceState begin = res->getCurrentState();
                ResourceState end = ResourceState::RenderTarget;

                bool write = res->isWrite(userPass);
                bool firstWrite = res->isFirstWrite(userPass);
                bool lastWrite = res->isLastWrite(userPass);

                const bool isBackbuffer = m_outputRes == res;

                if (write)
                {
                    if (firstWrite)
                    {
                        if (isBackbuffer)
                        {
                            begin = ResourceState::Undefined;
                        }
                        else
                        {
                            #ifdef VG_VULKAN
                            begin = ResourceState::Undefined; // Why not 'RenderTarget'? Because Vulkan.
                            #elif defined(VG_DX12)
                            begin = ResourceState::RenderTarget;
                            #else
                            VG_ASSERT_NOT_IMPLEMENTED();
                            #endif
                        }

                        flags |= ResourceTransitionFlags::Clear;
                    }
                    else
                    {
                        flags |= ResourceTransitionFlags::Preserve;
                    }
                }
                else
                {
                    flags |= ResourceTransitionFlags::Preserve;
                }

                // This is not correct in case of Write/Read/Write sequence! We should detect a write (current pass) followed by a read to resolve
                if (lastWrite)
                {
                    if (isBackbuffer)
                    {
                        flags |= ResourceTransitionFlags::Present;
                    }
                    else
                    {
                        if (res->isReadAfter(userPass))
                            end = ResourceState::ShaderResource;
                        else
                            end = ResourceState::RenderTarget;
                    }
                }

                ResourceTransitionDesc info(flags, begin, end);
                renderPassKey.m_subPassKeys[0].setColorAttachmentInfo(attachmentIndex, info);

                res->setCurrentState(info.end);
            }
        }

        FrameGraphTextureResource * depthStencilAttachment = nullptr;
        FrameGraphTextureResource * depthStencilRes = userPass->getDepthStencil();
        if (depthStencilRes)
        {
            FrameGraphTextureResource * res = userPass->getDepthStencil();
            const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

            if ((MSAA)0x0 != renderPassKey.m_msaa && renderPassKey.m_msaa != textureResourceDesc.msaa)
                invalidMSAAState = true;
            renderPassKey.m_msaa = textureResourceDesc.msaa;

            renderPassKey.m_depthStencilFormat = textureResourceDesc.format;

            ResourceTransitionFlags flags = ResourceTransitionFlags::RenderTarget;

            ResourceState begin = res->getCurrentState();;
            ResourceState end = ResourceState::RenderTarget;

            bool write = res->isWrite(userPass);
            bool firstWrite = res->isFirstWrite(userPass);

            if (write)
            {
                // Clear depthstencil on first write
                if (firstWrite)
                {
                    #ifdef VG_VULKAN
                    begin = ResourceState::Undefined;
                    #elif defined(VG_DX12)
                    begin = ResourceState::RenderTarget;
                    #endif

                    flags |= ResourceTransitionFlags::Clear;
                }
                else
                {
                    flags |= ResourceTransitionFlags::Preserve;
                }
            }
            else
            {
                // Use existing depthstencil content
                flags |= ResourceTransitionFlags::Preserve;
            }

            ResourceTransitionDesc info(flags, begin, end);
            renderPassKey.m_subPassKeys[0].setDepthStencilAttachmentInfo(info);

            res->setCurrentState(info.end);

            depthStencilAttachment = res;
        }

        if (invalidMSAAState)
        {
            string invalidMSAAStateMsg = "RenderTarget(s)/DepthStencil are using incompatible MSAA:\n";

            for (uint i = 0; i < renderTargets.size(); ++i)
            {
                const FrameGraphTextureResourceDesc & textureResourceDesc = renderTargets[i]->getTextureResourceDesc();
                invalidMSAAStateMsg += fmt::sprintf("RenderTarget[%u]: %s (\"%s\")", i, asString(textureResourceDesc.msaa).c_str(), renderTargets[i]->getName().c_str());

                if (depthStencilRes || i + 1 < renderTargets.size())
                    invalidMSAAStateMsg += "\n";
            }

            if (depthStencilRes)
            {
                const FrameGraphTextureResourceDesc & textureResourceDesc = userPass->getDepthStencil()->getTextureResourceDesc();
                invalidMSAAStateMsg += fmt::sprintf("DepthStencil:: %s (\"%s\")", asString(textureResourceDesc.msaa).c_str(), userPass->getDepthStencil()->getName().c_str());
            }

            VG_ASSERT(!invalidMSAAState, invalidMSAAStateMsg.c_str());
        }

        RenderPass * renderPass = new RenderPass(userPass->getUserPassType(), renderPassKey);
        renderPass->m_colorAttachments = std::move(colorAttachments); // transient resources will be lazy allocated before the actuel RenderPass begins
        renderPass->m_depthStencilAttachment = depthStencilAttachment;

        SubPass * subPass = new SubPass();

        userPassInfo.m_resourceTransitions = std::move(resourceTransitions);

        subPass->addUserPassInfo(userPassInfo);
        renderPass->addSubPass(subPass);
        m_renderPasses.push_back(renderPass);

        _node.m_renderPass = renderPass;

        renderPass->finalize();
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::build()
	{
        VG_PROFILE_CPU("build");

        // Cleanup
		for (auto * renderPass : m_renderPasses)
			VG_SAFE_RELEASE(renderPass);
		m_renderPasses.clear();

        Device * device = Device::get();

        for (auto & node : m_userPassInfoTree.m_children)
            buildNode(node);
	}

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createRenderTargetFromPool(const core::string & _name, const FrameGraphTextureResourceDesc & _textureResourceDesc, core::uint _createPassIndex)
    {
        return createTextureFromPool(_name, _textureResourceDesc, _createPassIndex, true, false, false);
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createDepthStencilFromPool(const core::string & _name, const FrameGraphTextureResourceDesc & _textureResourceDesc, core::uint _createPassIndex)
    {
        return createTextureFromPool(_name, _textureResourceDesc, _createPassIndex, false, true, false);
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createRWTextureFromPool(const core::string & _name, const FrameGraphTextureResourceDesc & _textureResourceDesc, core::uint _createPassIndex)
    {
        return createTextureFromPool(_name, _textureResourceDesc, _createPassIndex, false, false, true);
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createTextureFromPool(const core::string & _name, const FrameGraphTextureResourceDesc & _textureResourceDesc, core::uint _createPassIndex, bool _renderTarget, bool _depthStencil, bool _uav)
    {
        VG_ASSERT(_depthStencil == Texture::isDepthStencilFormat(_textureResourceDesc.format));

        // Use settings from desc?
        _uav = _textureResourceDesc.uav;
        //_renderTarget = _textureResourceDesc.renderTarget;

        for (uint i = 0; i < m_sharedTextures.size(); ++i)
        {
            auto & slot = m_sharedTextures[i];
            if (!slot.used)
            {
                if (slot.desc == _textureResourceDesc)
                {
                    slot.used = true;
                    VG_ASSERT(slot.tex);

                    const auto & desc = slot.tex->getTexDesc();

                    if (_renderTarget)
                        VG_ASSERT(desc.isRenderTarget());

                    if (_depthStencil)
                        VG_ASSERT(desc.isDepthStencil());

                    if (_uav)
                        VG_ASSERT(desc.resource.testBindFlags(BindFlags::UnorderedAccess));

                    return slot.tex;
                }
            }
        }

        // create if not found
        Device * device = Device::get();
       
        TextureDesc desc;
                    desc.type = _textureResourceDesc.type;
                    desc.format = _textureResourceDesc.format;
                    desc.width = _textureResourceDesc.width;
                    desc.height = _textureResourceDesc.height;
                    desc.slices = 1;
                    desc.msaa = _textureResourceDesc.msaa;
                    desc.flags = (TextureFlags)0x0;

        switch (desc.type)
        {
            case TextureType::Texture1D:
            case TextureType::Texture2D:
            case TextureType::Texture3D:
            case TextureType::TextureCube:
            case TextureType::Texture1DArray:
            case TextureType::Texture2DArray:
            case TextureType::TextureCubeArray:
                VG_ASSERT(MSAA::None == desc.msaa, "\"%s\" has TextureType::%s but is using MSAA::%s", _name.c_str(), asString(desc.type).c_str(), asString(desc.msaa).c_str());
                break;

            case TextureType::Texture2DMS:
            case TextureType::Texture2DMSArray:
                VG_ASSERT(MSAA::None!= desc.msaa, "\"%s\" has TextureType::%s but is using MSAA::%s", _name.c_str(), asString(desc.type).c_str(), asString(desc.msaa).c_str());
                break;
        }

        if (_renderTarget)
            desc.flags |= TextureFlags::RenderTarget;
        else if (_depthStencil)
            desc.flags |= TextureFlags::DepthStencil;
                    
        if (_uav)
            desc.resource.m_bindFlags = BindFlags::ShaderResource | BindFlags::UnorderedAccess; // What about "UAV-only" textures?
        else
            desc.resource.m_bindFlags = BindFlags::ShaderResource;

        desc.resource.m_cpuAccessFlags = CPUAccessFlags::None;
        desc.resource.m_usage = Usage::Default;

        string name = "Temp";

        if (_renderTarget)
            name += "RenderTarget";
        else if (_depthStencil)
            name += "DepthStencil";
        else if (_uav)
            name += "RWTexture";
        else
            name += "Texture";

        name += "#" + to_string(m_sharedTextures.size());

        if (asBool(BindFlags::ShaderResource & desc.resource.m_bindFlags))
            name += "_SRV";
        if (asBool(BindFlags::UnorderedAccess & desc.resource.m_bindFlags))
            name += "_RW";
        
        SharedTexture sharedTex;
                      sharedTex.desc = _textureResourceDesc;
                      sharedTex.tex  = device->createTexture(desc, name.c_str());
                      sharedTex.used = true;

        m_sharedTextures.push_back(sharedTex);
        return sharedTex.tex;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::releaseTextureFromPool(Texture *& _tex)
    {
        VG_ASSERT(nullptr != _tex);
        for (uint i = 0; i < m_sharedTextures.size(); ++i)
        {
            auto & slot = m_sharedTextures[i];
            if (slot.tex == _tex)
            {
                slot.used = false;
                _tex = nullptr;
                return;
            }
        }
        VG_ASSERT(false, "Could not release Texture \"%s\" from pool", _tex->GetName());
    }

    //--------------------------------------------------------------------------------------
    Buffer * FrameGraph::createRWBufferFromPool(const core::string & _name, const FrameGraphBufferResourceDesc & _bufferResourceDesc, core::uint _createPassIndex)
    {
        return createBufferFromPool(_name, _bufferResourceDesc, _createPassIndex, true);
    }

    //--------------------------------------------------------------------------------------
    Buffer * FrameGraph::createBufferFromPool(const core::string & _name, const FrameGraphBufferResourceDesc & _bufferResourceDesc, core::uint _createPassIndex, bool _uav)
    {
        VG_ASSERT(_uav == _bufferResourceDesc.uav);

        for (uint i = 0; i < m_sharedBuffers.size(); ++i)
        {
            auto & slot = m_sharedBuffers[i];
            if (!slot.used)
            {
                if (slot.desc == _bufferResourceDesc)
                {
                    slot.used = true;
                    VG_ASSERT(slot.buffer);
                    return slot.buffer;
                }
            }
        }

        // create if not found
        Device * device = Device::get();

        BufferDesc desc;
        desc.elementSize = _bufferResourceDesc.elementSize;
        desc.elementCount = _bufferResourceDesc.elementCount;
        desc.flags = BufferFlags::None;

        if (_uav)
            desc.resource.m_bindFlags = BindFlags::ShaderResource | BindFlags::UnorderedAccess; // What about "UAV-only" buffers?
        else
            desc.resource.m_bindFlags = BindFlags::ShaderResource;

        desc.resource.m_cpuAccessFlags = CPUAccessFlags::None;
        desc.resource.m_usage = Usage::Default;

        string name = "Temp";
        if (asBool(BindFlags::UnorderedAccess & desc.resource.m_bindFlags))
            name += "RWBuffer";
        else if (asBool(BindFlags::ShaderResource & desc.resource.m_bindFlags))
            name += "Buffer";
        name += "#" + to_string(m_sharedBuffers.size());

        SharedBuffer sharedBuf;
        sharedBuf.desc = _bufferResourceDesc;
        sharedBuf.buffer = device->createBuffer(desc, name.c_str());
        sharedBuf.used = true;

        m_sharedBuffers.push_back(sharedBuf);
        return sharedBuf.buffer;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::releaseBufferFromPool(Buffer *& _buffer)
    {
        VG_ASSERT(nullptr != _buffer);
        for (uint i = 0; i < m_sharedBuffers.size(); ++i)
        {
            auto & slot = m_sharedBuffers[i];
            if (slot.buffer == _buffer)
            {
                slot.used = false;
                _buffer = nullptr;
                return;
            }
        }
        VG_ASSERT(false, "Could not release Buffer \"%s\" from pool", _buffer->GetName());
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::gatherNodes(const UserPassInfoNode & _node, vector<UserPassInfoNode> & _nodes, u64 & _totalEstimatedCost)
    {
        for (auto & child : _node.m_children)
            gatherNodes(child, _nodes, _totalEstimatedCost);

        // Do not add empty nodes without renderpass
        if (nullptr != _node.m_renderPass)
        {
            _nodes.push_back(_node);
            _totalEstimatedCost += _node.getCostEstimate(); 
        }
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::gatherResources(const UserPassInfoNode & _node)
    {
        for (auto & child : _node.m_children)
            gatherResources(child);

        if (!_node.m_renderPass)
            return;

        RenderPass * renderPass = _node.m_renderPass;
        const auto & subPasses = renderPass->getSubPasses();

        for (uint i = 0; i < subPasses.size(); ++i)
        {
            SubPass * subPass = subPasses[i];
            const auto userPassInfo = subPass->getUserPassesInfos()[0];

            const UserPass * userPass = userPassInfo.m_userPass;

            auto & rwTextures = userPass->getRWTextures();
            for (uint i = 0; i < rwTextures.size(); ++i)
            {
                FrameGraphTextureResource * res = rwTextures[i];
                const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                if (textureResourceDesc.transient)
                {
                    if (res->isFirstWrite(userPass))
                    {
                        Texture * tex = createRWTextureFromPool(res->getName(), textureResourceDesc, i);
                        res->setTexture(tex);
                    }
                }
            }

            auto & rwBuffers = userPass->getRWBuffers();
            for (uint i = 0; i < rwBuffers.size(); ++i)
            {
                FrameGraphBufferResource * res = rwBuffers[i];
                const FrameGraphBufferResourceDesc & bufferResourceDesc = res->getBufferResourceDesc();

                if (bufferResourceDesc.transient)
                {
                    if (res->isFirstWrite(userPass))
                    {
                        Buffer * buffer = createRWBufferFromPool(res->getName(), bufferResourceDesc, i);
                        res->setBuffer(buffer);
                    }
                }
            }

            auto & renderTargets = userPass->getRenderTargets();
            for (uint i = 0; i < renderTargets.size(); ++i)
            {
                FrameGraphTextureResource * res = renderTargets[i];
                const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                if (textureResourceDesc.transient)
                {
                    if (res->isFirstWrite(userPass))
                    {
                        Texture * tex = createRenderTargetFromPool(res->getName(), textureResourceDesc, i);
                        res->setTexture(tex);
                    }
                }
            }


            FrameGraphTextureResource * depthStencil = userPass->getDepthStencil();
            if (depthStencil)
            {
                FrameGraphTextureResource * res = depthStencil;
                const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                if (textureResourceDesc.transient)
                {
                    if (res->isFirstWrite(userPass))
                    {
                        Texture * tex = createDepthStencilFromPool(res->getName(), textureResourceDesc, i);
                        res->setTexture(tex);
                    }
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::prepareNode(const UserPassInfoNode & _node)
    {
        for (auto & child : _node.m_children)
            prepareNode(child);

        if (!_node.m_renderPass)
            return;

        // TODO: refactor as we always have only one SubPass per UserPass?
        const auto & subPasses = _node.m_renderPass->getSubPasses();
        VG_ASSERT(subPasses.size() == 1);

        for (uint i = 0; i < subPasses.size(); ++i)
        {
            SubPass * subPass = subPasses[i];
            const auto & userPassInfo = subPass->getUserPassesInfos()[0];
            userPassInfo.m_userPass->Prepare(userPassInfo.m_renderContext);
        }
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::RenderNode(gfx::CommandList * _cmdList, const UserPassInfoNode * _node)
    {
        renderNode(*_node, _cmdList, false);
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::renderNode(const UserPassInfoNode & _node, gfx::CommandList * cmdList, bool _recur)
    {
        VG_PROFILE_GPU(_node.m_name.c_str());

        if (_recur)
        {
            // jobs disabled, render child jobs
            for (auto & child : _node.m_children)
                renderNode(child, cmdList, true);
        }

        if (!_node.m_renderPass)
            return;

        RenderPass * renderPass = _node.m_renderPass;
        const auto & subPasses = renderPass->getSubPasses();

        // Before + Render + After
        {
            // TODO : Refactor this to account that we only have one SubPass per RenderPass or make it work? (Not really relevant on Desktop, though)
            VG_ASSERT(subPasses.size() == 1);
            SubPass * subPass = subPasses[0];
            const auto & userPassInfo = subPass->getUserPassesInfos()[0];
            
            // Before
            {
                //VG_PROFILE_GPU("Before");

                for (uint i = 0; i < subPasses.size(); ++i)
                {
                    const auto & userPassInfo = subPass->getUserPassesInfos()[0];
                    userPassInfo.m_userPass->BeforeRender(userPassInfo.m_renderContext, cmdList);
                }
            }

            // Render
            {
                //VG_PROFILE_GPU("Render");

                cmdList->beginRenderPass(renderPass);
                for (uint i = 0; i < subPasses.size(); ++i)
                {
                    cmdList->beginSubPass(i, subPass);
                    {
                        VG_ASSERT(isEnumValue(userPassInfo.m_userPass->getUserPassType()), "UserPass \"%s\" has invalid RenderPassType 0x%02X. Valid values are Graphic (0), Compute (1), and Raytrace (2)", userPassInfo.m_userPass->GetName().c_str(), userPassInfo.m_userPass->getUserPassType());
                        userPassInfo.m_userPass->Render(userPassInfo.m_renderContext, cmdList);
                    }
                    cmdList->endSubPass();
                }
                cmdList->endRenderPass();
            }

            // After
            {
                //VG_PROFILE_GPU("After");

                for (uint i = 0; i < subPasses.size(); ++i)
                {
                    const auto & userPassInfo = subPass->getUserPassesInfos()[0];
                    userPassInfo.m_userPass->AfterRender(userPassInfo.m_renderContext, cmdList);
                }
            }
        }

        // release all transient resources that are read or write for the last time during this pass
        for (uint i = 0; i < subPasses.size(); ++i)
        {
            SubPass * subPass = subPasses[i];
            const UserPass * userPass = subPass->getUserPassesInfos()[0].m_userPass;
            auto & texturesRead = userPass->getTexturesRead();

            for (uint i = 0; i < texturesRead.size(); ++i)
            {
                FrameGraphTextureResource * res = texturesRead[i];
                const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                const auto & readWrites = res->getReadWriteAccess();
                const bool isLastReadOrWrite = readWrites.size() > 0 && readWrites[readWrites.size() - 1].m_userPass == userPass;

                if (isLastReadOrWrite)
                {
                    // Make sure RenderTarget is set in the 'RenderTarget' state after use by FrameGraph so that's in a predictable state for the next frame
                    auto current = res->getCurrentState();
                    if (ResourceState::RenderTarget != current)
                    {
                        cmdList->transitionResource(res->getTexture(), current, ResourceState::RenderTarget);
                        res->setCurrentState(ResourceState::RenderTarget);
                    }

                    if (textureResourceDesc.transient)
                    {
                        Texture * tex = res->getTexture();
                        releaseTextureFromPool(tex);
                        res->resetTexture();
                    }
                }
            }

            FrameGraphTextureResource * depthStencil = userPass->getDepthStencil();
            if (depthStencil)
            {
                FrameGraphTextureResource * res = depthStencil;
                const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                const auto & readWrites = res->getReadWriteAccess();
                const bool isLastReadOrWrite = readWrites.size() > 0 && readWrites[readWrites.size() - 1].m_userPass == userPass;

                if (isLastReadOrWrite)
                {
                    // Make sure DepthStencil is released in the 'RenderTarget' state
                    const auto current = res->getCurrentState();
                    if (ResourceState::RenderTarget != current)
                        VG_ERROR_ONCE("[FrameGraph] DepthStencil \"%s\" is released in the '%s' state. DepthStencil should be released in the 'ShaderResource' state", res->getName().c_str(), asString(res->getCurrentState()).c_str());

                    if (textureResourceDesc.transient)
                    {
                        Texture * tex = res->getTexture();
                        releaseTextureFromPool(tex);
                        res->resetTexture();
                    }
                }
            }

            auto & rwTextures = userPass->getRWTextures();

            for (uint i = 0; i < rwTextures.size(); ++i)
            {
                FrameGraphTextureResource * res = rwTextures[i];
                const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                const auto & readWrites = res->getReadWriteAccess();
                const bool isLastReadOrWrite = readWrites.size() > 0 && readWrites[readWrites.size() - 1].m_userPass == userPass;

                if (isLastReadOrWrite)
                {
                    // Make sure UAV is released in 'UnorderedAccess' state
                    auto current = res->getCurrentState();
                    if (ResourceState::UnorderedAccess != current)
                    {
                        cmdList->transitionResource(res->getTexture(), current, ResourceState::UnorderedAccess);
                        res->setCurrentState(ResourceState::UnorderedAccess);
                    }

                    if (textureResourceDesc.transient)
                    {
                        Texture * tex = res->getTexture();
                        releaseTextureFromPool(tex);
                        res->resetTexture();
                    }
                }
            }

            auto & rwBuffers = userPass->getRWBuffers();

            for (uint i = 0; i < rwBuffers.size(); ++i)
            {
                FrameGraphBufferResource * res = rwBuffers[i];
                const FrameGraphBufferResourceDesc & bufferResourceDesc = res->getBufferResourceDesc();

                const auto & readWrites = res->getReadWriteAccess();
                const bool isLastReadOrWrite = readWrites.size() > 0 && readWrites[readWrites.size() - 1].m_userPass == userPass;

                if (isLastReadOrWrite)
                {
                    // Make sure UAV is released in 'UnorderedAccess' state
                    auto current = res->getCurrentState();
                    if (ResourceState::UnorderedAccess != current)
                    {
                        cmdList->transitionResource(res->getBuffer(), current, ResourceState::UnorderedAccess);
                        res->setCurrentState(ResourceState::UnorderedAccess);
                    }

                    if (bufferResourceDesc.transient)
                    {
                        Buffer * buffer = res->getBuffer();
                        releaseBufferFromPool(buffer);
                        res->resetBuffer();
                    }
                }
            }

        }
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::render()
	{
		// Temp: use graphics command list
		Device * device = Device::get();
        const auto & cmdLists = device->getCommandLists(CommandListType::Graphics);

        // Split events by view
        const auto maxRenderJobCount = device->getMaxRenderJobCount();

        // Alloc render jobs if not yet created
        if (m_renderJobs.size() != maxRenderJobCount)
        {
            m_renderJobs.reserve(maxRenderJobCount);

            for (uint i = (uint)m_renderJobs.size(); i < maxRenderJobCount; ++i)
                m_renderJobs.push_back(new gfx::RenderJob("RenderJob", i, this));

            for (uint i = maxRenderJobCount; i < (uint)m_renderJobs.size(); ++i)
                VG_SAFE_RELEASE(m_renderJobs[i]);

            m_renderJobs.resize(maxRenderJobCount);
        }

        // Prepare nodes for rendering by determining the textures/buffers than will need to be allocated or reused
        for (auto & node : m_userPassInfoTree.m_children)
            gatherResources(node);

        // TODO: find a better name than "Prepare"?
        {
            VG_PROFILE_CPU("Prepare");
            for (auto & node : m_userPassInfoTree.m_children)
                prepareNode(node);
        }

        if (maxRenderJobCount > 0)
        {
            // List all nodes
            vector<UserPassInfoNode> nodes;
            u64 totalEstimatedCost = 0;
            for (auto & node : m_userPassInfoTree.m_children)
                gatherNodes(node, nodes, totalEstimatedCost);

            // temp: use default cmdlist
            CommandList * defaultCmdList = cmdLists[0];

            // TODO: estimate node cost to dispatch jobs
            const uint nodesPerJob = ((uint)nodes.size() + maxRenderJobCount - 1) / maxRenderJobCount;
           
            if (device->getRenderJobsMainThreadOnly())
            {
                // Render all nodes using several command list, but still on main thread (for debug purpose)
                uint cmdListIndex = 0;
                uint nodeCount = 0;
                for (uint i = 0; i < nodes.size(); ++i)
                {
                    CommandList * cmdList = cmdLists[cmdListIndex];

                    VG_PROFILE_GPU_CONTEXT(cmdList);
                    renderNode(nodes[i], cmdList, false);

                    if (++nodeCount > nodesPerJob)
                    {
                        cmdListIndex++;
                        nodeCount = 0;
                    }
                }

                device->setExecuteCommandListCount(gfx::CommandListType::Graphics, cmdListIndex + 1);
            }
            else
            {
                // Use up to 1 job per worker thread to run RenderJobs

                // Reset all jobs
                {
                    VG_PROFILE_CPU("Reset RenderJobs");
                    for (uint i = 0; i < m_renderJobs.size(); ++i)
                        m_renderJobs[i]->reset(cmdLists[i]);
                }

                uint jobCount;

                // Assign nodes to render jobs, sequentially
                {
                    VG_PROFILE_CPU("Dispatch RenderJobs");

                    const uint nodeCount = (uint)nodes.size();
                    jobCount = min((uint)m_renderJobs.size(), nodeCount); // Can't have more jobs than nodes to render

                    if (nodeCount <= jobCount)
                    {
                        for (uint n = 0; n < nodeCount; ++n)
                        {
                            auto & renderJob = m_renderJobs[n];
                            const UserPassInfoNode * node = &nodes[n];
                            renderJob->add(node);
                        }
                    }
                    else
                    {
                        #if 1

                        // Split based only on node count (doesn't account node estimated cost)
                        const uint minNodeCount = nodeCount / jobCount;
                        const uint maxNodeCount = minNodeCount + 1;
                        const uint limit = nodeCount % jobCount;

                        uint nodeBaseIndex = 0;
                        for (uint i = 0; i < jobCount; ++i)
                        {
                            auto & renderJob = m_renderJobs[i];

                            const uint jobNodeCount = i < limit ? maxNodeCount : minNodeCount;
                            for (uint n = 0; n < jobNodeCount; ++n)
                            {
                                const UserPassInfoNode * node = &nodes[nodeBaseIndex + n];
                                renderJob->add(node);
                            }
                            nodeBaseIndex += jobNodeCount;
                        }
                        #else

                        // Try to minimize sum of estimated cost per job
                        {
                           
                        }

                        #endif
                    }

                    device->setExecuteCommandListCount(gfx::CommandListType::Graphics, jobCount);
                }

                core::JobSync renderJobSync;
                core::IScheduler * jobScheduler = Kernel::getScheduler();

                // Kick jobs ...
                {
                    VG_PROFILE_CPU("Start RenderJobs");
                    for (uint i = 0; i < jobCount; ++i)
                        jobScheduler->Start(m_renderJobs[i], &renderJobSync);
                }

                // ... and wait for completion
                {
                    VG_PROFILE_CPU("Wait RenderJobs");
                    jobScheduler->Wait(&renderJobSync);
                }
            }
        }
        else
        {
            // render all nodes sequentially using default command list
            {
                CommandList * defaultCmdList = cmdLists[0];

                VG_PROFILE_GPU_CONTEXT(defaultCmdList);
                VG_PROFILE_GPU("Render");

                for (auto & node : m_userPassInfoTree.m_children)
                    renderNode(node, defaultCmdList, true);

                device->setExecuteCommandListCount(gfx::CommandListType::Graphics, 1);
            }
        }

        // All textures and buffers remaining in pool should be marked as 'unused' at this point
        for (uint i = 0; i < m_sharedTextures.size(); ++i)
        {
            auto & slot = m_sharedTextures[i];
            if (slot.used)
            {
                Texture * tex = slot.tex;
                VG_WARNING("[FrameGraph] Texture \"%s\" (%u) was not released", tex->GetName().c_str(), i);
                releaseTextureFromPool(tex);
            }
        }
        for (uint i = 0; i < m_sharedBuffers.size(); ++i)
        {
            auto & slot = m_sharedBuffers[i];
            if (slot.used)
            {
                Buffer * buffer = slot.buffer;
                VG_WARNING("[FrameGraph] Buffer \"%s\" (%u) was not released", buffer->GetName().c_str(), i);
                releaseBufferFromPool(buffer);
            }
        }

        cleanup();

        if (m_resized)
        {
            destroyTransientResources();
            m_resized = false;
        }        
	}

    //--------------------------------------------------------------------------------------
    void FrameGraph::setResized()
    {
        m_resized = true;
    }
}