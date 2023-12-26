namespace vg::gfx::vulkan
{
	//--------------------------------------------------------------------------------------
	CommandList::CommandList(gfx::CommandListType _type, gfx::CommandPool * _cmdPool, core::uint _frame, core::uint _index) :
		super(_type, _cmdPool, _frame, _index)
	{
		Device * device = gfx::Device::get();

		VkCommandBufferAllocateInfo cmdBufAllocInfo;
		cmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufAllocInfo.pNext = nullptr;
		cmdBufAllocInfo.commandPool = _cmdPool->getVulkanCommandPool();
		cmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufAllocInfo.commandBufferCount = 1;

		VG_VERIFY_VULKAN(vkAllocateCommandBuffers(device->getVulkanDevice(), &cmdBufAllocInfo, &m_vkCommandBuffer));
	}

	//--------------------------------------------------------------------------------------
	CommandList::~CommandList()
	{
		// Command buffers will be automatically freed when their command pool is destroyed,
		// so we don't need an explicit cleanup.
	}

	//--------------------------------------------------------------------------------------
	void CommandList::reset()
	{
		VkCommandBufferBeginInfo cmdBufBegInfo;
		cmdBufBegInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufBegInfo.pNext = nullptr;
		cmdBufBegInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		cmdBufBegInfo.pInheritanceInfo = nullptr;

		VG_VERIFY_VULKAN(vkBeginCommandBuffer(m_vkCommandBuffer, &cmdBufBegInfo));
	}

	//--------------------------------------------------------------------------------------
	void CommandList::close()
	{
		VG_VERIFY_VULKAN(vkEndCommandBuffer(m_vkCommandBuffer));
	}

    //--------------------------------------------------------------------------------------
    void CommandList::transitionResource(gfx::Texture * _texture, ResourceState _before, ResourceState _after)
    {
        VkImageMemoryBarrier imageMemoryBarrier;
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.pNext = nullptr;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.image = _texture->getResource().getVulkanImage();
        imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
        imageMemoryBarrier.subresourceRange.levelCount = 1;
        imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
        imageMemoryBarrier.subresourceRange.layerCount = 1;

        switch (_before)
        {
            default:
                VG_ASSERT(false, "transitionResource from '%s' to '%s' is not implemented", asString(_before).c_str(), asString(_after).c_str());
                break;

            case ResourceState::UnorderedAccess:
            {
                switch (_after)
                {
                    default:
                        VG_ASSERT(false, "transitionResource from '%s' to '%s' is not implemented", asString(_before).c_str(), asString(_after).c_str());
                        break;

                    case ResourceState::ShaderResource:
                    {
                        // ShaderResource to RenderTarget transition
                        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
                        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
                        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

                        vkCmdPipelineBarrier(m_vkCommandBuffer, srcStageMask, dstStageMask, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
                    }
                    break;
                }
            }
            break;

            case ResourceState::ShaderResource:
            {
                switch (_after)
                {
                    default:
                        VG_ASSERT(false, "transitionResource from '%s' to '%s' is not implemented", asString(_before).c_str(), asString(_after).c_str());
                        break;

                    case ResourceState::UnorderedAccess:
                    {
                        // ShaderResource to UnorderedAccess transition
                        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                        imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

                        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;

                        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
                        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

                        vkCmdPipelineBarrier(m_vkCommandBuffer, srcStageMask, dstStageMask, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
                    }
                    break;

                    case ResourceState::RenderTarget:
                    {
                        // ShaderResource to RenderTarget transition
                        imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                        imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

                        imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                        imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                        VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
                        VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
                        
                        vkCmdPipelineBarrier(m_vkCommandBuffer, srcStageMask, dstStageMask, 0, 0, NULL, 0, NULL, 1, &imageMemoryBarrier);
                    }
                    break;
                }
            }
            break;
        }        
    }

    //--------------------------------------------------------------------------------------
    void CommandList::transitionResource(gfx::Buffer * _buffer, ResourceState _before, ResourceState _after)
    {
        VkBufferMemoryBarrier bufferMemoryBarrier = {};
        bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        bufferMemoryBarrier.pNext = NULL;
        bufferMemoryBarrier.srcAccessMask = 0;
        bufferMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        bufferMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferMemoryBarrier.buffer = _buffer->getResource().getVulkanBuffer();
        bufferMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        bufferMemoryBarrier.size = _buffer->getBufDesc().getSize();

        switch (_before)
        {
            default:
                VG_ASSERT(false, "transitionResource from '%s' to '%s' is not implemented", asString(_before).c_str(), asString(_after).c_str());
                break;

            case ResourceState::NonPixelShaderResource:
            {
                switch (_after)
                {
                    VG_ASSERT(false, "transitionResource from '%s' to '%s' is not implemented", asString(_before).c_str(), asString(_after).c_str());
                    break;

                    case ResourceState::UnorderedAccess:
                    {
                        // ShaderResource to UnorderedAccess transition
                        bufferMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                        bufferMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

                        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
                    }
                    break;
                }
            }
            break;

            case ResourceState::UnorderedAccess:
            {
                switch (_after)
                {
                    default:
                        VG_ASSERT(false, "transitionResource from '%s' to '%s' is not implemented", asString(_before).c_str(), asString(_after).c_str());
                        break;

                    case ResourceState::NonPixelShaderResource:
                    {
                        bufferMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                        bufferMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

                        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
                    }
                    break;

                    case ResourceState::ShaderResource:
                    {
                        // ShaderResource to RenderTarget transition
                        bufferMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
                        bufferMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

                        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
                    }
                    break;
                }
            }
            break;

            case ResourceState::ShaderResource:
            {
                switch (_after)
                {
                    default:
                        VG_ASSERT(false, "transitionResource from '%s' to '%s' is not implemented", asString(_before).c_str(), asString(_after).c_str());
                        break;

                    case ResourceState::UnorderedAccess:
                    {
                        // ShaderResource to UnorderedAccess transition
                        bufferMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                        bufferMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

                        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 1, &bufferMemoryBarrier, 0, nullptr);
                    }
                    break;
                }
            }
            break;
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::addRWTextureBarrier(gfx::Texture * _texture)
    {
        VG_ASSERT_NOT_IMPLEMENTED();
        //VkFormat imageFormat = barrier.pResource->GetImpl()->GetImageCreateInfo().format;
        //
        //VkImageMemoryBarrier imageBarrier = {};
        //imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        //imageBarrier.pNext = nullptr;
        //imageBarrier.srcAccessMask = ConvertToAccessMask(barrier.SourceState); // Is this really needed for a UAV barrier? Remove if it's ignored
        //imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        //imageBarrier.oldLayout = barrier.SourceState == ResourceState::Present ? VK_IMAGE_LAYOUT_UNDEFINED : ConvertToLayout(barrier.SourceState);
        //imageBarrier.newLayout = ConvertToLayout(barrier.DestState);
        //imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        //imageBarrier.subresourceRange.aspectMask = GetImageAspectMask(imageFormat);
        //SetSubResourceRange(barrier.pResource, imageBarrier, barrier.SubResource);
        //imageBarrier.image = barrier.pResource->GetImpl()->GetImage();
        //
        //imageBarriers.push_back(imageBarrier);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::addRWBufferBarrier(gfx::Buffer * _buffer)
    {
        VkBufferMemoryBarrier bufferBarrier;
        bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        bufferBarrier.pNext = nullptr;
        bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferBarrier.buffer = _buffer->getResource().getVulkanBuffer();
        bufferBarrier.offset = 0;
        bufferBarrier.size = _buffer->getBufDesc().getSize();

        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0, 0, nullptr, 1, &bufferBarrier, 0, nullptr);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::clearRWBuffer(gfx::Buffer * _buffer, core::uint _clearValue)
    {
        vkCmdFillBuffer(m_vkCommandBuffer, _buffer->getResource().getVulkanBuffer(), 0, _buffer->getBufDesc().getSize(), _clearValue);
    }

	//--------------------------------------------------------------------------------------
	void CommandList::beginRenderPass(gfx::RenderPass * _renderPass)
	{
		super::beginRenderPass(_renderPass);

        auto subPass = _renderPass->getSubPasses()[0];
        auto passInfo = subPass->getUserPassesInfos()[0];

        const auto resourceTransitions = passInfo.m_resourceTransitions;

        for (uint i = 0; i < resourceTransitions.size(); ++i)
        {
            const FrameGraphResourceTransitionDesc resTrans = resourceTransitions[i];

            const auto resType = resTrans.m_resource->getType();
            switch (resType)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(resType);

                case FrameGraphResource::Type::Buffer:
                    transitionResource(((FrameGraphBufferResource *)resTrans.m_resource)->getBuffer(), resTrans.m_transitionDesc.begin, resTrans.m_transitionDesc.end);
                    break;

                case FrameGraphResource::Type::Texture:
                    transitionResource(((FrameGraphTextureResource *)resTrans.m_resource)->getTexture(), resTrans.m_transitionDesc.begin, resTrans.m_transitionDesc.end);
                    break;
            }            
        }

        _renderPass->begin(this);	
	}

	//--------------------------------------------------------------------------------------
	void CommandList::endRenderPass()
	{
        m_renderPass->end(this); 
		super::endRenderPass();
	}

	//--------------------------------------------------------------------------------------
	void CommandList::beginSubPass(core::uint _subPassIndex, gfx::SubPass * _subPass)
	{
        super::beginSubPass(_subPassIndex, _subPass);

        // TODO
	}

	//--------------------------------------------------------------------------------------
	void CommandList::endSubPass()
	{

	}

    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicRootSignature(gfx::RootSignature * _rootSig)
    {
        auto * device = gfx::Device::get();
        auto * sig = device->getRootSignature(m_graphicStateCache.m_graphicPipelineKey.m_rootSignature);

        if (sig->getVulkanDescriptorSetLayouts().size() > 0)
        {
            vector<VkDescriptorSet> vkDescriptorSets;

            vkDescriptorSets.push_back(device->m_vkBindlessDescriptors);
            vkDescriptorSets.push_back(device->m_vkSamplerDescriptors);

            vkCmdBindDescriptorSets(m_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, sig->getVulkanPipelineLayout(), 0, (uint)vkDescriptorSets.size(), vkDescriptorSets.data(), 0, nullptr);
        }
    }
    
    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicPipelineState(gfx::GraphicPipelineState * _pso)
    {
        vkCmdBindPipeline(m_vkCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pso->getVulkanPipeline());
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindViewport(const core::uint4 & _viewport)
    {
        VkViewport vp;
        vp.x = (float)_viewport.x;
        vp.y = (float)_viewport.y + _viewport.w;
        vp.width = (float)_viewport.z;
        vp.height = -(float)_viewport.w;
        vp.minDepth = 0.0f;
        vp.maxDepth = 1.0f;

        vkCmdSetViewport(m_vkCommandBuffer, 0, 1, &vp);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindScissor(const core::uint4 & _scissor)
    {
        VkRect2D scissor;
        scissor.offset.x = _scissor.x;
        scissor.offset.y = _scissor.y;
        scissor.extent.width = _scissor.z;
        scissor.extent.height = _scissor.w;

        vkCmdSetScissor(m_vkCommandBuffer, 0, 1, &scissor);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindGraphicRootConstants(core::uint(&_constants)[max_root_constants])
    {
        const auto vkPipelineLayout = m_graphicStateCache.m_rootSignature->getVulkanPipelineLayout();
        const auto & rootConstantDesc = m_graphicStateCache.m_rootSignature->getRootSignatureDesc().getRootConstants();
        for (uint i = 0; i < rootConstantDesc.size(); ++i)
        {
            const RootSignatureDesc::PushConstantParams & param = rootConstantDesc[i];
            vkCmdPushConstants(m_vkCommandBuffer, vkPipelineLayout, RootSignature::getVulkanShaderStageFlags(param.m_stages), param.m_register<<2, param.m_count<<2, _constants);
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindIndexBuffer(gfx::Buffer * _ib)
    {
        if (_ib)
        {
            const BufferDesc & ibDesc = _ib->getBufDesc();
            vkCmdBindIndexBuffer(m_vkCommandBuffer, _ib->getResource().getVulkanBuffer(), 0, ibDesc.elementSize == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::clear(const core::float4 & _color)
    {

    }

    //--------------------------------------------------------------------------------------
    void CommandList::draw(core::uint _vertexCount, core::uint _startOffset)
    {
        vkCmdDraw(m_vkCommandBuffer, _vertexCount, 1, _startOffset, 0);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::drawIndexed(core::uint _indexCount, core::uint _startIndex, core::uint _baseVertex)
    {
        // test
        vkCmdSetStencilReference(m_vkCommandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, 0);

        vkCmdDrawIndexed(m_vkCommandBuffer, _indexCount, 1, _startIndex, 0, 0);
    }

    //--------------------------------------------------------------------------------------
    Map CommandList::map(gfx::Buffer * _buffer, size_t _size)
    {
        VG_ASSERT((size_t)-1 != _size);

        auto * device = gfx::Device::get();
        auto uploadBuffer = device->getUploadBuffer();

        Map result;

        if (_buffer->getBufDesc().resource.m_usage == Usage::Staging)
        {
            VG_ASSERT_NOT_IMPLEMENTED();
            result.data = nullptr;
        }
        else
        {
            VkMemoryRequirements mem_reqs;
            vkGetBufferMemoryRequirements(device->getVulkanDevice(), _buffer->getResource().getVulkanBuffer(), &mem_reqs);
            VG_ASSERT(mem_reqs.size == _buffer->getBufDesc().getSize());

            result.data = uploadBuffer->map(_size, (uint)mem_reqs.alignment);
        }
        
        result.rowPitch = 0;
        result.slicePitch = 0;
        return result;
    }

    //--------------------------------------------------------------------------------------
    void CommandList::unmap(gfx::Buffer * _buffer, void * VG_RESTRICT _data, size_t _size)
    {
        VG_ASSERT((size_t)-1 != _size);

        if (_buffer->getBufDesc().resource.m_usage == Usage::Staging)
        {
            VG_ASSERT(nullptr == _data, "The '_data' parameter should be NULL when mapping Staging Resources");
            VG_ASSERT_NOT_IMPLEMENTED();
        }
        else
        {
            VG_ASSERT(nullptr != _data, "The '_data' parameter should not be NULL when mapping Resources for Upload");
            auto * device = gfx::Device::get();
            auto uploadBuffer = device->getUploadBuffer();
            uploadBuffer->unmap(_buffer, (u8 *)_data, _size);
            uploadBuffer->flush((gfx::CommandList *)this);
        }
    }
      
    //--------------------------------------------------------------------------------------
    void CommandList::copyBuffer(gfx::Buffer * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset, size_t _size)
    {
        auto * device = gfx::Device::get();
        auto & context = device->getCurrentFrameContext();

        const auto & desc = _dst->getBufDesc();

        VkBufferCopy vkBufferCopy = {};
        vkBufferCopy.srcOffset = _srcOffset; 
        vkBufferCopy.dstOffset = 0; 
        vkBufferCopy.size = _size; // desc.getSize();

        VkBufferMemoryBarrier vkBufMemBarrierBefore = {};
        vkBufMemBarrierBefore.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        vkBufMemBarrierBefore.pNext = NULL;
        vkBufMemBarrierBefore.srcAccessMask = 0;
        vkBufMemBarrierBefore.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkBufMemBarrierBefore.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBufMemBarrierBefore.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBufMemBarrierBefore.buffer = _dst->getResource().getVulkanBuffer();

        // Make sure anything that was copying from this image has completed 
        vkBufMemBarrierBefore.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkBufMemBarrierBefore.size = _size; //desc.getSize();

        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 1, &vkBufMemBarrierBefore, 0, nullptr);

        vkCmdCopyBuffer(m_vkCommandBuffer, _src->getResource().getVulkanBuffer(), _dst->getResource().getVulkanBuffer(), 1, &vkBufferCopy);

        VkBufferMemoryBarrier vkBufMemBarrierAfter = {};
        vkBufMemBarrierAfter.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        vkBufMemBarrierAfter.pNext = NULL;
        vkBufMemBarrierAfter.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        vkBufMemBarrierAfter.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        vkBufMemBarrierAfter.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBufMemBarrierAfter.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        vkBufMemBarrierAfter.buffer = _dst->getResource().getVulkanBuffer();
        vkBufMemBarrierAfter.size = _size; //desc.getSize();
        
        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 1, &vkBufMemBarrierAfter, 0, nullptr);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::copyTexture(gfx::Texture * _dst, gfx::Buffer * _src, core::uint_ptr _srcOffset)
    {
        auto * device = gfx::Device::get();
        auto & context = device->getCurrentFrameContext();

        const auto & desc = _dst->getTexDesc();

        VkImageMemoryBarrier image_memory_barrier = {};
                             image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                             image_memory_barrier.pNext = NULL;
                             image_memory_barrier.srcAccessMask = 0;
                             image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                             image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                             image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                             image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_PREINITIALIZED;
                             image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                             image_memory_barrier.image = _dst->getResource().getVulkanImage();
                             image_memory_barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, desc.mipmaps, 0, 1 };
                             // Make sure anything that was copying from this image has completed 
                             image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &image_memory_barrier);

        VkDeviceSize currentOffset = _srcOffset;

        for (uint i = 0; i < desc.mipmaps; ++i)
        {
            const uint w = desc.width >> i;
            const uint h = desc.height >> i;

            VkBufferImageCopy vkBufImgCopy = {};
                              vkBufImgCopy.bufferOffset = currentOffset;
                              vkBufImgCopy.bufferRowLength = 0;
                              vkBufImgCopy.bufferImageHeight = 0;
                              vkBufImgCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                              vkBufImgCopy.imageSubresource.mipLevel = i;
                              vkBufImgCopy.imageSubresource.baseArrayLayer = 0;
                              vkBufImgCopy.imageSubresource.layerCount = 1;
                              vkBufImgCopy.imageOffset = { 0, 0, 0 };
                              vkBufImgCopy.imageExtent = { w, h, 1 };

            vkCmdCopyBufferToImage(m_vkCommandBuffer, _src->getResource().getVulkanBuffer(), _dst->getResource().getVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &vkBufImgCopy);

            currentOffset += w * h * Texture::getPixelFormatSize(desc.format);
        }

        VkImageMemoryBarrier image_memory_barrier2 = {};
        image_memory_barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier2.pNext = NULL;
        image_memory_barrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        image_memory_barrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        image_memory_barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        image_memory_barrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        image_memory_barrier2.image = _dst->getResource().getVulkanImage();
        image_memory_barrier2.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, desc.mipmaps, 0, 1};

        vkCmdPipelineBarrier(m_vkCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier2);
    }

    //--------------------------------------------------------------------------------------
    float4 asFloat4(u32 _color)
    {
        float4 color = float4((_color & 0xFF), (_color >> 8) & 0xFF, (_color >> 16) & 0xFF, (_color >> 24) & 0xFF);
        return color / float4(255.0f);
    }

    //--------------------------------------------------------------------------------------
    void CommandList::beginGPUEvent(const char * _name, core::u32 _color)
    {
        #ifdef VG_ENABLE_GPU_MARKER 
        VkDebugUtilsLabelEXT markerInfo = {};
        markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
        float4 color = asFloat4(_color);
        markerInfo.color[0] = color.r;
        markerInfo.color[1] = color.g;
        markerInfo.color[2] = color.b;
        markerInfo.color[3] = color.a;
        markerInfo.pLabelName = _name;

        auto * device = gfx::Device::get();
        device->m_EXT_DebugUtils.m_pfnCmdBeginDebugUtilsLabelEXT(getVulkanCommandBuffer(), &markerInfo);
        #endif
    }

    //--------------------------------------------------------------------------------------
    void CommandList::endGPUEvent()
    {
        #ifdef VG_ENABLE_GPU_MARKER 
        auto * device = gfx::Device::get();
        device->m_EXT_DebugUtils.m_pfnCmdEndDebugUtilsLabelEXT(getVulkanCommandBuffer());
        #endif
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindComputeRootSignature(gfx::RootSignature * _rootSig)
    {
        auto * device = gfx::Device::get();
        auto * sig = device->getRootSignature(m_computeStateCache.m_computePipelineKey.m_computeRootSignature);

        if (sig->getVulkanDescriptorSetLayouts().size() > 0)
        {
            vector<VkDescriptorSet> vkDescriptorSets;

            vkDescriptorSets.push_back(device->m_vkBindlessDescriptors);
            vkDescriptorSets.push_back(device->m_vkSamplerDescriptors);

            vkCmdBindDescriptorSets(m_vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, sig->getVulkanPipelineLayout(), 0, (uint)vkDescriptorSets.size(), vkDescriptorSets.data(), 0, nullptr);
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindComputePipelineState(gfx::ComputePipelineState * _pso)
    {
        vkCmdBindPipeline(m_vkCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _pso->getVulkanComputePipeline());
    }

    //--------------------------------------------------------------------------------------
    void CommandList::bindComputeRootConstants(core::uint(&_constants)[max_root_constants])
    {
        const auto vkPipelineLayout = m_computeStateCache.m_rootSignature->getVulkanPipelineLayout();
        const auto & rootConstantDesc = m_computeStateCache.m_rootSignature->getRootSignatureDesc().getRootConstants();
        for (uint i = 0; i < rootConstantDesc.size(); ++i)
        {
            const RootSignatureDesc::PushConstantParams & param = rootConstantDesc[i];
            vkCmdPushConstants(m_vkCommandBuffer, vkPipelineLayout, RootSignature::getVulkanShaderStageFlags(param.m_stages), param.m_register << 2, param.m_count << 2, _constants);
        }
    }

    //--------------------------------------------------------------------------------------
    void CommandList::dispatch(core::uint3 _threadGroupCount)
    {
        vkCmdDispatch(m_vkCommandBuffer, _threadGroupCount.x, _threadGroupCount.y, _threadGroupCount.z);
    }

    //--------------------------------------------------------------------------------------
    // KHR_Acceleration_Structure extension device funcs
    //--------------------------------------------------------------------------------------
    
    //--------------------------------------------------------------------------------------
    void CommandList::buildAccelerationStructures(core::u32 _infoCount, const VkAccelerationStructureBuildGeometryInfoKHR * _infos, const VkAccelerationStructureBuildRangeInfoKHR * const * _buildRangeInfos)
    {
        VG_ASSERT(gfx::Device::get()->m_KHR_Acceleration_Structure.m_pfnCmdBuildAccelerationStructuresKHR);
        gfx::Device::get()->m_KHR_Acceleration_Structure.m_pfnCmdBuildAccelerationStructuresKHR(m_vkCommandBuffer, _infoCount, _infos, _buildRangeInfos);
    }
}