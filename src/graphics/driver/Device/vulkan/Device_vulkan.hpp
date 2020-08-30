#include "Device_Extensions_vulkan.hpp"

namespace vg::graphics::driver::vulkan
{
	using namespace vg::core;

	//--------------------------------------------------------------------------------------
	VKAPI_ATTR VkBool32 VKAPI_CALL debug_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
	{
		return static_cast<Device*>(pUserData)->onDebugMessage(messageSeverity, messageType, pCallbackData);
	}

	//--------------------------------------------------------------------------------------
	Severity Device::getSeverity(VkDebugUtilsMessageSeverityFlagBitsEXT _severity)
	{
		 if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT & _severity)
			return Severity::Error;
		 else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT & _severity)
			return Severity::Warning;
		 else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT & _severity)
			 return Severity::Info;
		 else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT & _severity)
			 return Severity::Verbose;
		 else
		 {
			 VG_ASSERT(false);
			 return Severity::Verbose;
		 }
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

		const Severity severity = getSeverity(_severity);
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

		VG_DEBUGPRINT(message);
		VG_SAFE_FREE(message);

        if ((Severity::Error == severity && deviceParams.breakOnErrors) || (Severity::Warning == severity && deviceParams.breakOnErrors))
            DebugBreak();

		return false;
	}

	//--------------------------------------------------------------------------------------
	void Device::registerExtensions(const DeviceParams & _params)
	{
		if (_params.debugDevice)
			m_instanceExtensionList.registerExtension(m_EXT_DebugUtils);

		m_instanceExtensionList.registerExtension(m_KHR_Surface);
		m_instanceExtensionList.registerExtension(m_KHR_Win32_Surface);

		m_deviceExtensionList.registerExtension(m_KHR_Swapchain);
	}

    //--------------------------------------------------------------------------------------
    bool Device::enableInstanceLayer(const char * _layerName)
    {
        core::uint enabled_layer_count = 0;
        const char * enabled_layers[64];

        bool found = false;
     
        core::u32 instance_layer_count = 0;
        VG_ASSERT_VULKAN(vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));
        VG_ASSERT(instance_layer_count <= countof(enabled_layers));

        if (instance_layer_count > 0)
        {
            auto * instance_layers = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * instance_layer_count);
            VG_ASSERT_VULKAN(vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layers));

            for (uint j = 0; j < instance_layer_count; j++)
            {
                if (!strcmp(_layerName, instance_layers[j].layerName))
                {
                    found = true;
                    VG_DEBUGPRINT("Layer \"%s\" is enabled", _layerName);
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

        if (validationLayer)
            validationLayer = enableInstanceLayer(validationLayerName);

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
			VK_API_VERSION_1_0
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

		if (validationLayer) 
			VG_ASSERT_VULKAN(m_EXT_DebugUtils.m_pfnCreateDebugUtilsMessengerEXT(m_vkInstance, &dbg_messenger_create_info, nullptr, &dbg_messenger));

		vkGetPhysicalDeviceProperties(m_vkPhysicalDevice, &m_vkPhysicalDeviceProperties);

		// Query fine-grained feature support for this device.
		//  If app has specific feature requirements it should check supported features based on this query
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
		createSwapchain();
		
		VkImage * swapchainImages = (VkImage *)malloc(swapchainImageCount * sizeof(VkImage));
		assert(swapchainImages);
		VG_ASSERT_VULKAN(m_KHR_Swapchain.m_pfnGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &swapchainImageCount, swapchainImages));

		for (uint i = 0; i < max_frame_latency; ++i)
		{
			VkImage * vulkanBackbufferResource = &swapchainImages[i];
			createFrameContext(i, vulkanBackbufferResource);
		}

		VG_SAFE_FREE(swapchainImages);
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

		if (presentMode != swapchainPresentMode) 
		{
			for (size_t i = 0; i < presentModeCount; ++i) 
			{
				if (presentModes[i] == presentMode) 
				{
					swapchainPresentMode = presentMode;
					break;
				}
			}
		}

		VG_ASSERT(swapchainPresentMode == presentMode, "Present mode specified is not supported");

		// Determine the number of VkImages to use in the swap chain.
		// Application desires to acquire 3 images at a time for triple
		// buffering
		uint32_t desiredNumOfSwapchainImages = max_frame_latency;
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
								 swapchain_ci.imageColorSpace = color_space;
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
			m_KHR_Swapchain.m_pfnDestroySwapchainKHR(m_vkDevice, oldSwapchain, nullptr);

		VG_ASSERT_VULKAN(m_KHR_Swapchain.m_pfnGetSwapchainImagesKHR(m_vkDevice, m_vkSwapchain, &swapchainImageCount, nullptr));
		
		VG_SAFE_FREE(presentModes);
	}

	static VkSurfaceFormatKHR pick_surface_format(const VkSurfaceFormatKHR *surfaceFormats, uint32_t count) 
	{
		// Prefer non-SRGB formats...
		

		return surfaceFormats[0];
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
                VG_DEBUGPRINT("Backbuffer uses %s format\n", asString(Texture::getPixelFormat(format)).c_str());
                selectedSurfaceFormat = curSurfaceFormat;
                break;
            }
        }

        VG_ASSERT(VK_FORMAT_UNDEFINED != selectedSurfaceFormat.format, "Could not find compatible backbuffer format");
            
        // temp: save colorspace here for now, might add a plaform-agnostic colorspace enum later
        color_space = selectedSurfaceFormat.colorSpace;

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
						   deviceCreateInfo.pEnabledFeatures = nullptr;

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

        m_memoryAllocator = new driver::MemoryAllocator();

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
			VG_ASSERT_VULKAN(vkCreateFence(m_vkDevice, &fence_ci, nullptr, &fences[i]));
			VG_ASSERT_VULKAN(vkCreateSemaphore(m_vkDevice, &semaphoreCreateInfo, nullptr, &image_acquired_semaphores[i]));
			VG_ASSERT_VULKAN(vkCreateSemaphore(m_vkDevice, &semaphoreCreateInfo, nullptr, &draw_complete_semaphores[i]));
			
			if (m_useSeparatePresentCommandQueue)
				VG_ASSERT_VULKAN(vkCreateSemaphore(m_vkDevice, &semaphoreCreateInfo, nullptr, &image_ownership_semaphores[i]));
		}

		frame_index = 0;

		// Get Memory information and properties
		vkGetPhysicalDeviceMemoryProperties(m_vkPhysicalDevice, &memory_properties);
	}

	//--------------------------------------------------------------------------------------
	void Device::deinit()
	{
        vkDeviceWaitIdle(m_vkDevice);

        // Wait for fences from present operations
        for (uint i = 0; i < max_frame_latency; i++)
        {
            vkWaitForFences(m_vkDevice, 1, &fences[i], VK_TRUE, UINT64_MAX);
            vkDestroyFence(m_vkDevice, fences[i], nullptr);
            vkDestroySemaphore(m_vkDevice, image_acquired_semaphores[i], nullptr);
            vkDestroySemaphore(m_vkDevice, draw_complete_semaphores[i], nullptr);
        }

		destroyCommandQueues();

		for (uint i = 0; i < countof(m_frameContext); ++i)
			destroyFrameContext(i);

        VG_SAFE_RELEASE(m_memoryAllocator);

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

    u32 currentBuffer;

	//--------------------------------------------------------------------------------------
	void Device::beginFrame()
	{
		super::beginFrame();

        VkDevice & vkDevice = getVulkanDevice();

        const auto currentFrameIndex = getFrameContextIndex();

        // Ensure no more than max_frame_latency renderings are outstanding
        if (m_frameCounter >= max_frame_latency)
        {
            VG_DEBUGPRINT("Wait completion of frame %u (fence[%u])\n", m_frameCounter - max_frame_latency, currentFrameIndex);
            vkWaitForFences(vkDevice, 1, &fences[currentFrameIndex], VK_TRUE, UINT64_MAX);
        }
        vkResetFences(vkDevice, 1, &fences[currentFrameIndex]);

        VG_ASSERT_VULKAN(m_KHR_Swapchain.m_pfnAcquireNextImageKHR(vkDevice, m_vkSwapchain, UINT64_MAX, image_acquired_semaphores[currentFrameIndex], VK_NULL_HANDLE, &currentBuffer));

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

        for (uint type = 0; type < enumCount<CommandListType>(); ++type)
            for (auto & cmdList : context.commandLists[type])
                cmdList->close();

        VkPipelineStageFlags pipe_stage_flags = 0;
        VkSubmitInfo submit_info;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.pNext = NULL;
        submit_info.pWaitDstStageMask = &pipe_stage_flags;
        pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &image_acquired_semaphores[currentFrameIndex];
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &context.commandLists[asInteger(CommandListType::Graphics)][0]->getVulkanCommandBuffer();
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &draw_complete_semaphores[currentFrameIndex];
        VG_ASSERT_VULKAN(vkQueueSubmit(m_commandQueues[asInteger(CommandQueueType::Graphics)][0]->m_vkCommandQueue, 1, &submit_info, fences[currentFrameIndex]));

        VG_DEBUGPRINT("Write fence %u\n", currentFrameIndex);

        VkPresentInfoKHR present;

        present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present.pNext = nullptr;
        present.waitSemaphoreCount = 1;
        present.pWaitSemaphores = &draw_complete_semaphores[currentFrameIndex];
        present.swapchainCount = 1;
        present.pSwapchains = &m_vkSwapchain;
        present.pImageIndices = &currentBuffer;
        present.pResults = nullptr;

        VG_ASSERT_VULKAN(m_KHR_Swapchain.m_pfnQueuePresentKHR(m_commandQueues[asInteger(CommandQueueType::Graphics)][0]->m_vkCommandQueue, &present));

		super::endFrame();
	}
}