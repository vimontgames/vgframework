#pragma once

namespace vg::graphics::driver::vulkan
{
	//--------------------------------------------------------------------------------------
	// Extension
	//--------------------------------------------------------------------------------------
	class Extension
	{
	public:
		virtual bool init() = 0;
		virtual bool create() = 0;

		bool isEnabled() const { return m_enabled; }

	protected:
		bool m_enabled = false;

		static PFN_vkVoidFunction  getVulkanInstanceProcAddress(const char * _func);
		static PFN_vkVoidFunction  getVulkanDeviceProcAddress(const char * _func);

	private:
		static PFN_vkGetDeviceProcAddr s_pfnGetDeviceProcAddr;
	};

	class InstanceExtension : public Extension
	{
	public:
		bool enable(const char * _name);
	};

	class DeviceExtension : public Extension
	{
	public:
		bool enable(const char * _name);
	};

	//--------------------------------------------------------------------------------------
	// ExtensionList
	//--------------------------------------------------------------------------------------
	template <class T> class ExtensionList
	{
	public:
		void					init();
		void					deinit();

		void					registerExtension(T & _extension);
		bool					addExtension(const char * _name);

		core::uint				getEnabledExtensionCount() const;
		const char * const *	getEnabledExtensionNames() const;

	protected:
		core::uint				m_enabledExtensionCount = 0;
		const char *			m_enabledExtensionNames[64];
		VkExtensionProperties *	m_availableExtensions = nullptr;
		core::uint				m_availableExtensionCount = 0;
		core::vector<T*>		m_registered;
	};

	class InstanceExtensionList : public ExtensionList<InstanceExtension>
	{
		using super = ExtensionList;
	public:
		bool init();
		bool onInstanceCreated();
	};

	class DeviceExtensionList : public ExtensionList<DeviceExtension>
	{
		using super = ExtensionList;
	public:
		bool init();
		bool onDeviceCreated();
	};

	#define VG_VULKAN_INSTANCE_PROC_ADRESS(func)	m_pfn##func = (PFN_vk##func)Extension::getVulkanInstanceProcAddress("vk" #func);
	#define VG_VULKAN_DEVICE_PROC_ADRESS(func)	m_pfn##func = (PFN_vk##func)Extension::getVulkanDeviceProcAddress("vk" #func);
}