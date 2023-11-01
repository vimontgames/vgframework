#include "gfx/Precomp.h"
#include "FrameGraph.h"
#include "gfx/Device/Device.h"
#include "gfx/CommandList/CommandList.h"
#include "gfx/Resource/Texture.h"
#include "gfx/FrameGraph/RenderPass.h"
#include "gfx/FrameGraph/SubPass.h"
#include "gfx/FrameGraph/UserPass.h"
#include "gfx/Profiler/Profiler.h"
#include "gfx/IView.h"

using namespace vg::core;

#include "RenderPass.hpp"
#include "SubPass.hpp"
#include "UserPass.hpp"

namespace vg::gfx
{
	//--------------------------------------------------------------------------------------
	// FrameGraph::Resource
	//--------------------------------------------------------------------------------------
	FrameGraph::Resource::Resource()
	{

	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::Resource::setType(Resource::Type _type)
	{
		m_type = _type;
	}

	//--------------------------------------------------------------------------------------
	FrameGraph::Resource::Type FrameGraph::Resource::getType() const
	{ 
		return m_type; 
	}

    //--------------------------------------------------------------------------------------
    void FrameGraph::Resource::setName(const core::string & _name)
    {
        m_name = _name;
    }

    //--------------------------------------------------------------------------------------
    const core::string & FrameGraph::Resource::getName() const
    {
        return m_name;
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::Resource::setReadAtPass(const UserPass * _subPass)
	{
		m_read.push_back(_subPass);
        m_readWrite.push_back(PassRWAccess(_subPass, RWAccess::Read));
	}

	//--------------------------------------------------------------------------------------
	const core::vector<const UserPass*> & FrameGraph::Resource::getReadAtPass() const
	{
		return m_read;
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::Resource::setWriteAtPass(const UserPass * _subPass)
	{
		m_write.push_back(_subPass);
        m_readWrite.push_back(PassRWAccess(_subPass, RWAccess::Write));
	}

    //--------------------------------------------------------------------------------------
    void FrameGraph::Resource::setReadWriteAtPass(const UserPass * _subPass)
    {
        m_read.push_back(_subPass);
        m_write.push_back(_subPass);
        m_readWrite.push_back(PassRWAccess(_subPass, RWAccess::Read | RWAccess::Write));
    }

	//--------------------------------------------------------------------------------------
	const core::vector<const UserPass*> & FrameGraph::Resource::getWriteAtPass() const
	{
		return m_write;
	}

    //--------------------------------------------------------------------------------------
    const core::vector<FrameGraph::Resource::PassRWAccess> & FrameGraph::Resource::getReadWriteAccess() const
    {
        return m_readWrite;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::Resource::setCurrentState(ResourceState _state)
    {
        m_state = _state;
    }

    //--------------------------------------------------------------------------------------
    ResourceState FrameGraph::Resource::getCurrentState() const
    {
        return m_state;
    }

	//--------------------------------------------------------------------------------------
	// FrameGraph::TextureResource
	//--------------------------------------------------------------------------------------
	void FrameGraph::TextureResource::setTextureResourceDesc(const FrameGraph::TextureResourceDesc & _texResDesc)
	{
		m_desc = _texResDesc;
	}

    //--------------------------------------------------------------------------------------
    void FrameGraph::TextureResource::setTexture(Texture * _tex)
    {
        VG_ASSERT(nullptr == m_texture, "Resource \"%s\" already has a texture", getName().c_str());
        m_texture = _tex;
    }

    //--------------------------------------------------------------------------------------
    void FrameGraph::TextureResource::resetTexture()
    {
        VG_ASSERT(m_desc.transient);
        m_texture = nullptr;
    }

    //--------------------------------------------------------------------------------------
    const FrameGraph::TextureResourceDesc & FrameGraph::TextureResource::getTextureResourceDesc() const
    {
        return m_desc;
    }
    
	//--------------------------------------------------------------------------------------
	Texture * FrameGraph::TextureResource::getTexture() const
	{
		VG_ASSERT(nullptr != m_texture, "Resource \"%s\" has not texture", getName().c_str());
		return m_texture;
	}

	//--------------------------------------------------------------------------------------
	// FrameGraph::BufferResource
	//--------------------------------------------------------------------------------------
	void FrameGraph::BufferResource::setBufferResourceDesc(const FrameGraph::BufferResourceDesc & _bufResDesc, Buffer * _buffer)
	{
		m_desc = _bufResDesc;
        m_buffer = _buffer;
	}

    //--------------------------------------------------------------------------------------
    const FrameGraph::BufferResourceDesc & FrameGraph::BufferResource::getBufferResourceDesc() const
    {
        return m_desc;
    }

    //--------------------------------------------------------------------------------------
    Buffer * FrameGraph::BufferResource::getBuffer() const
    {
        return m_buffer;
    }

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
        }
        else
        {
            for (SharedTexture & shared : m_sharedTextures)
                VG_SAFE_RELEASE_ASYNC(shared.tex);
        }
        m_sharedTextures.clear();
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
	void FrameGraph::importRenderTarget(const ResourceID & _resID, Texture * _tex, float4 _clearColor, FrameGraph::Resource::InitState _initState)
	{
        const TextureDesc & texDesc = _tex->getTexDesc(); 

        FrameGraph::TextureResourceDesc desc;
                                        desc.width = texDesc.width;
                                        desc.height = texDesc.height;
                                        desc.format = texDesc.format;
                                        desc.clearColor = _clearColor;
                                        desc.initState = _initState;

		TextureResource * res = addTextureResource(_resID, desc, _tex);
        VG_ASSERT(res);
	}

    //--------------------------------------------------------------------------------------
    FrameGraph::TextureResource * FrameGraph::addTextureResource(const ResourceID & _resID, const TextureResourceDesc & _texResDesc, Texture * _tex)
    {
        TextureResource * texRes = getResource<TextureResource>(Resource::Type::Texture, _resID, false);
        if (texRes)
        {
            texRes->setTextureResourceDesc(_texResDesc);
            texRes->setTexture(_tex);
        }
        return texRes;
    }

    //--------------------------------------------------------------------------------------
    FrameGraph::BufferResource * FrameGraph::addBufferResource(const ResourceID & _resID, const BufferResourceDesc & _bufResDesc, Buffer * _buf)
    {
        BufferResource * bufRes = getResource<BufferResource>(Resource::Type::Buffer, _resID, false);
        if (bufRes)
            bufRes->setBufferResourceDesc(_bufResDesc, _buf);
        return bufRes;
    }

    //--------------------------------------------------------------------------------------
    FrameGraph::TextureResource * FrameGraph::getTextureResource(const ResourceID & _resID) const
    {
        return const_cast<FrameGraph*>(this)->getResource<TextureResource>(Resource::Type::Texture, _resID, true);
    }

    //--------------------------------------------------------------------------------------
    FrameGraph::BufferResource * FrameGraph::getBufferResource(const ResourceID & _resID) const
    {
        return const_cast<FrameGraph*>(this)->getResource<BufferResource>(Resource::Type::Buffer, _resID, true);
    }

    //--------------------------------------------------------------------------------------
    template <class T> T * FrameGraph::getResource(Resource::Type _type, const ResourceID & _resID, bool _mustExist)
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
            VG_ASSERT(_type == it->second->getType());
            return static_cast<T*>(it->second);
        }

        return nullptr;
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::setGraphOutput(const ResourceID & _destTexResID)
	{
		m_outputResID = _destTexResID;	
        m_outputRes = getTextureResource(m_outputResID);
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::findDependencies(const UserPass & _renderPassDesc, uint _depth)
	{
		//TODO
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::allocateResources()
	{
		//for (auto * subPass : m_userPassStack)
		//{
        //    // in
        //    for (FrameGraph::TextureResource * tex : subPass->m_textures)
        //    {
        //        const auto & desc = tex->getTextureResourceDesc();
        //        if (desc.transient)
        //        {
        //            const auto & reads = tex->getReadAtPass();
        //            if (reads[0] == subPass)
        //            {
        //                VG_DEBUGPRINT("Transient TextureResource \"%s\" first read during pass \"%s\"\n", tex->getName(), subPass->getName());
        //            }
        //            if (reads[reads.size() - 1] == subPass)
        //            {
        //                VG_DEBUGPRINT("Transient TextureResource \"%s\" last read during pass \"%s\"\n", tex->getName(), subPass->getName());
        //            }
        //        }
        //    }
        //
        //    // out
        //    for (FrameGraph::TextureResource * renderTarget : subPass->m_renderTarget)
        //    {
        //        const auto & desc = renderTarget->getTextureResourceDesc();
        //        if (desc.transient)
        //        {
        //            const auto & writes = renderTarget->getWriteAtPass();
        //            if (writes[0] == subPass)
        //            {
        //                VG_DEBUGPRINT("Transient TextureResource \"%s\" first write during pass \"%s\"\n", renderTarget->getName(), subPass->getName());
        //            } 
        //            if (writes[writes.size()-1] == subPass)
        //            {
        //                VG_DEBUGPRINT("Transient TextureResource \"%s\" last write during pass \"%s\"\n", renderTarget->getName(), subPass->getName());
        //            }
        //        }
        //    }
		//}
	}

    //--------------------------------------------------------------------------------------
    bool FrameGraph::addUserPass(const RenderPassContext & _renderContext, UserPass * _userPass, const UserPassID & _renderPassID)
    {
        _userPass->addRef();
        _userPass->setName(_renderPassID);
        _userPass->setFrameGraph(this);

        m_userPassInfo.push_back({_renderContext, _userPass});
        return true;
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::setup(double _dt)
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

		//auto itBackbuffer = m_resources.find(m_outputResID);
		//VG_ASSERT(m_resources.end() != itBackbuffer, "FrameGraph destination resource not found");
        //
		//Resource * backbufferRes = itBackbuffer->second;
		//const auto backbufferWrites = backbufferRes->getWriteAtPass();
		//VG_ASSERT(!backbufferWrites.empty(), "No RenderPass is writing to the FrameGraph destination");
        //
		//m_userPassStack.clear();
        //
		//// Add all passes that write to the destination to the stack
		//for (const UserPass * subPass : backbufferWrites)
		//	m_userPassStack.push_back(subPass);
        //
		//auto tempStack = m_userPassStack;
        //
		//for (const UserPass * subPass : tempStack)
		//	findDependencies(*subPass, 0);

		// Reverse & remove duplicates
		//reverseAndRemoveDuplicates();

		// TODO: remove useless passes (output is unused)

		// TODO: reorder passes

		// TODO: allocate resources
		allocateResources();

		// TODO: merge adjacent passes when possible

		// TODO: make resources required in only one RenderPass transient
		for (auto * renderPass : m_renderPasses)
			VG_SAFE_RELEASE(renderPass);
		m_renderPasses.clear();

        Device * device = Device::get();

		// TEMP: one render pass with one subpass for each userPass (TODO: make pools)
		for (auto & userPassInfo : m_userPassInfo)
		{
            const auto * userPass = userPassInfo.m_userPass;

            core::vector<TextureResource*> colorAttachments;

            RenderPassKey renderPassKey;
                          renderPassKey.m_subPassCount = 1;
			
			// build all list with attachments from all passes
			// for (...)
			{
                auto & renderTargets = userPass->getRenderTargets();
				for (uint i = 0; i < renderTargets.size(); ++i)
				{
                    TextureResource * res = renderTargets[i];
                    const TextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                    renderPassKey.m_colorFormat[i] = textureResourceDesc.format;

                    // add or get index of attachment
                    uint attachmentIndex = (uint)-1;
                    for (uint i = 0; i < colorAttachments.size(); ++i)
                    {
                        if (colorAttachments[i]->getTextureResourceDesc() == textureResourceDesc)
                            attachmentIndex = i;
                    }

                    if ((uint)-1 == attachmentIndex)
                    {
                        VG_ASSERT(textureResourceDesc.transient || res->getTexture());
                        res->setTextureResourceDesc(textureResourceDesc);
                        colorAttachments.push_back(res);
                        attachmentIndex = uint(colorAttachments.size() - 1);
                    }

                    SubPassKey::AttachmentFlags flags = SubPassKey::AttachmentFlags::RenderTarget;

                    ResourceState begin = res->getCurrentState();;
                    ResourceState end = ResourceState::RenderTarget;
                    
                    const auto & reads      = res->getReadAtPass();
                    const bool firstRead    = reads.size() > 0 && reads[0] == userPass;
                    const bool lastRead     = reads.size() > 0 && reads[reads.size() - 1] == userPass;

                    const auto & writes     = res->getWriteAtPass();
                    const bool firstWrite   = writes.size() > 0 && writes[0] == userPass;
                    const bool lastWrite    = writes.size() > 0 && writes[writes.size() - 1] == userPass;

                    const bool backbuffer = m_outputRes == res;

                    // first pass writing to RT ? 
                    //if (firstWrite)
                    //{
                    //    flags |= SubPassKey::AttachmentFlags::Clear;
                    //
                    //    if (!backbuffer)
                    //        begin = ResourceState::ShaderResource;
                    //}
                    //else 
                    //    flags |= SubPassKey::AttachmentFlags::Preserve;

                    // first pass writing to RT/backbuffer ?
                    if (firstWrite)
                    {
                        flags |= SubPassKey::AttachmentFlags::Clear;
                    
                        if (backbuffer)
                            begin = ResourceState::Undefined;
                        else
                        {
                            #ifdef VG_DX12
                            flags |= SubPassKey::AttachmentFlags::MakeWritable; // dx12-only, used to transition to 'RenderTarget' state before 1st use
                            #endif

                            #ifdef VG_VULKAN
                            begin = ResourceState::Undefined; //Why not 'RenderTarget'? Because Vulkan.
                            #elif defined(VG_DX12)
                            begin = ResourceState::RenderTarget;
                            #else
                            VG_ASSERT_NOT_IMPLEMENTED();
                            #endif
                            }
                    }
                    else
                        flags |= SubPassKey::AttachmentFlags::Preserve;

                    // last pass writing to backbuffer ?
                    if (lastWrite)
                    {
                        if (backbuffer)
                            flags |= SubPassKey::AttachmentFlags::Present;
                        else
                            end = ResourceState::ShaderResource;
                    }

                    SubPassKey::AttachmentInfo info;
                                               info.flags = flags;
                                               info.begin = begin;
                                               info.end = end;

                    renderPassKey.m_subPassKeys[0].setColorAttachmentInfo(attachmentIndex, info);

                    res->setCurrentState(info.end);
				}
			}

            TextureResource * depthStencilAttachment = nullptr;
            FrameGraph::TextureResource * depthStencilRes = userPass->getDepthStencil();
            if (depthStencilRes)
            {
                //renderPassKey.m_depthStencilFormat = depthStencilRes->getTextureResourceDesc().format;
                //renderPassKey.m_subPassKeys[0].setDepthStencilFlags(SubPassKey::Flags::RenderTarget);

                TextureResource * res = userPass->getDepthStencil();

                const TextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                renderPassKey.m_depthStencilFormat = textureResourceDesc.format;

                SubPassKey::AttachmentFlags flags = SubPassKey::AttachmentFlags::RenderTarget;

                ResourceState begin = res->getCurrentState();;
                ResourceState end = ResourceState::RenderTarget;

                const auto & reads = res->getReadAtPass();
                const bool firstRead = reads.size() > 0 && reads[0] == userPass;
                const bool lastRead = reads.size() > 0 && reads[reads.size() - 1] == userPass;

                const auto & writes = res->getWriteAtPass();
                const bool firstWrite = writes.size() > 0 && writes[0] == userPass;
                const bool lastWrite = writes.size() > 0 && writes[writes.size() - 1] == userPass;

                 // first pass writing to RT ? 
                    if (firstWrite)
                        flags |= SubPassKey::AttachmentFlags::Clear;
                    else 
                        flags |= SubPassKey::AttachmentFlags::Preserve;

                    // first pass writing to backbuffer ?
                    if (firstWrite)
                    {
                        flags |= SubPassKey::AttachmentFlags::Clear;
                    
                        //if (backbuffer)
                        //    begin = ResourceState::Undefined;
                        //else
                            #ifdef VG_VULKAN
                            begin = ResourceState::Undefined;
                            #elif defined(VG_DX12)
                            begin = ResourceState::RenderTarget;
                            #else
                            VG_ASSERT_NOT_IMPLEMENTED();
                            #endif
                    }
                    else
                        flags |= SubPassKey::AttachmentFlags::Preserve;

                    // last pass writing to backbuffer ?
                    //if (lastWrite)
                    //{
                    //    if (backbuffer)
                    //        flags |= SubPassKey::AttachmentFlags::Present;
                    //}

                    SubPassKey::AttachmentInfo info;
                                               info.flags = flags;
                                               info.begin = begin;
                                               info.end = end;

                    renderPassKey.m_subPassKeys[0].setDepthStencilAttachmentInfo(info);

                    res->setCurrentState(info.end);

                    depthStencilAttachment = res;
            }

            RenderPass * renderPass = new RenderPass(userPass->getUserPassType(), renderPassKey);
            renderPass->m_colorAttachments = std::move(colorAttachments); // transient resources will be lazy allocated before the actuel RenderPass begins
            renderPass->m_depthStencilAttachment = depthStencilAttachment;

            SubPass * subPass = new SubPass();

			subPass->addUserPassInfo(userPassInfo);
			renderPass->addSubPass(subPass);
			m_renderPasses.push_back(renderPass);

			renderPass->finalize();
		}
	}

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createRenderTargetFromPool(const FrameGraph::TextureResourceDesc & _textureResourceDesc)
    {
        return createTextureFromPool(_textureResourceDesc, false, false);
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createDepthStencilFromPool(const FrameGraph::TextureResourceDesc & _textureResourceDesc)
    {
        return createTextureFromPool(_textureResourceDesc, true, false);
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createRWTextureFromPool(const TextureResourceDesc & _textureResourceDesc)
    {
        return createTextureFromPool(_textureResourceDesc, false, true);
    }

    //--------------------------------------------------------------------------------------
    Texture * FrameGraph::createTextureFromPool(const FrameGraph::TextureResourceDesc & _textureResourceDesc, bool _depthStencil, bool _uav)
    {
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
                    desc.flags = _depthStencil ? TextureFlags::DepthStencil : TextureFlags::RenderTarget;
                    
                    if (_uav)
                        desc.resource.m_bindFlags = BindFlags::ShaderResource | BindFlags::UnorderedAccess; // What about "UAV-only" textures?
                    else
                        desc.resource.m_bindFlags = BindFlags::ShaderResource;

                    desc.resource.m_cpuAccessFlags = CPUAccessFlags::None;
                    desc.resource.m_usage = Usage::Default;

        string name = "Temp#" + to_string(m_sharedTextures.size());
        if (asBool(BindFlags::ShaderResource & desc.resource.m_bindFlags))
            name += "_SRV";
        if (asBool(BindFlags::UnorderedAccess & desc.resource.m_bindFlags))
            name += "_UAV";
        
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
        VG_ASSERT(false, "Could not release texture \"%s\" from pool", _tex->getName());
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
                auto & renderTargets = userPass->getRenderTargets();
                auto & rwTextures = userPass->getRWTextures();
                auto & texturesRead = userPass->getTexturesRead();

                for (uint i = 0; i < renderTargets.size(); ++i)
                {
                    TextureResource * res = renderTargets[i];
                    const TextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                    if (textureResourceDesc.transient)
                    {
                        const auto & writes = res->getWriteAtPass();
                        if (writes.size() > 0 && writes[0] == userPass)
                        {
                            Texture * tex = createRenderTargetFromPool(textureResourceDesc);
                            res->setTexture(tex);
                        }
                    }
                }

                for (uint i = 0; i < rwTextures.size(); ++i)
                {
                    TextureResource * res = rwTextures[i];
                    const TextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                    if (textureResourceDesc.transient)
                    {
                        const auto & writes = res->getWriteAtPass();
                        if (writes.size() > 0 && writes[0] == userPass)
                        {
                            Texture * tex = createRWTextureFromPool(textureResourceDesc);
                            res->setTexture(tex);
                        }
                    }
                }

                TextureResource * depthStencil = userPass->getDepthStencil();
                if (depthStencil)
                {
                    TextureResource * res = depthStencil;
                    const TextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                    if (textureResourceDesc.transient)
                    {
                        const auto & writes = res->getWriteAtPass();
                        if (writes[0] == userPass)
                        {
                            Texture * tex = createDepthStencilFromPool(textureResourceDesc);
                            res->setTexture(tex);
                        }
                    }
                }
            }

            for (uint i = 0; i < subPasses.size(); ++i)
            {
                SubPass * subPass = subPasses[i];
                const auto & userPassInfo = subPass->getUserPassesInfos()[0];
                userPassInfo.m_userPass->BeforeRender(userPassInfo.m_renderContext, cmdList);
            }

			cmdList->beginRenderPass(renderPass);
			for (uint i = 0; i < subPasses.size(); ++i)
			{
				SubPass * subPass = subPasses[i];
                const auto & userPassInfo = subPass->getUserPassesInfos()[0];

                VG_PROFILE_GPU(userPassInfo.m_userPass->getName().c_str());

                cmdList->beginSubPass(i, subPass);
				{
                    VG_ASSERT(isEnumValue(userPassInfo.m_userPass->getUserPassType()), "UserPass \"%s\" has invalid RenderPassType 0x%02X. Valid values are Graphic (0), Compute (1), and Raytrace (2).", userPassInfo.m_userPass->getName().c_str(), userPassInfo.m_userPass->getUserPassType());
                    userPassInfo.m_userPass->Render(userPassInfo.m_renderContext, cmdList);
				}
                cmdList->endSubPass();
			}
			cmdList->endRenderPass();

            for (uint i = 0; i < subPasses.size(); ++i)
            {
                SubPass * subPass = subPasses[i];
                const auto & userPassInfo = subPass->getUserPassesInfos()[0];
                userPassInfo.m_userPass->AfterRender(userPassInfo.m_renderContext, cmdList);
            }
           
            // release all transient resources that are read or write for the last time during this pass
            for (uint i = 0; i < subPasses.size(); ++i)
            {
                SubPass * subPass = subPasses[i];
                const UserPass * userPass = subPass->getUserPassesInfos()[0].m_userPass;                
                auto & texturesRead = userPass->getTexturesRead();

                for (uint i = 0; i < texturesRead.size(); ++i)
                {
                    TextureResource * res = texturesRead[i];
                    const TextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                    if (textureResourceDesc.transient)
                    {
                        //const auto & reads = res->getReadAtPass();
                        //const auto & writes = res->getWriteAtPass();
                        //
                        //if (reads.size() > 0 && reads[reads.size() - 1] == userPass)
                        //{
                        //    Texture * tex = res->getTexture();
                        //    releaseTextureFromPool(tex);
                        //    res->resetTexture();
                        //}
                        //else if (writes.size() > 0 && writes[writes.size() - 1] == userPass)
                        //{
                        //    Texture * tex = res->getTexture();
                        //    releaseTextureFromPool(tex);
                        //    res->resetTexture();
                        //}

                        const auto & readWrites = res->getReadWriteAccess();

                        if (readWrites.size() > 0 && readWrites[readWrites.size() - 1].m_userPass == userPass)
                        {
                            // Last read or writes access
                            Texture * tex = res->getTexture();
                            releaseTextureFromPool(tex);
                            res->resetTexture();
                        }
                    }
                }

                TextureResource * depthStencil = userPass->getDepthStencil();
                if (depthStencil)
                {
                    TextureResource * res = depthStencil;
                    const TextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                    if (textureResourceDesc.transient)
                    {
                        const auto & reads = res->getReadAtPass();
                        const auto & writes = res->getWriteAtPass();

                        if (reads.size() > 0 && reads[reads.size() - 1] == userPass)
                        {
                            Texture * tex = res->getTexture();
                            releaseTextureFromPool(tex);
                            res->resetTexture();
                        }
                        else if (writes.size() > 0 && writes[writes.size() - 1] == userPass)
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
                    TextureResource * res = rwTextures[i];
                    const TextureResourceDesc & textureResourceDesc = res->getTextureResourceDesc();

                    if (textureResourceDesc.transient)
                    {
                        //const auto & reads = res->getReadAtPass();
                        //const auto & writes = res->getWriteAtPass();
                        //
                        //if (reads.size() > 0 && reads[reads.size() - 1] == userPass)
                        //{
                        //    Texture * tex = res->getTexture();
                        //    releaseTextureFromPool(tex);
                        //    res->resetTexture();
                        //}
                        //else if (writes.size() > 0 && writes[writes.size() - 1] == userPass)
                        //{
                        //    Texture * tex = res->getTexture();
                        //    releaseTextureFromPool(tex);
                        //    res->resetTexture();
                        //}

                        const auto & readWrites = res->getReadWriteAccess();

                        if (readWrites.size() > 0 && readWrites[readWrites.size() - 1].m_userPass == userPass)
                        {
                            // Last read or writes access
                            Texture * tex = res->getTexture();
                            releaseTextureFromPool(tex);
                            res->resetTexture();
                        }
                    }
                }
            }            
		}

        if (NULL != currentView)
            profiler->stopGpuEvent();

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