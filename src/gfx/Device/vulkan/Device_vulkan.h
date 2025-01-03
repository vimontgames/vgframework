#pragma once

#include "gfx/Device/Device.h"
#include "gfx/Device/vulkan/Extension/ExtensionList_vulkan.h"
#include "gfx/Device/vulkan/Extension/Device/EXT_hdr_metadata.h"
#include "gfx/Device/vulkan/Extension/Device/KHR_Acceleration_Structure.h"
#include "gfx/Device/vulkan/Extension/Device/KHR_Buffer_Device_Address.h"
#include "gfx/Device/vulkan/Extension/Device/KHR_Deferred_Host_Operations.h"
#include "gfx/Device/vulkan/Extension/Device/KHR_Multiview.h"
#include "gfx/Device/vulkan/Extension/Device/KHR_Ray_Query.h"
#include "gfx/Device/vulkan/Extension/Device/KHR_Ray_Tracing_Pipeline.h"
#include "gfx/Device/vulkan/Extension/Device/KHR_Swapchain.h"
#include "gfx/Device/vulkan/Extension/Instance/EXT_DebugUtils.h"
#include "gfx/Device/vulkan/Extension/Instance/EXT_swapchain_colorspace.h"
#include "gfx/Device/vulkan/Extension/Instance/KHR_Surface.h"
#include "gfx/Device/vulkan/Extension/Instance/KHR_Win32_Surface.h"
#include "gfx/Device/vulkan/Extension/Instance/KHR_Get_Surface_Capabilities2.h"

#include "gfx/PipelineState/Graphic/SamplerState_consts.h"
#include "gfx/FrameGraph/RenderPassKey.h"

namespace vg::gfx::vulkan
{
	class Device : public base::Device
	{
		using super = base::Device;

	public:
		void							init								(const DeviceParams & _params);
		void							deinit								();

        void                            resize								(core::uint _width, core::uint _height);

		void							beginFrame							();
		void							endFrame							();

        void                            waitGPUIdle							();

        void							applyVSync							(VSync mode);
        bool							isVSyncSupported					(VSync _mode) const;

        void                            applyHDR							(HDR _mode);
        void							applyColorSpace						(ColorSpace _mode);

		VG_INLINE VkInstance &			getVulkanInstance						();
		VG_INLINE VkDevice &			getVulkanDevice							();
		VG_INLINE VkPhysicalDevice &	getVulkanPhysicalDevice					();
		VG_INLINE VkPhysicalDeviceProperties & getVulkanPhysicalDeviceProperties();
        VmaAllocator &					getVulkanMemoryAllocator				();

		bool							onDebugMessage						(VkDebugUtilsMessageSeverityFlagBitsEXT _severity, VkDebugUtilsMessageTypeFlagsEXT _flags, const VkDebugUtilsMessengerCallbackDataEXT * _data);

		core::u32						getVulkanCommandQueueFamilyIndex	(CommandQueueType _type) const;
       
        VkRenderPass                    getVulkanRenderPass					(const RenderPassKey & _key);
        core::uint                      releaseVulkanRenderPass				(const RenderPassKey & _key, VkRenderPass & _vkRenderPass);

		VG_INLINE VkDescriptorSet		getVulkanBindlessDescriptors		() const ;

		// KHR_Acceleration_Structure extension device funcs
        void							getAccelerationStructureBuildSizes(VkAccelerationStructureBuildTypeKHR _buildType, const VkAccelerationStructureBuildGeometryInfoKHR * _buildInfo, const uint32_t * _maxPrimitiveCounts, VkAccelerationStructureBuildSizesInfoKHR * _sizeInfo);
		VkResult						createAccelerationStructure			(const VkAccelerationStructureCreateInfoKHR * _createInfo, const VkAllocationCallbacks * _allocator, VkAccelerationStructureKHR * _accelerationStructure);
        void							destroyAccelerationStructure		(VkAccelerationStructureKHR _accelerationStructure, const VkAllocationCallbacks * _allocator);
        
		// KHR_Buffer_Device_Address extension device funcs
		VkDeviceAddress					getBufferDeviceAddress				(const VkBufferDeviceAddressInfo * _info);

        static const char *             getVulkanErrorString				(VkResult _vkResult);

		 bool							updateHDR							();

		 void							beginCapture						();
		 void							endCapture							();

	private:
		void							registerExtensions					(const DeviceParams & _params);

		void							createVulkanDevice					();
		void							createSwapchain						();
        void							destroySwapchain					();
		void							createCommandQueues					(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _vkSurface);

        PixelFormat                     detectBackbufferFormat				();

        void                            initSamplerStates					();

		static Severity					getSeverity							(VkDebugUtilsMessageSeverityFlagBitsEXT _severity, const VkDebugUtilsMessengerCallbackDataEXT * _data = nullptr);
		static const char *				getVulkanObjectTypeName				(VkObjectType _type);

        bool                            enableInstanceLayer					(const char * _layerName);

        void                            createVulkanBackbuffers				();
        void                            destroyVulkanBackbuffers			();
        VG_INLINE VkSwapchainKHR        getVulkanSwapchain					() const;

		static VkPresentModeKHR			VSyncToVkPresentModeKHR				(VSync mode);
       
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

			DeviceExtensionList			m_deviceExtensionList;
			EXT_HDR_Metadata			m_EXT_HDR_Metadata;
			KHR_Acceleration_Structure	m_KHR_Acceleration_Structure;
			KHR_BufferDeviceAddress		m_KHR_Buffer_Device_Address;
			KHR_DeferredHostOperations	m_KHR_Deferred_Host_Operations;
			KHR_Multiview				m_KHR_Multiview;
			KHR_RayQuery				m_KHR_Ray_Query;
			KHR_RayTracingPipeline		m_KHR_Ray_Tracing_Pipeline;
			KHR_Swapchain				m_KHR_Swapchain;

            InstanceExtensionList		m_instanceExtensionList;
            EXT_DebugUtils				m_EXT_DebugUtils;
            EXT_Swapchain_Colorspace	m_EXT_SwapChain_Colorspace;
            KHR_Surface					m_KHR_Surface;
            KHR_Win32_Surface			m_KHR_Win32_Surface;
			KHR_Get_Surface_Capabilities2 m_KHR_Get_Surface_Capabilities2;

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

#if VG_ENABLE_INLINE
#include "Device_Vulkan.inl"
#endif