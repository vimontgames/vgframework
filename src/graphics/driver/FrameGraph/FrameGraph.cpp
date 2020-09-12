#include "graphics/driver/Precomp.h"
#include "FrameGraph.h"
#include "graphics/driver/Device/Device.h"
#include "graphics/driver/CommandList/CommandList.h"
#include "graphics/driver/Texture/Texture.h"

#include "graphics/driver/FrameGraph/RenderPass.h"
#include "graphics/driver/FrameGraph/SubPass.h"
#include "graphics/driver/FrameGraph/UserPass.h"

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
	void FrameGraph::TextureResource::setTextureDesc(const FrameGraph::TextureDesc & _resDesc)
	{
		m_desc = _resDesc;
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::TextureResource::setTextureUsage(TextureResource::Usage _usage)
	{
		m_usage |= _usage;
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::TextureResource::setTexture(const Texture * _texture)
	{
		VG_ASSERT(nullptr == m_texture, "Resource \"%s\" already has a texture", getName().c_str());
		m_texture = _texture;
	}

	//--------------------------------------------------------------------------------------
	const FrameGraph::TextureDesc & FrameGraph::TextureResource::getTextureDesc() const
	{
		return m_desc;
	}

	//--------------------------------------------------------------------------------------
	FrameGraph::TextureResource::Usage FrameGraph::TextureResource::getTextureUsage() const
	{
		return m_usage;
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
	void FrameGraph::BufferResource::setBufferDesc(const FrameGraph::BufferDesc & _resDesc)
	{
		m_desc = _resDesc;
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::BufferResource::setBufferUsage(BufferResource::Usage _usage)
	{
		m_usage |= _usage;
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

#define VG_SAFE_RELEASE_ASYNC(p) { auto * device = driver::Device::get(); if (device) device->releaseAsync(p); else p->Release(); }

	//--------------------------------------------------------------------------------------
	void FrameGraph::cleanup()
	{
		for (auto & pair : m_subPasses)
			VG_SAFE_RELEASE(pair.second);
		m_subPasses.clear();

		for (auto & pair : m_resources)
			VG_SAFE_RELEASE(pair.second);
		m_resources.clear();

		for (auto * renderPass : m_renderPasses)
			VG_SAFE_RELEASE_ASYNC(renderPass);
		m_renderPasses.clear();
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::import(const ResourceID & _resID, Texture * _tex)
	{
		TextureResource & res = addTextureResource(_resID);
						  res.setTexture(_tex);
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
			for (auto * renderTarget : subPass->m_renderTargetOut)
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
		reverseAndRemoveDuplicates();

		// TODO: remove useless passes (output is unused)

		// TODO: reorder passes

		// TODO: allocate resources
		allocateResources();

		// TODO: merge adjacent passes when possible

		// TODO: make resources required in only one RenderPass transient
		for (auto * renderPass : m_renderPasses)
			VG_SAFE_RELEASE(renderPass);
		m_renderPasses.clear();

		// TEMP: one render pass with one subpass for each userpass (TODO: make pools)
		for (auto * userPass : m_userPassStack)
		{
			RenderPass * renderPass = new RenderPass();
			SubPass * subPass = new SubPass();
			
			// build all list with attachments from all passes
			// for (...)
			{
				auto * pass = userPass;

				for (auto * res : pass->getRenderTargets())
				{
					auto * tex = res->getTexture();
					renderPass->m_attachments.push_back(tex); // TODO: check if already exist
				}
			}

			subPass->m_userPasses.push_back(userPass);
			renderPass->addSubPass(subPass);
			m_renderPasses.push_back(renderPass);

			renderPass->finalize();
		}
	}

	//--------------------------------------------------------------------------------------
	void FrameGraph::render()
	{
		// Temp: use graphics command list
		Device * device = Device::get();
		CommandList * cmdList = device->getCommandLists(CommandListType::Graphics)[0];

		for (const RenderPass * renderPass : m_renderPasses)
		{
			cmdList->beginRenderPass(renderPass);
			const auto & subPasses = renderPass->getSubPasses();
			for (uint i = 0; i < subPasses.size(); ++i)
			{
				const SubPass * subPass = subPasses[i];
				cmdList->beginSubPass(i, subPass);
				{
					for (auto * userPass : subPass->m_userPasses)
						userPass->draw(cmdList);
				}cmdList->endSubPass();
			}
			cmdList->endRenderPass();
		}

        cleanup();
	}
}