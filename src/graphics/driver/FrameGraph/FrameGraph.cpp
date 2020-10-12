#include "graphics/driver/Precomp.h"
#include "FrameGraph.h"
#include "graphics/driver/Device/Device.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/Resource/Texture.h"

#include "graphics/driver/FrameGraph/RenderPass.h"
#include "graphics/driver/FrameGraph/SubPass.h"
#include "graphics/driver/FrameGraph/UserPass.h"

#include "graphics/driver/Profiler/Profiler.h"

using namespace vg::core;

#include "RenderPass.hpp"
#include "SubPass.hpp"
#include "UserPass.hpp"

namespace vg::graphics::driver
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
		m_read.insert(_subPass);
	}

	//--------------------------------------------------------------------------------------
	const core::unordered_set<const UserPass*> & FrameGraph::Resource::getReadAtPass() const
	{
		return m_read;
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::Resource::setWriteAtPass(const UserPass * _subPass)
	{
		m_write.insert(_subPass);
	}

	//--------------------------------------------------------------------------------------
	const core::unordered_set<const UserPass*> & FrameGraph::Resource::getWriteAtPass() const
	{
		return m_write;
	}

	//--------------------------------------------------------------------------------------
	// FrameGraph::TextureResource
	//--------------------------------------------------------------------------------------
	void FrameGraph::TextureResource::setTextureResourceDesc(const FrameGraph::TextureResourceDesc & _texResDesc, const Texture * _tex)
	{
        VG_ASSERT(nullptr == m_texture, "Resource \"%s\" already has a texture", getName().c_str());
		m_desc = _texResDesc;
        m_texture = _tex;
	}

    //--------------------------------------------------------------------------------------
    const FrameGraph::TextureResourceDesc & FrameGraph::TextureResource::getTextureResourceDesc() const
    {
        return m_desc;
    }
    
	//--------------------------------------------------------------------------------------
	const Texture * FrameGraph::TextureResource::getTexture() const
	{
		VG_ASSERT(nullptr != m_texture, "Resource \"%s\" has not texture", getName().c_str());
		return m_texture;
	}

	//--------------------------------------------------------------------------------------
	// FrameGraph::BufferResource
	//--------------------------------------------------------------------------------------
	void FrameGraph::BufferResource::setBufferResourceDesc(const FrameGraph::BufferResourceDesc & _bufResDesc, const Buffer * _buffer)
	{
		m_desc = _bufResDesc;
        m_buffer = _buffer;
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
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::cleanup()
	{
		for (auto & pair : m_subPasses)
			VG_SAFE_RELEASE(pair.second);
		m_subPasses.clear();

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
    FrameGraph::TextureResource * FrameGraph::addTextureResource(const ResourceID & _resID, const TextureResourceDesc & _texResDesc, const Texture * _tex)
    {
        TextureResource * texRes = getResource<TextureResource>(Resource::Type::Texture, _resID, false);
        if (texRes)
            texRes->setTextureResourceDesc(_texResDesc, _tex);
        return texRes;
    }

    //--------------------------------------------------------------------------------------
    FrameGraph::BufferResource * FrameGraph::addBufferResource(const ResourceID & _resID, const BufferResourceDesc & _bufResDesc, const Buffer * _buf)
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

        return false;
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::setGraphOutput(const ResourceID & _destTexResID)
	{
		m_outputResID = _destTexResID;	
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::findDependencies(const UserPass & _renderPassDesc, uint _depth)
	{
		//TODO
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::reverseAndRemoveDuplicates()
	{
		core::vector<const UserPass*> stack;
		stack.reserve(m_userPassStack.size());

		if (m_userPassStack.size() > 0)
		{
			for (int i = (int)(m_userPassStack.size() - 1); i >= 0; --i)
			{
				const UserPass * subPass = m_userPassStack[i];

				bool found = false;
				for (uint j = 0; j < stack.size(); ++j)
				{
					if (stack[j] == subPass)
					{
						found = true;
						break;
					}
				}
				if (!found)
					stack.push_back(subPass);
			}
		}
		m_userPassStack = std::move(stack);
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::allocateResources()
	{
		for (auto * subPass : m_userPassStack)
		{
			for (auto * renderTarget : subPass->m_renderTarget)
			{

			}
		}
	}

    //--------------------------------------------------------------------------------------
    bool FrameGraph::addUserPass(UserPass * _userPass, const UserPassID & _renderPassID)
    {
        auto it = m_subPasses.find(_renderPassID);
        
        if (m_subPasses.end() == it)
        {
            _userPass->addRef();
            _userPass->setName(_renderPassID);
            _userPass->setFrameGraph(this);
            m_subPasses[_renderPassID] = _userPass;
            return true;
        }
        else
        {
            VG_ASSERT(m_subPasses.end() == it, "Could not add UserPass \"%s\" to FrameGraph as there's already an UserPass with the same identifier", _renderPassID);
            return false;
        }
    }

	//--------------------------------------------------------------------------------------
	void FrameGraph::setup()
	{
        VG_PROFILE_CPU("setup");

		for (auto & pair : m_subPasses)
		{
			UserPass * subPass = pair.second;
			subPass->reset();
			subPass->setup();
		}
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::build()
	{
        VG_PROFILE_CPU("build");

		auto itBackbuffer = m_resources.find(m_outputResID);
		VG_ASSERT(m_resources.end() != itBackbuffer, "FrameGraph destination resource not found");

		Resource * backbufferRes = itBackbuffer->second;
		const auto backbufferWrites = backbufferRes->getWriteAtPass();
		VG_ASSERT(!backbufferWrites.empty(), "No RenderPass is writing to the FrameGraph destination");

		m_userPassStack.clear();

		// Add all passes that write to the destination to the stack
		for (const UserPass * subPass : backbufferWrites)
			m_userPassStack.push_back(subPass);

		auto tempStack = m_userPassStack;

		for (const UserPass * subPass : tempStack)
			findDependencies(*subPass, 0);

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

		// TEMP: one render pass with one subpass for each userPass (TODO: make pools)
		for (auto * userPass : m_userPassStack)
		{
            core::vector<const driver::Texture*> attachments;

            RenderPassKey renderPassKey;
                          renderPassKey.m_subPassCount = 1;
			
			// build all list with attachments from all passes
			// for (...)
			{
                auto & renderTargets = userPass->getRenderTargets();
				for (uint i = 0; i < renderTargets.size(); ++i)
				{
                    FrameGraph::TextureResource * res = renderTargets[i];
                    auto * tex = res->getTexture();

                    renderPassKey.m_colorFormat[i] = res->getTextureResourceDesc().format;

                    // add or get index of attachment
                    uint attachmentIndex = (uint)-1;
                    for (uint i = 0; i < attachments.size(); ++i)
                    {
                        if (attachments[i] == tex)
                            attachmentIndex = i;
                    }

                    if ((uint)-1 == attachmentIndex)
                    {
                        attachments.push_back(tex);
                        attachmentIndex = uint(attachments.size() - 1);
                    }

                    SubPassKey::Flags flags = SubPassKey::Flags::Bind;

                    // first pass writing to RT ? 
                    const auto & writes = res->getWriteAtPass();
                    if (*writes.begin() == userPass)
                        flags |= SubPassKey::Flags::Clear | SubPassKey::Flags::Store;
                    else 
                        flags |= SubPassKey::Flags::Load;

                    renderPassKey.m_subPassKeys[0].setRenderTargetFlags(attachmentIndex, flags);
				}
			}

            FrameGraph::TextureResource * depthStencilRes = userPass->getDepthStencil();
            if (depthStencilRes)
            {
                renderPassKey.m_depthStencilFormat = depthStencilRes->getTextureResourceDesc().format;
                renderPassKey.m_subPassKeys[0].setDepthStencilFlags(SubPassKey::Bind);
            }

            RenderPass * renderPass = new RenderPass(renderPassKey);
            renderPass->m_attachments = std::move(attachments);

            SubPass * subPass = new SubPass();

			subPass->addUserPass(userPass);
			renderPass->addSubPass(subPass);
			m_renderPasses.push_back(renderPass);

			renderPass->finalize();
		}
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::render()
	{
        VG_PROFILE_CPU("render");

		// Temp: use graphics command list
		Device * device = Device::get();
		CommandList * cmdList = device->getCommandLists(CommandListType::Graphics)[0]; 

        VG_PROFILE_GPU_CONTEXT(cmdList);

		for (const RenderPass * renderPass : m_renderPasses)
		{
			cmdList->beginRenderPass(renderPass);
			const auto & subPasses = renderPass->getSubPasses();
			for (uint i = 0; i < subPasses.size(); ++i)
			{
				const SubPass * subPass = subPasses[i];
				cmdList->beginSubPass(i, subPass);
				{
                    for (const UserPass * userPass : subPass->getUserPasses())
                        userPass->draw(cmdList);
				}
                cmdList->endSubPass();
			}
			cmdList->endRenderPass();
		}

        cleanup();
	}
}