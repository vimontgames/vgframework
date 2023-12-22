#include "gfx/Device/vulkan/Extension/ExtensionList_vulkan.hpp"

#if !VG_ENABLE_INLINE
#include "Device_Vulkan.inl"
#endif

namespace vg::gfx::vulkan
{
	using namespace vg::core;

	//--------------------------------------------------------------------------------------
	VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
	{
		return static_cast<Device*>(pUserData)->onDebugMessage(messageSeverity, messageType, pCallbackData);
	}

	//--------------------------------------------------------------------------------------
	Severity Device::getSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT _severity, const VkDebugUtilsMessengerCallbackDataEXT * _data)
	{
		Severity severity = Severity::Verbose;

		 if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT & _severity)
			 severity = Severity::Error;
		 else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT & _severity)
			 severity = Severity::Warning;
		 else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT & _severity)
			 severity = Severity::Info;
		 else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT & _severity)
			 severity = Severity::Verbose;

		 if (nullptr != _data)
		 {
			 // These messages are annoying but should really be errors so treat them as if they were warnings
			 if (Severity::Error == severity)
			 {
				 static const char * errorAsWarningList[] =
				 {
					 "UNASSIGNED-CoreValidation-Shader-OutputNotConsumed",
					 //"VUID-VkGraphicsPipelineCreateInfo-layout-07990"
				 };
				 for (uint i = 0; i < countof(errorAsWarningList); ++i)
				 {
					 if (!strcmp(errorAsWarningList[i], _data->pMessageIdName))
					 {
						 severity = Severity::Warning;
						 break;
					 }
				 }
			 }
		 }

		 return severity;
	}

	//--------------------------------------------------------------------------------------
	const char * Device::getVulkanObjectTypeName(VkObjectType _type)
	{
		switch (_type)
		{
			case VK_OBJECT_TYPE_QUERY_POOL:
				return "VK_OBJECT_TYPE_QUERY_POOL";
			case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
				return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION";
			case VK_OBJECT_TYPE_SEMAPHORE:
				return "VK_OBJECT_TYPE_SEMAPHORE";
			case VK_OBJECT_TYPE_SHADER_MODULE:
				return "VK_OBJECT_TYPE_SHADER_MODULE";
			case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
				return "VK_OBJECT_TYPE_SWAPCHAIN_KHR";
			case VK_OBJECT_TYPE_SAMPLER:
				return "VK_OBJECT_TYPE_SAMPLER";
			case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV:
				return "VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV";
			case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:
				return "VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT";
			case VK_OBJECT_TYPE_IMAGE:
				return "VK_OBJECT_TYPE_IMAGE";
			case VK_OBJECT_TYPE_UNKNOWN:
				return "VK_OBJECT_TYPE_UNKNOWN";
			case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
				return "VK_OBJECT_TYPE_DESCRIPTOR_POOL";
			case VK_OBJECT_TYPE_COMMAND_BUFFER:
				return "VK_OBJECT_TYPE_COMMAND_BUFFER";
			case VK_OBJECT_TYPE_BUFFER:
				return "VK_OBJECT_TYPE_BUFFER";
			case VK_OBJECT_TYPE_SURFACE_KHR:
				return "VK_OBJECT_TYPE_SURFACE_KHR";
			case VK_OBJECT_TYPE_INSTANCE:
				return "VK_OBJECT_TYPE_INSTANCE";
			case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:
				return "VK_OBJECT_TYPE_VALIDATION_CACHE_EXT";
			case VK_OBJECT_TYPE_IMAGE_VIEW:
				return "VK_OBJECT_TYPE_IMAGE_VIEW";
			case VK_OBJECT_TYPE_DESCRIPTOR_SET:
				return "VK_OBJECT_TYPE_DESCRIPTOR_SET";
			case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
				return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT";
			case VK_OBJECT_TYPE_COMMAND_POOL:
				return "VK_OBJECT_TYPE_COMMAND_POOL";
			case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
				return "VK_OBJECT_TYPE_PHYSICAL_DEVICE";
			case VK_OBJECT_TYPE_DISPLAY_KHR:
				return "VK_OBJECT_TYPE_DISPLAY_KHR";
			case VK_OBJECT_TYPE_BUFFER_VIEW:
				return "VK_OBJECT_TYPE_BUFFER_VIEW";
			case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
				return "VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT";
			case VK_OBJECT_TYPE_FRAMEBUFFER:
				return "VK_OBJECT_TYPE_FRAMEBUFFER";
			case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:
				return "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE";
			case VK_OBJECT_TYPE_PIPELINE_CACHE:
				return "VK_OBJECT_TYPE_PIPELINE_CACHE";
			case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
				return "VK_OBJECT_TYPE_PIPELINE_LAYOUT";
			case VK_OBJECT_TYPE_DEVICE_MEMORY:
				return "VK_OBJECT_TYPE_DEVICE_MEMORY";
			case VK_OBJECT_TYPE_FENCE:
				return "VK_OBJECT_TYPE_FENCE";
			case VK_OBJECT_TYPE_QUEUE:
				return "VK_OBJECT_TYPE_QUEUE";
			case VK_OBJECT_TYPE_DEVICE:
				return "VK_OBJECT_TYPE_DEVICE";
			case VK_OBJECT_TYPE_RENDER_PASS:
				return "VK_OBJECT_TYPE_RENDER_PASS";
			case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
				return "VK_OBJECT_TYPE_DISPLAY_MODE_KHR";
			case VK_OBJECT_TYPE_EVENT:
				return "VK_OBJECT_TYPE_EVENT";
			case VK_OBJECT_TYPE_PIPELINE:
				return "VK_OBJECT_TYPE_PIPELINE";
			default:
				return "Unhandled VkObjectType";
		}
	}

	//--------------------------------------------------------------------------------------
	bool Device::onDebugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT _severity, VkDebugUtilsMessageTypeFlagsEXT _flags, const VkDebugUtilsMessengerCallbackDataEXT * _data)
	{
		char prefix[64] = "";
		const auto msgLen = strlen(_data->pMessage) + 5000;
		char * message = (char *)malloc(msgLen);
		VG_ASSERT(message);

		const DeviceParams & deviceParams = getDeviceParams();

		const Severity severity = getSeverity(_severity, _data);
		switch (severity)
		{
			case Severity::Verbose:
				strcat_s(prefix, "VERBOSE : ");
				break;

			case Severity::Info:
				strcat_s(prefix, "INFO : ");
				break;

			case Severity::Warning:
				strcat_s(prefix, "WARNING : ");
				break;

			case Severity::Error:
				strcat_s(prefix, "ERROR : ");
				break;
		}

		if (_flags & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
		{
			strcat_s(prefix, "GENERAL");
		}
		else 
		{
			if (_flags & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
			{
				strcat_s(prefix, "VALIDATION");
				m_validationErrors++;
			}
			if (_flags & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT)
			{
				if (_flags & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT)
					strcat_s(prefix, "|");

				strcat_s(prefix, "PERFORMANCE");
			}
		}

		sprintf_s(message, msgLen, "%s - Message Id Number: %d | Message Id Name: %s\n\t%s\n", prefix, _data->messageIdNumber, _data->pMessageIdName, _data->pMessage);
		if (_data->objectCount > 0) 
		{
			char tmp_message[500];
			sprintf_s(tmp_message, "\n\tObjects - %d\n", _data->objectCount);
			strcat_s(message, msgLen, tmp_message);

			for (uint object = 0; object < _data->objectCount; ++object)
			{
				if (nullptr != _data->pObjects[object].pObjectName && strlen(_data->pObjects[object].pObjectName) > 0)
					sprintf_s(tmp_message, "\t\tObject[%d] - %s, Handle %p, Name \"%s\"\n", object, getVulkanObjectTypeName(_data->pObjects[object].objectType), (void *)(_data->pObjects[object].objectHandle), _data->pObjects[object].pObjectName);
				else
					sprintf_s(tmp_message, "\t\tObject[%d] - %s, Handle %p\n", object, getVulkanObjectTypeName(_data->pObjects[object].objectType), (void *)(_data->pObjects[object].objectHandle));

				strcat_s(message, msgLen, tmp_message);
			}
		}
		if (_data->cmdBufLabelCount > 0)
		{
			char tmp_message[500];
			sprintf_s(tmp_message, "\n\tCommand Buffer Labels - %d\n", _data->cmdBufLabelCount);
			strcat_s(message, msgLen, tmp_message);
			for (uint cmd_buf_label = 0; cmd_buf_label < _data->cmdBufLabelCount; ++cmd_buf_label)
			{
				sprintf_s(tmp_message, "\t\tLabel[%d] - %s { %f, %f, %f, %f}\n", cmd_buf_label,
					_data->pCmdBufLabels[cmd_buf_label].pLabelName, _data->pCmdBufLabels[cmd_buf_label].color[0],
					_data->pCmdBufLabels[cmd_buf_label].color[1], _data->pCmdBufLabels[cmd_buf_label].color[2],
					_data->pCmdBufLabels[cmd_buf_label].color[3]);
				strcat_s(message, msgLen, tmp_message);
			}
		}

        bool ignore = false;
        //static const char * ignoreList[] =
        //{
        //    "VUID-vkCmdResetQueryPool-commandBuffer-recording",
        //    "VUID-vkFreeDescriptorSets-pDescriptorSets-00309",
        //    "UNASSIGNED-CoreValidation-DrawState-QueryNotReset",
        //    "UNASSIGNED-CoreValidation-Shader-DescriptorTypeMismatch"
        //};
        //for (uint i = 0; i < countof(ignoreList); ++i)
        //{
        //    if (!strcmp(ignoreList[i], _data->pMessageIdName))
        //    {
        //        ignore = true;
        //        break;
        //    }
        //}

		// Hack to ignore false positive in this very specific case
		// Generated SPIR-V code is a ByteAddressBuffer but validation layers says shader is expecting a texel buffer
		if (!strcmp("VUID-VkGraphicsPipelineCreateInfo-layout-07990", _data->pMessageIdName))
		{
			if (strstr(message, "uses type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER but expected (VK_DESCRIPTOR_TYPE_STORAGE_BUFFER or VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)"))
				ignore = true;
		}

        if (ignore)
        {
            VG_SAFE_FREE(message);
        }
        else
        {
			switch (severity)
			{
				case Severity::Error:
					VG_ERROR("[Device] %s", message);
					break;

				case Severity::Warning:
					VG_WARNING("[Device] %s", message);
					break;

				case Severity::Info:
					VG_INFO("[Device] %s", message);
					break;

				case Severity::Verbose:
					VG_DEBUGPRINT("[Device] %s", message);
					break;
			}

            VG_SAFE_FREE(message);

            if ((Severity::Error == severity && deviceParams.breakOnErrors) || (Severity::Warning == severity && deviceParams.breakOnWarnings))
                DebugBreak();
        }
		return false;
	}

	//--------------------------------------------------------------------------------------
	void Device::registerExtensions(const DeviceParams & _params)
	{
        #ifdef VG_ENABLE_GPU_MARKER
        m_instanceExtensionList.registerExtension(m_EXT_DebugUtils);
        #endif

		m_instanceExtensionList.registerExtension(m_KHR_Surface);
		m_instanceExtensionList.registerExtension(m_KHR_Win32_Surface);

		m_deviceExtensionList.registerExtension(m_KHR_Swapchain);    
		m_deviceExtensionList.registerExtension(m_KHR_Deferred_Host_Operations);
		m_deviceExtensionList.registerExtension(m_KHR_Buffer_Device_Address);
		m_deviceExtensionList.registerExtension(m_KHR_Acceleration_Structure);
		m_deviceExtensionList.registerExtension(m_KHR_Ray_Tracing_Pipeline);
		m_deviceExtensionList.registerExtension(m_KHR_Ray_Query);
	}

    //--------------------------------------------------------------------------------------
    bool Device::enableInstanceLayer(const char * _layerName)
    {
        bool found = false;
     
        core::u32 instance_layer_count = 0;
        VG_ASSERT_VULKAN(vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));

        if (instance_layer_count > 0)
        {
            auto * instance_layers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * instance_layer_count);
            VG_ASSERT_VULKAN(vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layers));

            for (uint j = 0; j < instance_layer_count; j++)
            {
                if (!strcmp(_layerName, instance_layers[j].layerName))
                {
                    found = true;
                    VG_INFO("[Device] Layer \"%s\" is enabled", _layerName);
                    break;
                }
            }

            free(instance_layers);
        }

        VG_ASSERT(found, "Could not find layer \"%s\"", _layerName);
        return found;
    }

	//--------------------------------------------------------------------------------------
	void Device::init(const DeviceParams & _params)
	{
		base::Device::init(_params);

		registerExtensions(_params);

		for (uint index  = 0; index < enumCount<CommandQueueType>(); ++index)
			m_vkCommandQueueFamilyIndex[index] = s_vkCommandQueueFamilyIndexInvalid;
        
		bool validationLayer = _params.debugDevice;
        const char * validationLayerName = "VK_LAYER_KHRONOS_validation";

        #ifdef VG_ENABLE_GPU_MARKER
        if (_params.debugDevice)
            validationLayer = enableInstanceLayer(validationLayerName);
        #endif

		// Look for instance extensions 
		m_instanceExtensionList.init();

		const VkApplicationInfo app = 
		{
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			"VimontGames framework",
			0,
			"VimontGames framework",
			0,
			VK_API_VERSION_1_3
		};

		VkInstanceCreateInfo inst_info = 
		{
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,					
			nullptr,
			0,
			&app,
            validationLayer ? 1UL : 0,
			(const char *const *)&validationLayerName,
			m_instanceExtensionList.getEnabledExtensionCount(),
			m_instanceExtensionList.getEnabledExtensionNames(),
		};

		VkDebugUtilsMessengerCreateInfoEXT dbg_messenger_create_info;
		if (validationLayer) 
		{
			// VK_EXT_debug_utils style
			dbg_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			dbg_messenger_create_info.pNext = NULL;
			dbg_messenger_create_info.flags = 0;
			dbg_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			dbg_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			dbg_messenger_create_info.pfnUserCallback = debug_messenger_callback;
			dbg_messenger_create_info.pUserData = this;
			inst_info.pNext = &dbg_messenger_create_info;
		}

		VG_ASSERT_VULKAN(vkCreateInstance(&inst_info, nullptr, &m_vkInstance));

		m_instanceExtensionList.onInstanceCreated();

		u32 gpu_count;
		// Make initial call to query gpu_count, then second call for gpu info
		VG_ASSERT_VULKAN(vkEnumeratePhysicalDevices(m_vkInstance, &gpu_count, nullptr));
		VG_ASSERT(gpu_count > 0);

		if (gpu_count > 0) 
		{
			auto * physical_devices = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
			VG_ASSERT_VULKAN(vkEnumeratePhysicalDevices(m_vkInstance, &gpu_count, physical_devices));
	
			// just grab the first physical device
			m_vkPhysicalDevice = physical_devices[0];
			free(physical_devices);
		}

		// Look for device extensions 	
		m_deviceExtensionList.init();

		// Update device caps according to extensions
        if (m_KHR_Ray_Tracing_Pipeline.isEnabled())
            m_caps.supportRayTracing = true;
        else
            m_caps.supportRayTracing = false;

		if (m_KHR_Buffer_Device_Address.isEnabled())
			m_caps.supportDeviceAddress = true;

        #ifdef VG_ENABLE_GPU_MARKER
        VG_ASSERT_VULKAN(m_EXT_DebugUtils.m_pfnCreateDebugUtilsMessengerEXT(m_vkInstance, &dbg_messenger_create_info, nullptr, &m_vkDebugMessenger));
        #endif

		vkGetPhysicalDeviceProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceProperties);

		// Query feature support for this device
		VkPhysicalDeviceFeatures physDevFeatures;
		vkGetPhysicalDeviceFeatures(m_vkPhysicalDevice, &physDevFeatures);

		VG_ASSERT(core::invalidInstanceHandle != _params.instance);
		VG_ASSERT(core::invalidWindowHandle != _params.window);

		VkWin32SurfaceCreateInfoKHR createInfo;
									createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
									createInfo.pNext = NULL;
									createInfo.flags = 0;
									createInfo.hinstance = static_cast<HINSTANCE>(_params.instance);
									createInfo.hwnd = static_cast<HWND>(_params.window);

		VG_ASSERT_VULKAN(vkCreateWin32SurfaceKHR(m_vkInstance, &createInfo, NULL, &m_vkSurface));

		createCommandQueues();
		createVulkanDevice();

		m_vkPresentMode = VSyncToVkPresentModeKHR(m_VSync);
		createSwapchain();

        for (uint i = 0; i < max_frame_latency; ++i)
            createFrameContext(i);
		
        createUploadBuffer();
        createVulkanBackbuffers();

        m_bindlessTable = new gfx::BindlessTable();

        gfx::Device * device = static_cast<gfx::Device*>(this);

        RootSignatureDesc rsDesc;
                          rsDesc.addTable(getBindlessTable()->getTableDesc());
        m_bindlessRootSignatureHandle = device->addRootSignature(rsDesc);

        RootSignature * rs = device->getRootSignature(m_bindlessRootSignatureHandle);
        const auto & bindlessLayouts = rs->getVulkanDescriptorSetLayouts();
        {
            VkDescriptorPoolSize type_counts[] =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, BINDLESS_TEXTURE_COUNT },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, BINDLESS_BUFFER_COUNT },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, BINDLESS_RWTEXTURE_COUNT },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, BINDLESS_RWBUFFER_COUNT },
				{ VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, BINDLESS_TLAS_COUNT }
            };

            VkDescriptorPoolCreateInfo descriptor_pool = {};

            descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptor_pool.pNext = nullptr;
            descriptor_pool.maxSets = max_frame_latency;
            descriptor_pool.poolSizeCount = (uint)countof(type_counts);
            descriptor_pool.pPoolSizes = type_counts;
            descriptor_pool.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

            VG_ASSERT_VULKAN(vkCreateDescriptorPool(m_vkDevice, &descriptor_pool, nullptr, &m_vkBindlessDescriptorPool));

            VkDescriptorSetAllocateInfo alloc_info = {};
            alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            alloc_info.pNext = nullptr;
            alloc_info.descriptorPool = m_vkBindlessDescriptorPool;
            alloc_info.descriptorSetCount = 1;

            // Bindless descriptors (layout 0)
            {
                VkDescriptorSetAllocateInfo alloc_info1 = {};
                alloc_info1.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                alloc_info1.pNext = nullptr;
                alloc_info1.descriptorPool = m_vkBindlessDescriptorPool;
                alloc_info1.descriptorSetCount = 1;
                alloc_info1.pSetLayouts = &bindlessLayouts[0];

                VG_ASSERT_VULKAN(vkAllocateDescriptorSets(m_vkDevice, &alloc_info1, &m_vkBindlessDescriptors));
            }
        }

        initSamplerStates();
	}

    //--------------------------------------------------------------------------------------
    // Use the bindless signature to create sampler states
    //--------------------------------------------------------------------------------------
    void Device::initSamplerStates()
    {
        gfx::Device * device = static_cast<gfx::Device*>(this);

        RootSignature * rs = device->getRootSignature(m_bindlessRootSignatureHandle);
        const auto & bindlessLayouts = rs->getVulkanDescriptorSetLayouts();

        // allocate enough descriptors in pool for bindless
        VkDescriptorPoolSize type_counts[] =
        {
            {VK_DESCRIPTOR_TYPE_SAMPLER, (uint)enumCount<Sampler>()},
        };

        VkDescriptorPoolCreateInfo descriptor_pool = {};

        descriptor_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool.pNext = nullptr;
        descriptor_pool.maxSets = max_frame_latency;
        descriptor_pool.poolSizeCount = (uint)countof(type_counts);
        descriptor_pool.pPoolSizes = type_counts;

        VG_ASSERT_VULKAN(vkCreateDescriptorPool(m_vkDevice, &descriptor_pool, nullptr, &m_vkSamplerDescriptorPool));

        // Sampler descriptors (layout 1)
        {
            VkDescriptorSetAllocateInfo alloc_info1 = {};
            alloc_info1.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            alloc_info1.pNext = nullptr;
            alloc_info1.descriptorPool = m_vkSamplerDescriptorPool;
            alloc_info1.descriptorSetCount = 1;
            alloc_info1.pSetLayouts = &bindlessLayouts[1];

            VG_ASSERT_VULKAN(vkAllocateDescriptorSets(m_vkDevice, &alloc_info1, &m_vkSamplerDescriptors));
        }

        vector<VkDescriptorImageInfo> vkSamplerDescs;
        for (uint i = 0; i < enumCount<Sampler>(); ++i)
        {
            VkSamplerCreateInfo vkSamplerCreateInfo = SamplerState::getVulkanSamplerState((Sampler)i);
            VG_ASSERT_VULKAN(vkCreateSampler(m_vkDevice, &vkSamplerCreateInfo, nullptr, &m_vkSampler[i]));

            VkDescriptorImageInfo vkSamplerDesc = {};
            vkSamplerDesc.imageView = VK_NULL_HANDLE;
            vkSamplerDesc.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            vkSamplerDesc.sampler = device->m_vkSampler[i];

            vkSamplerDescs.push_back(vkSamplerDesc);
        }

        VkWriteDescriptorSet writeSamplers = {};
        writeSamplers.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeSamplers.dstBinding = 0;
        writeSamplers.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        writeSamplers.descriptorCount = (uint)vkSamplerDescs.size();
        writeSamplers.pImageInfo = vkSamplerDescs.data();
        writeSamplers.dstSet = device->m_vkSamplerDescriptors;
        writeSamplers.dstArrayElement = 0;

        vkUpdateDescriptorSets(device->getVulkanDevice(), 1, &writeSamplers, 0, nullptr);
    }

	//--------------------------------------------------------------------------------------
	core::u32 Device::getVulkanCommandQueueFamilyIndex(CommandQueueType _type) const
	{
		VG_ASSERT(asInteger(_type) < enumCount<CommandQueueType>());
		const core::u32 index = m_vkCommandQueueFamilyIndex[asInteger(_type)];
		VG_ASSERT(s_vkCommandQueueFamilyIndexInvalid != index);
		return index;
	}

	//--------------------------------------------------------------------------------------
	void Device::createCommandQueues()
	{
		// Call with nullptr to get count
		vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &m_vkCommandQueueFamilyCount, nullptr);
		VG_ASSERT(m_vkCommandQueueFamilyCount >= 1);

		VkQueueFamilyProperties * queue_props = (VkQueueFamilyProperties *)malloc(m_vkCommandQueueFamilyCount * sizeof(VkQueueFamilyProperties));
		vkGetPhysicalDeviceQueueFamilyProperties(m_vkPhysicalDevice, &m_vkCommandQueueFamilyCount, queue_props);

		// Iterate over each queue to learn whether it supports presenting:
		VkBool32 * supportsPresent = (VkBool32 *)malloc(m_vkCommandQueueFamilyCount * sizeof(VkBool32));
		for (uint i = 0; i < m_vkCommandQueueFamilyCount; i++)
			m_KHR_Surface.m_pfnGetPhysicalDeviceSurfaceSupportKHR(m_vkPhysicalDevice, i, m_vkSurface, &supportsPresent[i]);

		// Search for a graphics and a present queue in the array of queue families, try to find one that supports both
		u32 graphicsQueueFamilyIndex = s_vkCommandQueueFamilyIndexInvalid;
		u32 computeQueueFamilyIndex = s_vkCommandQueueFamilyIndexInvalid;
		u32 presentQueueFamilyIndex = s_vkCommandQueueFamilyIndexInvalid;

		for (uint i = 0; i < m_vkCommandQueueFamilyCount; i++)
		{
			if (queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && queue_props[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				if (UINT32_MAX == graphicsQueueFamilyIndex)
					graphicsQueueFamilyIndex = i;

				if (UINT32_MAX == computeQueueFamilyIndex)
					computeQueueFamilyIndex = i;

				if (supportsPresent[i] == VK_TRUE)
				{
					graphicsQueueFamilyIndex = i;
					computeQueueFamilyIndex = i;
					presentQueueFamilyIndex = i;
					break;
				}
			}
		}

		if (presentQueueFamilyIndex == UINT32_MAX)
		{
			// If didn't find a queue that supports both graphics and present, then
			// find a separate present queue.
			for (uint i = 0; i < m_vkCommandQueueFamilyCount; ++i)
			{
				if (supportsPresent[i] == VK_TRUE)
				{
					presentQueueFamilyIndex = i;
					break;
				}
			}
		}

		// Generate error if could not find both a graphics and a present queue
		VG_ASSERT(UINT32_MAX != graphicsQueueFamilyIndex && UINT32_MAX != computeQueueFamilyIndex && UINT32_MAX != presentQueueFamilyIndex, "Could not find both graphics and present queues");

		m_vkCommandQueueFamilyIndex[asInteger(CommandQueueType::Graphics)] = graphicsQueueFamilyIndex;
		m_vkCommandQueueFamilyIndex[asInteger(CommandQueueType::Compute)] = computeQueueFamilyIndex;
		m_vkCommandQueueFamilyIndex[asInteger(CommandQueueType::Present)] = presentQueueFamilyIndex;

		m_useSeparatePresentCommandQueue = (graphicsQueueFamilyIndex != presentQueueFamilyIndex);
		
		VG_SAFE_FREE(supportsPresent);
		VG_SAFE_FREE(queue_props);
	}

    //--------------------------------------------------------------------------------------
    VkPresentModeKHR Device::VSyncToVkPresentModeKHR(VSync mode)
	{
		switch (mode)
		{
			default:
			case VSync::None:
				return VK_PRESENT_MODE_IMMEDIATE_KHR;
				break;

			case VSync::VBL_1:
			case VSync::VBL_2:
			case VSync::VBL_3:
			case VSync::VBL_4:
				return VK_PRESENT_MODE_FIFO_KHR;
		}
	}

    //--------------------------------------------------------------------------------------
    void Device::setVSync(VSync mode)
    {
		auto vkPresentMode = VSyncToVkPresentModeKHR(m_VSync);
		if (vkPresentMode != m_vkPresentMode)
		{
			m_vkPresentMode = vkPresentMode;
			m_vkDirtySwapchain = true;
		}
    }

	//--------------------------------------------------------------------------------------
	void Device::createSwapchain()
	{
		const DeviceParams & deviceParams = getDeviceParams();

		VkSwapchainKHR oldSwapchain = m_vkSwapchain;

		// Check the surface capabilities and formats
		VkSurfaceCapabilitiesKHR surfCapabilities;
		VG_ASSERT_VULKAN(m_KHR_Surface.m_pfnGetPhysicalDeviceSurfaceCapabilitiesKHR(m_vkPhysicalDevice, m_vkSurface, &surfCapabilities));

		uint32_t presentModeCount;
		VG_ASSERT_VULKAN(m_KHR_Surface.m_pfnGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, m_vkSurface, &presentModeCount, nullptr));

		VkPresentModeKHR *presentModes = (VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
		VG_ASSERT(presentModes);

		VG_ASSERT_VULKAN(m_KHR_Surface.m_pfnGetPhysicalDeviceSurfacePresentModesKHR(m_vkPhysicalDevice, m_vkSurface, &presentModeCount, presentModes));

		VkExtent2D swapchainExtent;
		// width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
		if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) 
		{
			// If the surface size is undefined, the size is set to the size
			// of the images requested, which must fit within the minimum and
			// maximum values.
			swapchainExtent.width  = deviceParams.resolution.x; 
			swapchainExtent.height = deviceParams.resolution.y;

			if (swapchainExtent.width < surfCapabilities.minImageExtent.width) 
				swapchainExtent.width = surfCapabilities.minImageExtent.width;
			else if (swapchainExtent.width > surfCapabilities.maxImageExtent.width) 
				swapchainExtent.width = surfCapabilities.maxImageExtent.width;

			if (swapchainExtent.height < surfCapabilities.minImageExtent.height) 
				swapchainExtent.height = surfCapabilities.minImageExtent.height;
			else if (swapchainExtent.height > surfCapabilities.maxImageExtent.height) 
				swapchainExtent.height = surfCapabilities.maxImageExtent.height;
		}
		else 
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCapabilities.currentExtent;

			VG_ASSERT(deviceParams.resolution.x == surfCapabilities.currentExtent.width);
			VG_ASSERT(deviceParams.resolution.y == surfCapabilities.currentExtent.height);
		}

		// The FIFO present mode is guaranteed by the spec to be supported
		// and to have no tearing.  It's a great default present mode to use.
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		//  There are times when you may wish to use another present mode.  The
		//  following code shows how to select them, and the comments provide some
		//  reasons you may wish to use them.
		//
		// It should be noted that Vulkan 1.0 doesn't provide a method for
		// synchronizing rendering with the presentation engine's display.  There
		// is a method provided for throttling rendering with the display, but
		// there are some presentation engines for which this method will not work.
		// If an application doesn't throttle its rendering, and if it renders much
		// faster than the refresh rate of the display, this can waste power on
		// mobile devices.  That is because power is being spent rendering images
		// that may never be seen.

		// VK_PRESENT_MODE_IMMEDIATE_KHR is for applications that don't care about
		// tearing, or have some way of synchronizing their rendering with the
		// display.
		// VK_PRESENT_MODE_MAILBOX_KHR may be useful for applications that
		// generally render a new presentable image every refresh cycle, but are
		// occasionally early.  In this case, the application wants the new image
		// to be displayed instead of the previously-queued-for-presentation image
		// that has not yet been displayed.
		// VK_PRESENT_MODE_FIFO_RELAXED_KHR is for applications that generally
		// render a new presentable image every refresh cycle, but are occasionally
		// late.  In this case (perhaps because of stuttering/latency concerns),
		// the application wants the late image to be immediately displayed, even
		// though that may mean some tearing.

		if (m_vkPresentMode != swapchainPresentMode) 
		{
			for (size_t i = 0; i < presentModeCount; ++i) 
			{
				if (presentModes[i] == m_vkPresentMode) 
				{
					swapchainPresentMode = m_vkPresentMode;
					break;
				}
			}
		}

		VG_ASSERT(swapchainPresentMode == m_vkPresentMode, "Present mode specified is not supported");

		// Determine the number of VkImages to use in the swap chain.
		// Application desires to acquire 3 images at a time for triple
		// buffering
		uint32_t desiredNumOfSwapchainImages = max_backbuffer_count;
		if (desiredNumOfSwapchainImages < surfCapabilities.minImageCount) 
			desiredNumOfSwapchainImages = surfCapabilities.minImageCount;

		// If maxImageCount is 0, we can ask for as many images as we want;
		// otherwise we're limited to maxImageCount
		if ((surfCapabilities.maxImageCount > 0) && (desiredNumOfSwapchainImages > surfCapabilities.maxImageCount)) 
			// Application must settle for fewer images than desired:
			desiredNumOfSwapchainImages = surfCapabilities.maxImageCount;

		VkSurfaceTransformFlagBitsKHR preTransform;
		if (surfCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) 
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		else 
			preTransform = surfCapabilities.currentTransform;

		// Find a supported composite alpha mode - one of these is guaranteed to be set
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		VkCompositeAlphaFlagBitsKHR compositeAlphaFlags[4] = 
		{
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (uint32_t i = 0; i < countof(compositeAlphaFlags); i++) 
		{
			if (surfCapabilities.supportedCompositeAlpha & compositeAlphaFlags[i]) 
			{
				compositeAlpha = compositeAlphaFlags[i];
				break;
			}
		}

		VkSwapchainCreateInfoKHR swapchain_ci;
								 swapchain_ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
								 swapchain_ci.pNext = nullptr;
								 swapchain_ci.flags = 0;
								 swapchain_ci.surface = m_vkSurface;
								 swapchain_ci.minImageCount = desiredNumOfSwapchainImages;
								 swapchain_ci.imageFormat = Texture::getVulkanPixelFormat(m_backbufferFormat);
								 swapchain_ci.imageColorSpace = m_vkColorSpace;
								 swapchain_ci.imageExtent.width = swapchainExtent.width;
								 swapchain_ci.imageExtent.height = swapchainExtent.height;
								 swapchain_ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
								 swapchain_ci.preTransform = preTransform;
								 swapchain_ci.compositeAlpha = compositeAlpha;
								 swapchain_ci.imageArrayLayers = 1;
								 swapchain_ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
								 swapchain_ci.queueFamilyIndexCount = 0;
								 swapchain_ci.pQueueFamilyIndices = nullptr;
								 swapchain_ci.presentMode = swapchainPresentMode;
								 swapchain_ci.oldSwapchain = oldSwapchain;
								 swapchain_ci.clipped = true;
		
		VG_ASSERT_VULKAN(m_KHR_Swapchain.m_pfnCreateSwapchainKHR(m_vkDevice, &swapchain_ci, nullptr, &m_vkSwapchain));

		// If we just re-created an existing swapchain, we should destroy the old swapchain at this point.
		// Note: destroying the swapchain also cleans up all its associated presentable images once the platform is done with them.
        if (oldSwapchain != VK_NULL_HANDLE)
        {
            m_KHR_Swapchain.m_pfnDestroySwapchainKHR(m_vkDevice, oldSwapchain, nullptr);
        
            for (uint i = 0; i < countof(m_bufferContext); ++i)
            {
				if (nullptr != m_bufferContext[i].backbuffer)
				{
					m_bufferContext[i].backbuffer->getResource().setVulkanImage(VK_NULL_HANDLE, VK_NULL_HANDLE);
					VG_SAFE_RELEASE(m_bufferContext[i].backbuffer);
				}
            }
        }

		VG_ASSERT_VULKAN(m_KHR_Swapchain.m_pfnGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &m_vkSwapchainImageCount, nullptr));
		
		VG_SAFE_FREE(presentModes);
	}

    //--------------------------------------------------------------------------------------
    void Device::destroySwapchain()
	{
		m_KHR_Swapchain.m_pfnDestroySwapchainKHR(getVulkanDevice(), m_vkSwapchain, nullptr);
	}

    //--------------------------------------------------------------------------------------
    PixelFormat Device::detectBackbufferFormat()
    {
        u32 count;
        VG_ASSERT_VULKAN(m_KHR_Surface.m_pfnGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, m_vkSurface, &count, nullptr));

        vector<VkSurfaceFormatKHR> availableSurfaceFormats;
        availableSurfaceFormats.resize(count);

        VG_ASSERT_VULKAN(m_KHR_Surface.m_pfnGetPhysicalDeviceSurfaceFormatsKHR(m_vkPhysicalDevice, m_vkSurface, &count, availableSurfaceFormats.data()));

        VkSurfaceFormatKHR selectedSurfaceFormat = { VK_FORMAT_UNDEFINED };

        for (const VkSurfaceFormatKHR & curSurfaceFormat : availableSurfaceFormats)
        {
            const VkFormat format = curSurfaceFormat.format;

            if (format == VK_FORMAT_R8G8B8A8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM)
            {
				VG_INFO("[Device] Backbuffer uses %s format", asString(Texture::getPixelFormat(format)).c_str());
                selectedSurfaceFormat = curSurfaceFormat;
                break;
            }
        }

        VG_ASSERT(VK_FORMAT_UNDEFINED != selectedSurfaceFormat.format, "Could not find compatible backbuffer format");
            
        // temp: save colorspace here for now, might add a plaform-agnostic colorspace enum later
        m_vkColorSpace = selectedSurfaceFormat.colorSpace;

        return Texture::getPixelFormat(selectedSurfaceFormat.format);
    }

	//--------------------------------------------------------------------------------------
	void Device::createVulkanDevice()
	{
		float queue_priorities[1] = { 0.0 };

		VkDeviceQueueCreateInfo queues[2];
								queues[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
								queues[0].pNext = NULL;
								queues[0].queueFamilyIndex = getVulkanCommandQueueFamilyIndex(CommandQueueType::Graphics);
								queues[0].queueCount = 1;
								queues[0].pQueuePriorities = queue_priorities;
								queues[0].flags = 0;

		// TODO : clean this mess and declared chained structs only once

		VkPhysicalDeviceRayQueryFeaturesKHR rayQuerySupportedFeatures = {};
		rayQuerySupportedFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
		rayQuerySupportedFeatures.pNext = nullptr;

		VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureSupportedFeatures = {};
		accelerationStructureSupportedFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		accelerationStructureSupportedFeatures.pNext = &rayQuerySupportedFeatures;

        VkPhysicalDeviceVulkan12Features vulkan12SupportedFeatures = {};
        vulkan12SupportedFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vulkan12SupportedFeatures.pNext = &accelerationStructureSupportedFeatures;

        VkPhysicalDeviceFeatures2 supportedFeatures = {};
		supportedFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		supportedFeatures.pNext = &vulkan12SupportedFeatures;

        vkGetPhysicalDeviceFeatures2(m_vkPhysicalDevice, &supportedFeatures);

        VkPhysicalDeviceAccelerationStructurePropertiesKHR accelerationStructureProps = {};
		accelerationStructureProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
		accelerationStructureProps.pNext = nullptr;

		VkPhysicalDeviceProperties2 physicalDeviceProps;
		physicalDeviceProps.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		physicalDeviceProps.pNext = &accelerationStructureProps;

		vkGetPhysicalDeviceProperties2(m_vkPhysicalDevice, &physicalDeviceProps);

		// Query RayTracing acceleration structure alignment
        if (m_caps.supportRayTracing)
            m_caps.rayTracingAccelerationStructureScratchOffsetAlignment = accelerationStructureProps.minAccelerationStructureScratchOffsetAlignment;

        VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures = {};
		rayQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
		rayQueryFeatures.pNext = nullptr;

		VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};
        accelerationStructureFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		accelerationStructureFeatures.pNext = &rayQueryFeatures; 

        VkPhysicalDeviceVulkan12Features vulkan12Features = {};
        vulkan12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vulkan12Features.pNext = &accelerationStructureFeatures;

		#define CheckVulkanFeature(supported, destination, name, value)	if (!supported.name)																		\
																		{																							\
																			VG_ASSERT(supported.name, "\"%s\" is not supported by Vulkan device", #name);			\
																			destination.name = value;																\
																		}																							\
																		else																						\
																		{																							\
																			VG_INFO("[Device] Feature \"%s\" is enabled", #name);									\
																			destination.name = value;																\
																		}

		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, descriptorIndexing, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, descriptorBindingPartiallyBound, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, shaderSampledImageArrayNonUniformIndexing, true);
		//CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, shaderInputAttachmentArrayDynamicIndexing, true);
		//CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, shaderInputAttachmentArrayNonUniformIndexing, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, shaderUniformTexelBufferArrayDynamicIndexing, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, runtimeDescriptorArray, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, shaderStorageImageArrayNonUniformIndexing, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, descriptorBindingUpdateUnusedWhilePending, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, descriptorBindingUniformBufferUpdateAfterBind, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, descriptorBindingSampledImageUpdateAfterBind, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, descriptorBindingStorageImageUpdateAfterBind, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, descriptorBindingStorageBufferUpdateAfterBind, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, descriptorBindingUniformTexelBufferUpdateAfterBind, true);
		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, descriptorBindingStorageTexelBufferUpdateAfterBind, true);

		CheckVulkanFeature(vulkan12SupportedFeatures, vulkan12Features, bufferDeviceAddress, true);

		if (m_caps.supportRayTracing)
		{
			CheckVulkanFeature(accelerationStructureSupportedFeatures, accelerationStructureFeatures, accelerationStructure, true);
			CheckVulkanFeature(accelerationStructureSupportedFeatures, accelerationStructureFeatures, descriptorBindingAccelerationStructureUpdateAfterBind, true);
			CheckVulkanFeature(rayQuerySupportedFeatures, rayQueryFeatures, rayQuery, true);
		}

        VkPhysicalDeviceFeatures enabledFeatures = {};
		CheckVulkanFeature(supportedFeatures.features, enabledFeatures, fillModeNonSolid, true);
		CheckVulkanFeature(supportedFeatures.features, enabledFeatures, fragmentStoresAndAtomics, true);
		//CheckVulkanFeature(supportedFeatures.features, enabledFeatures, textureCompressionETC2, true);

		VkDeviceCreateInfo deviceCreateInfo; 
						   deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
						   deviceCreateInfo.pNext = nullptr;
						   deviceCreateInfo.flags = 0;
						   deviceCreateInfo.queueCreateInfoCount = 1;
						   deviceCreateInfo.pQueueCreateInfos = queues;
						   deviceCreateInfo.enabledLayerCount = 0;
						   deviceCreateInfo.ppEnabledLayerNames = nullptr;
						   deviceCreateInfo.enabledExtensionCount = m_deviceExtensionList.getEnabledExtensionCount();
						   deviceCreateInfo.ppEnabledExtensionNames = m_deviceExtensionList.getEnabledExtensionNames();
						   deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
                           deviceCreateInfo.pNext = &vulkan12Features;

		if (m_useSeparatePresentCommandQueue)
		{
			queues[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queues[1].pNext = NULL;
			queues[1].queueFamilyIndex = getVulkanCommandQueueFamilyIndex(CommandQueueType::Present);
			queues[1].queueCount = 1;
			queues[1].pQueuePriorities = queue_priorities;
			queues[1].flags = 0;

			deviceCreateInfo.queueCreateInfoCount = 2;
		}
		VG_ASSERT_VULKAN(vkCreateDevice(m_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_vkDevice));

		m_deviceExtensionList.onDeviceCreated();

        m_memoryAllocator = new gfx::MemoryAllocator();

		auto * graphicsQueue = createCommandQueue(CommandQueueType::Graphics);
		
		if (m_useSeparatePresentCommandQueue)
			auto * presentQueue = createCommandQueue(CommandQueueType::Present);

        m_backbufferFormat = detectBackbufferFormat();

		// Create semaphores to synchronize acquiring presentable buffers before rendering and waiting for drawing to be complete before presenting
		VkSemaphoreCreateInfo semaphoreCreateInfo;
							  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
							  semaphoreCreateInfo.pNext = nullptr;
							  semaphoreCreateInfo.flags = 0;

		// Create fences that we can use to throttle if we get too far ahead of the image presents
		VkFenceCreateInfo fence_ci;
						  fence_ci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
						  fence_ci.pNext = nullptr;
						  fence_ci.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < max_frame_latency; i++)
		{
			VG_ASSERT_VULKAN(vkCreateFence(m_vkDevice, &fence_ci, nullptr, &m_vkFences[i]));
			VG_ASSERT_VULKAN(vkCreateSemaphore(m_vkDevice, &semaphoreCreateInfo, nullptr, &m_vkImageAcquiredSemaphores[i]));
			VG_ASSERT_VULKAN(vkCreateSemaphore(m_vkDevice, &semaphoreCreateInfo, nullptr, &m_vkDrawCompleteSemaphores[i]));
			
			if (m_useSeparatePresentCommandQueue)
				VG_ASSERT_VULKAN(vkCreateSemaphore(m_vkDevice, &semaphoreCreateInfo, nullptr, &m_vkImageOwnershipSemaphores[i]));
		}

        m_nextFrameIndex = 0;

		// Get Memory information and properties
		vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceMemoryProperties);
	}
 
    //--------------------------------------------------------------------------------------
    void Device::waitGPUIdle()
    {
        vkDeviceWaitIdle(m_vkDevice);

        // Wait for fences from present operations
        for (uint i = 0; i < max_frame_latency; i++)
            vkWaitForFences(m_vkDevice, 1, &m_vkFences[i], VK_TRUE, UINT64_MAX);
    }

    //--------------------------------------------------------------------------------------
    void Device::createVulkanBackbuffers()
    {
        VkImage * swapchainImages = (VkImage *)malloc(m_vkSwapchainImageCount * sizeof(VkImage));
        assert(swapchainImages);
        VG_ASSERT_VULKAN(m_KHR_Swapchain.m_pfnGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &m_vkSwapchainImageCount, swapchainImages));

        for (uint i = 0; i < max_backbuffer_count; ++i)
            createBackbuffer(i, &swapchainImages[i]);

        VG_SAFE_FREE(swapchainImages);
    }

    //--------------------------------------------------------------------------------------
    void Device::destroyVulkanBackbuffers()
    {
        for (uint i = 0; i < countof(m_bufferContext); ++i)
            destroyBackbuffer(i);
    }

	//--------------------------------------------------------------------------------------
	void Device::deinit()
	{
        for (uint i = 0; i < max_frame_latency; i++)
        {
            vkDestroyFence(m_vkDevice, m_vkFences[i], nullptr);
            vkDestroySemaphore(m_vkDevice, m_vkImageAcquiredSemaphores[i], nullptr);
            vkDestroySemaphore(m_vkDevice, m_vkDrawCompleteSemaphores[i], nullptr);
        }

        static_cast<gfx::Device*>(this)->removeRootSignature(m_bindlessRootSignatureHandle);

        for (auto & pair : m_vkRenderPassHash)
            vkDestroyRenderPass(m_vkDevice, pair.second, nullptr);

        m_vkRenderPassHash.clear();

		destroyCommandQueues();
        destroyUploadBuffer();

		for (uint i = 0; i < countof(m_frameContext); ++i)
			destroyFrameContext(i);

        destroyVulkanBackbuffers();
        destroySwapchain();

        VG_SAFE_DELETE(m_bindlessTable);
        VG_SAFE_RELEASE(m_memoryAllocator);

        vkDestroyDescriptorPool(m_vkDevice, m_vkBindlessDescriptorPool, nullptr);

        for (auto & vkSampler : m_vkSampler)
            vkDestroySampler(m_vkDevice, vkSampler, nullptr);

        vkDestroyDescriptorPool(m_vkDevice, m_vkSamplerDescriptorPool, nullptr);

		m_instanceExtensionList.deinit();
		m_deviceExtensionList.deinit();

		base::Device::deinit();
	}

	//--------------------------------------------------------------------------------------
	VkInstance & Device::getVulkanInstance()
	{
		return m_vkInstance;
	}

	//--------------------------------------------------------------------------------------
	VkDevice & Device::getVulkanDevice()
	{
		return m_vkDevice;
	}

	//--------------------------------------------------------------------------------------
	VkPhysicalDevice & Device::getVulkanPhysicalDevice()
	{
		return m_vkPhysicalDevice;
	}

    //--------------------------------------------------------------------------------------
    VmaAllocator & Device::getVulkanMemoryAllocator() 
    { 
        return m_memoryAllocator->getVulkanMemoryAllocator(); 
    }

    //--------------------------------------------------------------------------------------
    void Device::resize(core::uint _width, core::uint _height)
    {
		destroyVulkanBackbuffers();
        createSwapchain();
        createVulkanBackbuffers();   
    }

	//--------------------------------------------------------------------------------------
	void Device::beginFrame()
	{
		super::beginFrame();

        VkDevice & vkDevice = getVulkanDevice();

        // Get/Increment the frame ring-buffer index
        UINT FrameIndex = m_nextFrameIndex;
        m_nextFrameIndex = (m_nextFrameIndex + 1) % (UINT)max_frame_latency;

		u32 currentBuffer;

		// Scope WaitGPU/VSync
		{
			VG_PROFILE_CPU("Wait");
			// Ensure no more than max_frame_latency renderings are outstanding
			//if (m_frameCounter >= max_frame_latency)
			{
				#if VG_DBG_CPUGPUSYNC
				VG_DEBUGPRINT("Wait completion of frame %u (fence[%u])\n", m_frameCounter - max_frame_latency, FrameIndex);
				#endif

				VG_PROFILE_GPU_SWAP(this);
				vkWaitForFences(vkDevice, 1, &m_vkFences[FrameIndex], VK_TRUE, UINT64_MAX);
			}
			vkResetFences(vkDevice, 1, &m_vkFences[FrameIndex]);
			m_currentFrameIndex = FrameIndex;

			VG_ASSERT_VULKAN(m_KHR_Swapchain.m_pfnAcquireNextImageKHR(vkDevice, m_vkSwapchain, UINT64_MAX, m_vkImageAcquiredSemaphores[FrameIndex], VK_NULL_HANDLE, &currentBuffer));
		}

        m_currentBackbufferIndex = currentBuffer;

        auto & context = getCurrentFrameContext();

        for (auto & cmdPool : context.commandPools)
            cmdPool->beginFrame();

        for (uint type = 0; type < enumCount<CommandListType>(); ++type)
            for (auto & cmdList : context.commandLists[type])
                cmdList->reset();
	}

	//--------------------------------------------------------------------------------------
	void Device::endFrame()
	{
        const auto currentFrameIndex = getFrameContextIndex();
        FrameContext & context = getCurrentFrameContext();

        for (uint q = 0; q < enumCount<CommandQueueType>(); ++q)
        {
            const auto cmdQueueType = (CommandQueueType)q;
            auto * queue = getCommandQueue(cmdQueueType);

            const auto & queueCmdLists = context.commandLists[q];
            vector<VkCommandBuffer> cmdBuffersToExecute;
            for (uint c = 0; c < queueCmdLists.size(); ++c)
            {
                auto * cmdList = queueCmdLists[c];
                cmdList->close();
                VkCommandBuffer & vkCmdBuffer = cmdList->getVulkanCommandBuffer();
                cmdBuffersToExecute.push_back(vkCmdBuffer);
            }

            if (cmdBuffersToExecute.size() > 0)
            {
                VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                VkSubmitInfo submit_info = {};
                submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submit_info.pWaitDstStageMask = &pipe_stage_flags;
                submit_info.waitSemaphoreCount = 1;
                submit_info.pWaitSemaphores = &m_vkImageAcquiredSemaphores[currentFrameIndex];
                submit_info.commandBufferCount = (uint)cmdBuffersToExecute.size();
                submit_info.pCommandBuffers = cmdBuffersToExecute.data();
                submit_info.signalSemaphoreCount = 1;
                submit_info.pSignalSemaphores = &m_vkDrawCompleteSemaphores[currentFrameIndex];

                VG_ASSERT_VULKAN(vkQueueSubmit(queue->getVulkanCommandQueue(), 1, &submit_info, m_vkFences[currentFrameIndex]));
            }
        }

        #if VG_DBG_CPUGPUSYNC
        VG_DEBUGPRINT("Write fence %u\n", currentFrameIndex);
        #endif

        VkPresentInfoKHR present;

        present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present.pNext = nullptr;
        present.waitSemaphoreCount = 1;
        present.pWaitSemaphores = &m_vkDrawCompleteSemaphores[currentFrameIndex];
        present.swapchainCount = 1;
        present.pSwapchains = &m_vkSwapchain;
        const u32 currentBackbuffer = m_currentBackbufferIndex;
        present.pImageIndices = &currentBackbuffer;
        present.pResults = nullptr;

        VG_ASSERT_VULKAN(m_KHR_Swapchain.m_pfnQueuePresentKHR(getCommandQueue(CommandQueueType::Graphics)->getVulkanCommandQueue(), &present));

		if (m_vkDirtySwapchain)
		{
			waitGPUIdle();
            createSwapchain();
            createVulkanBackbuffers();
			m_vkDirtySwapchain = false;
		}

		super::endFrame();
	}

    //--------------------------------------------------------------------------------------
    VkRenderPass Device::getVulkanRenderPass(const RenderPassKey & _key)
    {
        VkRenderPass vkRenderPass;

        auto it = m_vkRenderPassHash.find(_key);
        if (m_vkRenderPassHash.end() == it)
        {
            vkRenderPass = RenderPass::createVulkanRenderPassFromKey(_key);
            m_vkRenderPassHash[_key] = vkRenderPass;
            return vkRenderPass;
        }
        return it->second;
    }

    //--------------------------------------------------------------------------------------
    // TODO: increase RefCount on 'get' and release ?
    //--------------------------------------------------------------------------------------
    uint Device::releaseVulkanRenderPass(const RenderPassKey & _key, VkRenderPass & _vkRenderPass)
    {
        _vkRenderPass = VK_NULL_HANDLE;
        return (uint)-1;
    }

    //--------------------------------------------------------------------------------------
    const char * Device::getVulkanErrorString(VkResult _vkResult)
    {
        switch (_vkResult)
        {
            // Success Codes
            case VK_SUCCESS:
                return "VK_SUCCESS\nCommand successfully completed.";
            case VK_NOT_READY:
                return "VK_NOT_READY\nA fence or query has not yet completed.";
            case VK_TIMEOUT:
                return "VK_TIMEOUT\nA wait operation has not completed in the specified time.";
            case VK_EVENT_SET:
                return "VK_EVENT_SET\nAn event is signaled.";
            case VK_EVENT_RESET:
                return "VK_EVENT_RESET\nAn event is unsignaled.";
            case VK_INCOMPLETE:
                return "VK_INCOMPLETE\nA return array was too small for the result.";
            case VK_THREAD_IDLE_KHR:
                return "VK_THREAD_IDLE_KHR\nA deferred operation is not complete but there is currently no work for this thread to do at the time of this call..";
            case VK_THREAD_DONE_KHR:
                return "VK_THREAD_DONE_KHR\nA deferred operation is not complete but there is no work remaining to assign to additional threads.";
            case VK_OPERATION_DEFERRED_KHR:
                return "VK_OPERATION_DEFERRED_KHR\nA deferred operation was requested and at least some of the work was deferred..";
            case VK_OPERATION_NOT_DEFERRED_KHR:
                return "VK_OPERATION_NOT_DEFERRED_KHR\nA deferred operation was requested and no operations were deferred..";
            case VK_PIPELINE_COMPILE_REQUIRED_EXT:
                return "VK_PIPELINE_COMPILE_REQUIRED_EXT\nA requested pipeline creation would have required compilation, but the application requested compilation to not be performed..";

            // Error codes
            default:
            case VK_ERROR_UNKNOWN:
                return "VK_ERROR_UNKNOWN\nAn unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred.";
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return "VK_ERROR_OUT_OF_HOST_MEMORY\nA host memory allocation has failed..";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return "VK_ERROR_OUT_OF_DEVICE_MEMORY\nA device memory allocation has failed.";
            case VK_ERROR_INITIALIZATION_FAILED:
                return "VK_ERROR_INITIALIZATION_FAILED\nInitialization of an object could not be completed for implementation-specific reasons.";
            case VK_ERROR_DEVICE_LOST:
                return "VK_ERROR_DEVICE_LOST\nThe logical or physical device has been lost. See Lost Device.";
            case VK_ERROR_MEMORY_MAP_FAILED:
                return "VK_ERROR_MEMORY_MAP_FAILED\nMapping of a memory object has failed.";
            case VK_ERROR_LAYER_NOT_PRESENT:
                return "VK_ERROR_LAYER_NOT_PRESENT\nA requested layer is not present or could not be loaded.";
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                return "VK_ERROR_EXTENSION_NOT_PRESENT\nA requested extension is not supported.";
            case VK_ERROR_FEATURE_NOT_PRESENT:
                return "VK_ERROR_FEATURE_NOT_PRESENT\nA requested feature is not supported.";
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                return "VK_ERROR_INCOMPATIBLE_DRIVER\nThe requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.";
            case VK_ERROR_TOO_MANY_OBJECTS:
                return "VK_ERROR_TOO_MANY_OBJECTS\nToo many objects of the type have already been created.";
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                return "VK_ERROR_FORMAT_NOT_SUPPORTED\nA requested format is not supported on this device.";
            case VK_ERROR_FRAGMENTED_POOL:
                return "VK_ERROR_FRAGMENTED_POOL\nA pool allocation has failed due to fragmentation of the pool’s memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation.";             
            case VK_ERROR_OUT_OF_POOL_MEMORY:
                return "VK_ERROR_OUT_OF_POOL_MEMORY\nA pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead.";
            case VK_ERROR_INVALID_EXTERNAL_HANDLE:
                return "VK_ERROR_INVALID_EXTERNAL_HANDLE\nAn external handle is not a valid handle of the specified type.";
            case VK_ERROR_FRAGMENTATION:
                return "VK_ERROR_FRAGMENTATION\nA descriptor pool creation has failed due to fragmentation.";
            case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
                return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS\nA buffer creation or memory allocation failed because the requested address is not available. A shader group handle assignment failed because the requested shader group handle information is no longer valid.";
            case VK_ERROR_SURFACE_LOST_KHR:
                return "VK_ERROR_SURFACE_LOST_KHR\nA surface is no longer available.";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR\nThe requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.";
            case VK_SUBOPTIMAL_KHR:
                return "VK_SUBOPTIMAL_KHR\nA swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.";
            case VK_ERROR_OUT_OF_DATE_KHR:
                return "VK_ERROR_OUT_OF_DATE_KHR\nA surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface.";
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR\nThe display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.";
            case VK_ERROR_VALIDATION_FAILED_EXT:
                return "VK_ERROR_VALIDATION_FAILED_EXT";
            case VK_ERROR_INVALID_SHADER_NV:
                return "VK_ERROR_INVALID_SHADER_NV\nOne or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled.";
            case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
                return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
            case VK_ERROR_NOT_PERMITTED_EXT:
                return "VK_ERROR_NOT_PERMITTED_EXT";
            case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
                return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT\nAn operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application’s control."; 
			#if VK_HEADER_VERSION >= 135 && VK_HEADER_VERSION < 162
			case VK_ERROR_INCOMPATIBLE_VERSION_KHR:
				return "VK_ERROR_INCOMPATIBLE_VERSION_KHR";
			#endif
		}
    }

    //--------------------------------------------------------------------------------------
	// KHR_Acceleration_Structure extension funcs
	//--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
    void Device::getAccelerationStructureBuildSizes(VkAccelerationStructureBuildTypeKHR _buildType, const VkAccelerationStructureBuildGeometryInfoKHR * _buildInfo, const uint32_t * _maxPrimitiveCounts, VkAccelerationStructureBuildSizesInfoKHR * _sizeInfo)
    {
        VG_ASSERT(m_KHR_Acceleration_Structure.m_pfnGetAccelerationStructureBuildSizesKHR);
        m_KHR_Acceleration_Structure.m_pfnGetAccelerationStructureBuildSizesKHR(m_vkDevice, _buildType, _buildInfo, _maxPrimitiveCounts, _sizeInfo);
    }

    //--------------------------------------------------------------------------------------
    VkResult Device::createAccelerationStructure(const VkAccelerationStructureCreateInfoKHR * _createInfo, const VkAllocationCallbacks * _allocator, VkAccelerationStructureKHR * _accelerationStructure)
	{
		VG_ASSERT(m_KHR_Acceleration_Structure.m_pfnCreateAccelerationStructureKHR);
		return m_KHR_Acceleration_Structure.m_pfnCreateAccelerationStructureKHR(m_vkDevice, _createInfo, _allocator, _accelerationStructure);
	}

    //--------------------------------------------------------------------------------------
	void Device::destroyAccelerationStructure(VkAccelerationStructureKHR _accelerationStructure, const VkAllocationCallbacks * _allocator)
	{
		VG_ASSERT(m_KHR_Acceleration_Structure.m_pfnDestroyAccelerationStructureKHR);
		m_KHR_Acceleration_Structure.m_pfnDestroyAccelerationStructureKHR(m_vkDevice, _accelerationStructure, _allocator);
	}

    //--------------------------------------------------------------------------------------
    // KHR_Buffer_Device_Address extension funcs
    //--------------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------------
    VkDeviceAddress Device::getBufferDeviceAddress(const VkBufferDeviceAddressInfo * _info)
	{
		VG_ASSERT(m_KHR_Buffer_Device_Address.m_pfnGetBufferDeviceAddressKHR);
		return m_KHR_Buffer_Device_Address.m_pfnGetBufferDeviceAddressKHR(m_vkDevice, _info);
	}
}