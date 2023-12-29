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
#include "gfx/IView.h"

using namespace vg::core;

#include "FrameGraphResource.hpp"
#include "RenderPass.hpp"
#include "SubPass.hpp"
#include "UserPass.hpp"

namespace vg::gfx
{
	//--------------------------------------------------------------------------------------
	// FrameGraph
	//--------------------------------------------------------------------------------------
	FrameGraph::FrameGraph()
	{

	}

	//--------------------------------------------------------------------------------------
	FrameGraph::~FrameGraph()
	{
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
        FrameGraphTextureResource * texRes = getResource<FrameGraphTextureResource>(FrameGraphResource::Type::Texture, _resID, false);
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
        FrameGraphBufferResource * bufRes = getResource<FrameGraphBufferResource>(FrameGraphResource::Type::Buffer, _resID, false);
        if (bufRes)
            bufRes->setBufferResourceDesc(_bufResDesc, _buf);
        return bufRes;
    }

    //--------------------------------------------------------------------------------------
    FrameGraphTextureResource * FrameGraph::getTextureResource(const FrameGraphResourceID & _resID) const
    {
        return const_cast<FrameGraph*>(this)->getResource<FrameGraphTextureResource>(FrameGraphResource::Type::Texture, _resID, true);
    }

    //--------------------------------------------------------------------------------------
    FrameGraphBufferResource * FrameGraph::getBufferResource(const FrameGraphResourceID & _resID) const
    {
        return const_cast<FrameGraph*>(this)->getResource<FrameGraphBufferResource>(FrameGraphResource::Type::Buffer, _resID, true);
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * FrameGraph::getResource(FrameGraphResource::Type _type, const FrameGraphResourceID & _resID, bool _mustExist)
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
    bool FrameGraph::addUserPass(const RenderPassContext & _renderContext, UserPass * _userPass, const FrameGraphUserPassID & _renderPassID)
    {
        VG_ASSERT(nullptr != _userPass, "Adding NULL UserPass to FrameGraph");

        if (nullptr != _userPass)
        {
            _userPass->addRef();
            _userPass->setName(_renderPassID);
            _userPass->setFrameGraph(this);

            m_userPassInfo.push_back({ _renderContext, _userPass });
            return true;
        }

        return false;
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::setup(float _dt)
	{
        VG_PROFILE_CPU("setup");

		for (auto & subPassInfo : m_userPassInfo)
		{
            subPassInfo.m_userPass->reset();
            subPassInfo.m_userPass->Setup(subPassInfo.m_renderContext, _dt);
		}
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

		// TEMP: one render pass with one subpass for each userPass (TODO: make pools)
		for (auto & userPassInfo : m_userPassInfo)
		{
            const auto * userPass = userPassInfo.m_userPass;

            core::vector<FrameGraphTextureResource *> colorAttachments;

            RenderPassKey renderPassKey;
                          renderPassKey.m_subPassCount = 1;

            vector<FrameGraphResourceTransitionDesc> resourceTransitions;
			
			// build all list with attachments from all passes
			// for (...)
			{
                auto & rwTextures = userPass->getRWTextures();
                for (uint i = 0; i < rwTextures.size(); ++i)
                {
                    FrameGraphTextureResource * res = rwTextures[i];
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
				for (uint i = 0; i < renderTargets.size(); ++i)
				{
                    FrameGraphTextureResource * res = renderTargets[i];
                    const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                    renderPassKey.m_colorFormat[i] = textureResourceDesc.format;

                    // add or get index of attachment
                    uint attachmentIndex = -1;

                    // what if the same render targets are reused but in a different order in a sub-pass?
                    //for (uint i = 0; i < colorAttachments.size(); ++i)
                    //{
                    //    if (colorAttachments[i]->getTextureResourceDesc() == textureResourceDesc)
                    //        attachmentIndex = i;
                    //}

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

            RenderPass * renderPass = new RenderPass(userPass->getUserPassType(), renderPassKey);
            renderPass->m_colorAttachments = std::move(colorAttachments); // transient resources will be lazy allocated before the actuel RenderPass begins
            renderPass->m_depthStencilAttachment = depthStencilAttachment;

            SubPass * subPass = new SubPass();

            userPassInfo.m_resourceTransitions = std::move(resourceTransitions);

			subPass->addUserPassInfo(userPassInfo);
			renderPass->addSubPass(subPass);
			m_renderPasses.push_back(renderPass);

			renderPass->finalize();
		}
	}

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createRenderTargetFromPool(const FrameGraphTextureResourceDesc & _textureResourceDesc)
    {
        return createTextureFromPool(_textureResourceDesc, true, false, false);
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createDepthStencilFromPool(const FrameGraphTextureResourceDesc & _textureResourceDesc)
    {
        return createTextureFromPool(_textureResourceDesc, false, true, false);
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createRWTextureFromPool(const FrameGraphTextureResourceDesc & _textureResourceDesc)
    {
        return createTextureFromPool(_textureResourceDesc, false, false, true);
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createTextureFromPool(const FrameGraphTextureResourceDesc & _textureResourceDesc, bool _renderTarget, bool _depthStencil, bool _uav)
    {
        VG_ASSERT(_depthStencil == Texture::isDepthStencilFormat(_textureResourceDesc.format));

        // Use settings from desc
        _uav = _textureResourceDesc.uav;

        for (uint i = 0; i < m_sharedTextures.size(); ++i)
        {
            auto & slot = m_sharedTextures[i];
            if (!slot.used)
            {
                if (slot.desc == _textureResourceDesc)
                {
                    slot.used = true;
                    VG_ASSERT(slot.tex);
                    return slot.tex;
                }
            }
        }

        // create if not found
        Device * device = Device::get();
       
        TextureDesc desc;
                    desc.format = _textureResourceDesc.format;
                    desc.width = _textureResourceDesc.width;
                    desc.height = _textureResourceDesc.height;

                    if (_renderTarget)
                        desc.flags = TextureFlags::RenderTarget;
                    else if (_depthStencil)
                        desc.flags = TextureFlags::DepthStencil;
                    else
                        desc.flags = (TextureFlags)0x0;
                    
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
        VG_ASSERT(false, "Could not release Texture \"%s\" from pool", _tex->getName());
    }

    //--------------------------------------------------------------------------------------
    Buffer * FrameGraph::createRWBufferFromPool(const FrameGraphBufferResourceDesc & _bufferResourceDesc)
    {
        return createBufferFromPool(_bufferResourceDesc, true);
    }

    //--------------------------------------------------------------------------------------
    Buffer * FrameGraph::createBufferFromPool(const FrameGraphBufferResourceDesc & _bufferResourceDesc, bool _uav)
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
        VG_ASSERT(false, "Could not release Buffer \"%s\" from pool", _buffer->getName());
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::render()
	{
		// Temp: use graphics command list
		Device * device = Device::get();
		CommandList * cmdList = device->getCommandLists(CommandListType::Graphics)[0]; 

        VG_PROFILE_GPU_CONTEXT(cmdList);
        VG_PROFILE_GPU("render");

        // Split events by view
        auto profiler = core::Kernel::getProfiler();
        IView * currentView = nullptr;

		for (uint j = 0; j < m_renderPasses.size(); ++j)
		{
            RenderPass * renderPass = m_renderPasses[j];
            const auto & subPasses = renderPass->getSubPasses();

            // allocate all transient resources that will be required during the subpasses before beginning the renderpass
            for (uint i = 0; i < subPasses.size(); ++i)
            {
                SubPass * subPass = subPasses[i];
                const auto userPassInfo = subPass->getUserPassesInfos()[0];

                if (currentView != userPassInfo.m_renderContext.m_view)
                {
                    if (NULL != currentView)
                        profiler->stopGpuEvent();

                    currentView = userPassInfo.m_renderContext.m_view;
                    profiler->startGpuEvent(currentView->getName().c_str());
                }

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
                            Texture * tex = createRWTextureFromPool(textureResourceDesc);
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
                            Buffer * buffer = createRWBufferFromPool(bufferResourceDesc);
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
                            Texture * tex = createRenderTargetFromPool(textureResourceDesc);
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
                            Texture * tex = createDepthStencilFromPool(textureResourceDesc);
                            res->setTexture(tex);
                        }
                    }
                }
            }

            // Before + Render + After
            {
                // TODO : Refactor this to account that we only have one SubPass per RenderPass or make it work? (Not really relevant on Desktop, though)
                VG_ASSERT(subPasses.size() == 1);
                SubPass * subPass = subPasses[0];
                const auto & userPassInfo = subPass->getUserPassesInfos()[0];
                VG_PROFILE_GPU(userPassInfo.m_userPass->getName().c_str());

                for (uint i = 0; i < subPasses.size(); ++i)
                {                   
                    const auto & userPassInfo = subPass->getUserPassesInfos()[0];
                    userPassInfo.m_userPass->BeforeRender(userPassInfo.m_renderContext, cmdList);
                }

                cmdList->beginRenderPass(renderPass);
                for (uint i = 0; i < subPasses.size(); ++i)
                {
                    cmdList->beginSubPass(i, subPass);
                    {
                        VG_ASSERT(isEnumValue(userPassInfo.m_userPass->getUserPassType()), "UserPass \"%s\" has invalid RenderPassType 0x%02X. Valid values are Graphic (0), Compute (1), and Raytrace (2)", userPassInfo.m_userPass->getName().c_str(), userPassInfo.m_userPass->getUserPassType());
                        userPassInfo.m_userPass->Render(userPassInfo.m_renderContext, cmdList);
                    }
                    cmdList->endSubPass();
                }
                cmdList->endRenderPass();

                for (uint i = 0; i < subPasses.size(); ++i)
                {
                    const auto & userPassInfo = subPass->getUserPassesInfos()[0];
                    userPassInfo.m_userPass->AfterRender(userPassInfo.m_renderContext, cmdList);
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

                //auto & renderTargets = userPass->getRenderTargets();
                //for (uint i = 0; i < renderTargets.size(); ++i)
                //{
                //    FrameGraphTextureResource * res = renderTargets[i];
                //    const FrameGraphTextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();
                //
                //    const auto & readWrites = res->getReadWriteAccess();
                //    const bool isLastReadOrWrite = readWrites.size() > 0 && readWrites[readWrites.size() - 1].m_userPass == userPass;
                //
                //    if (isLastReadOrWrite)
                //    {
                //        // Make sure RenderTarget is set in the 'RenderTarget' state after use by FrameGraph so that's in a predictable state for the next frame
                //        auto current = res->getCurrentState();
                //        if (ResourceState::RenderTarget != current)
                //        {
                //            cmdList->transitionResource(res->getTexture(), current, ResourceState::RenderTarget);
                //            res->setCurrentState(ResourceState::RenderTarget);
                //        }
                //
                //        if (textureResourceDesc.transient)
                //        {
                //            Texture * tex = res->getTexture();
                //            releaseTextureFromPool(tex);
                //            res->resetTexture();
                //        }
                //    }
                //}

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

        if (NULL != currentView)
            profiler->stopGpuEvent();

        // All textures and buffers remaining in pool should be marked as 'unused' at this point
        for (uint i = 0; i < m_sharedTextures.size(); ++i)
        {
            auto & slot = m_sharedTextures[i];
            if (slot.used)
            {
                Texture * tex = slot.tex;
                VG_WARNING("[FrameGraph] Texture \"%s\" (%u) was not released", tex->getName().c_str(), i);
                releaseTextureFromPool(tex);
            }
        }
        for (uint i = 0; i < m_sharedBuffers.size(); ++i)
        {
            auto & slot = m_sharedBuffers[i];
            if (slot.used)
            {
                Buffer * buffer = slot.buffer;
                VG_WARNING("[FrameGraph] Buffer \"%s\" (%u) was not released", buffer->getName().c_str(), i);
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