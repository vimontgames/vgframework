#pragma once

#include "gfx/Device/Device.h"
#include "gfx/Device/vulkan/ext/EXT_DebugUtils.h"
#include "gfx/Device/vulkan/ext/KHR_Surface.h"
#include "gfx/Device/vulkan/ext/KHR_Win32_Surface.h"
#include "gfx/Device/vulkan/ext/KHR_Swapchain.h"
#include "gfx/PipelineState/SamplerState_consts.h"
#include "gfx/FrameGraph/RenderPassKey.h"

namespace vg::gfx::vulkan
{
	class Device : public base::Device
	{
		using super = base::Device;

	public:
		void							init							(const DeviceParams & _params);
		void							deinit							();

        void                            resize                          (core::uint _width, core::uint _height);

		void							beginFrame						();
		void							endFrame						();

        void                            waitGPUIdle                     ();
        void							setVSync						(VSync mode);

		VkInstance &					getVulkanInstance				();
		VkDevice &						getVulkanDevice					();
		VkPhysicalDevice &				getVulkanPhysicalDevice			();
        VmaAllocator &                  getVulkanMemoryAllocator        ();

		bool							onDebugMessage					(VkDebugUtilsMessageSeverityFlagBitsEXT _severity, VkDebugUtilsMessageTypeFlagsEXT _flags, const VkDebugUtilsMessengerCallbackDataEXT * _data);

		core::u32						getVulkanCommandQueueFamilyIndex(CommandQueueType _type) const;
       
        VkRenderPass                    getVulkanRenderPass             (const RenderPassKey & _key);
        core::uint                      releaseVulkanRenderPass         (const RenderPassKey & _key, VkRenderPass & _vkRenderPass);

        static const char *             getVulkanErrorString            (VkResult _vkResult);

	//private:
		void							registerExtensions				(const DeviceParams & _params);

		void							createVulkanDevice				();
		void							createSwapchain					();
		void							createCommandQueues				();

        PixelFormat                     detectBackbufferFormat          ();

        void                            initSamplerStates               ();

		static Severity					getSeverity						(VkDebugUtilsMessageSeverityFlagBitsEXT _severity);
		static const char *				getVulkanObjectTypeName			(VkObjectType _type);

        bool                            enableInstanceLayer             (const char * _layerName);

        void                            createVulkanBackbuffers         ();
        void                            destroyVulkanBackbuffers        ();
        VkSwapchainKHR                  getVulkanSwapchain              () const { return m_vkSwapchain; }

		VkDescriptorSet					getVulkanBindlessDescriptors	() const { return m_vkBindlessDescriptors;}

		static VkPresentModeKHR			VSyncToVkPresentModeKHR			(VSync mode);
       
	private:
			VkInstance					m_vkInstance;
			VkDevice					m_vkDevice;
			VkPhysicalDevice			m_vkPhysicalDevice;
			VkPhysicalDeviceProperties	m_vkPhysicalDeviceProperties;
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

            VkDescriptorSet             m_vkBindlessDescriptors;
            VkDescriptorPool            m_vkBindlessDescriptorPool;

            VkDescriptorSet             m_vkSamplerDescriptors;
            VkDescriptorPool            m_vkSamplerDescriptorPool;
            VkSampler                   m_vkSampler[core::enumCount<Sampler>()];

			VkDebugUtilsMessengerEXT	m_vkDebugMessenger;
			core::uint					m_validationErrors = 0;

			VkPhysicalDeviceMemoryProperties m_vkPhysicalDeviceMemoryProperties;

			uint32_t					m_vkSwapchainImageCount;
			VkSwapchainKHR				m_vkSwapchain;
			bool						m_vkDirtySwapchain = false;
			VkPresentModeKHR			m_vkPresentMode;
			VkFence						m_vkFences[max_frame_latency];
            VkSemaphore					m_vkImageAcquiredSemaphores[max_frame_latency];
            VkSemaphore					m_vkDrawCompleteSemaphores[max_frame_latency];
            VkSemaphore					m_vkImageOwnershipSemaphores[max_frame_latency];
			VkColorSpaceKHR				m_vkColorSpace;

            core::unordered_map<RenderPassKey, VkRenderPass, RenderPassKey::hash> m_vkRenderPassHash;
            RootSignatureHandle m_bindlessRootSignatureHandle;

            friend class InstanceExtension;
            friend class DeviceExtension;
			friend class BindlessTable;
			friend class CommandList;
	};
}