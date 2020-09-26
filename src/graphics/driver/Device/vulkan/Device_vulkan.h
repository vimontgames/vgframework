#pragma once

#include "graphics/driver/Device/Device.h"
#include "graphics/driver/Device/vulkan/ext/EXT_DebugUtils.h"
#include "graphics/driver/Device/vulkan/ext/KHR_Surface.h"
#include "graphics/driver/Device/vulkan/ext/KHR_Win32_Surface.h"
#include "graphics/driver/Device/vulkan/ext/KHR_Swapchain.h"

#include "graphics/driver/FrameGraph/RenderPassKey.h"

namespace vg::graphics::driver::vulkan
{
	class Device : public base::Device
	{
		using super = base::Device;

	public:
		void							init							(const DeviceParams & _params);
		void							deinit							();

		void							beginFrame						();
		void							endFrame						();

        void                            waitGPUIdle                     ();

		VkInstance &					getVulkanInstance				();
		VkDevice &						getVulkanDevice					();
		VkPhysicalDevice &				getVulkanPhysicalDevice			();
        VmaAllocator &                  getVulkanMemoryAllocator        ();

		bool							onDebugMessage					(VkDebugUtilsMessageSeverityFlagBitsEXT _severity, VkDebugUtilsMessageTypeFlagsEXT _flags, const VkDebugUtilsMessengerCallbackDataEXT * _data);

		core::u32						getVulkanCommandQueueFamilyIndex(CommandQueueType _type) const;
       
        VkRenderPass                    getVulkanRenderPass             (const RenderPassKey & _key);
        core::uint                      releaseVulkanRenderPass         (const RenderPassKey & _key, VkRenderPass & _vkRenderPass);

	//private:
		void							registerExtensions				(const DeviceParams & _params);

		void							createVulkanDevice				();
		void							createSwapchain					();
		void							createCommandQueues				();

        PixelFormat                     detectBackbufferFormat          ();

		static Severity					getSeverity						(VkDebugUtilsMessageSeverityFlagBitsEXT _severity);
		static const char *				getVulkanObjectTypeName			(VkObjectType _type);

        bool                            enableInstanceLayer             (const char * _layerName);
       
	//private:
			VkInstance					m_vkInstance;
			VkDevice					m_vkDevice;
			VkPhysicalDevice			m_vkPhysicalDevice;
			VkPhysicalDeviceProperties	m_vkPhysicalDeviceProperties;
            VkDescriptorPool            m_vkDescriptorPool;
			VkSurfaceKHR				m_vkSurface;

			static const core::u32		s_vkCommandQueueFamilyIndexInvalid = (core::u32)-1;
			core::u32					m_vkCommandQueueFamilyCount = 0;
			core::u32					m_vkCommandQueueFamilyIndex[core::enumCount<CommandQueueType>()];
			bool						m_useSeparatePresentCommandQueue = false;

			InstanceExtensionList		m_instanceExtensionList;
			EXT_DebugUtils				m_EXT_DebugUtils;
			KHR_Surface					m_KHR_Surface;
			KHR_Win32_Surface			m_KHR_Win32_Surface;

			DeviceExtensionList			m_deviceExtensionList;
			KHR_Swapchain				m_KHR_Swapchain;

            VkDescriptorSet             m_vkbindlessDescriptorSet[2];
            VkSampler                   vk_immutableSampler;

			friend class InstanceExtension;
			friend class DeviceExtension;

			VkDebugUtilsMessengerEXT			dbg_messenger;
			core::uint							m_validationErrors = 0;

			VkPhysicalDeviceMemoryProperties memory_properties;


			PFN_vkGetRefreshCycleDurationGOOGLE				m_pfnGetRefreshCycleDurationGOOGLE;
			PFN_vkGetPastPresentationTimingGOOGLE			m_pfnGetPastPresentationTimingGOOGLE;

			PFN_vkGetDeviceProcAddr							m_pfnGetDeviceProcAddr;

			uint32_t swapchainImageCount;
			VkSwapchainKHR m_vkSwapchain;
			VkPresentModeKHR presentMode;
			VkFence fences[max_frame_latency];
			int frame_index;

			VkSemaphore image_acquired_semaphores[max_frame_latency];
			VkSemaphore draw_complete_semaphores[max_frame_latency];
			VkSemaphore image_ownership_semaphores[max_frame_latency];

			//VkFormat format;
			VkColorSpaceKHR color_space;

            core::unordered_map<RenderPassKey, VkRenderPass, RenderPassKey::hash> m_vkRenderPassHash;

            RootSignatureHandle m_bindlessRootSignatureHandle;
	};
}