namespace vg::gfx::vulkan
{
	using namespace core;

	//--------------------------------------------------------------------------------------
	// Extension list
	//--------------------------------------------------------------------------------------
	template <class T> void ExtensionList<T>::init()
	{
		memset(m_enabledExtensionNames, 0, sizeof(m_enabledExtensionNames));
	}

	//--------------------------------------------------------------------------------------
	template <class T> void ExtensionList<T>::deinit()
	{
		m_enabledExtensionCount = 0;
		VG_SAFE_FREE(m_availableExtensions);
	}

	//--------------------------------------------------------------------------------------
	template <class T> void ExtensionList<T>::registerExtension(T & _extension)
	{
		m_registered.push_back(&_extension);
	}

	//--------------------------------------------------------------------------------------
	template <class T> bool ExtensionList<T>::addExtension(const char * _name)
	{
		for (uint i = 0; i < m_availableExtensionCount; ++i)
		{
			if (!strcmp(_name, m_availableExtensions[i].extensionName))
			{
				m_enabledExtensionNames[m_enabledExtensionCount++] = _name;
				VG_ASSERT(m_enabledExtensionCount < countof(m_enabledExtensionNames));
				VG_INFO("[Device] Extension \"%s\" is enabled", _name);
				return true;
			}
		}
		VG_WARNING("[Device] Extension \"%s\" cound not be enabled", _name);
		return false;
	}

	//--------------------------------------------------------------------------------------
	template <class T> core::uint ExtensionList<T>::getEnabledExtensionCount() const
	{
		return m_enabledExtensionCount;
	}

	//--------------------------------------------------------------------------------------
	template <class T> const char * const * ExtensionList<T>::getEnabledExtensionNames() const
	{
		return m_enabledExtensionNames;
	}

	//--------------------------------------------------------------------------------------
	// Instance extension list
	//--------------------------------------------------------------------------------------
	bool InstanceExtensionList::init()
	{
		super::init();

		VG_ASSERT_VULKAN(vkEnumerateInstanceExtensionProperties(nullptr, &m_availableExtensionCount, nullptr));

		if (m_availableExtensionCount > 0)
		{
			m_availableExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * m_availableExtensionCount);
			VG_ASSERT_VULKAN(vkEnumerateInstanceExtensionProperties(nullptr, &m_availableExtensionCount, m_availableExtensions));
			
			for (auto * ext : m_registered)
				ext->init();
			
			return true;
		}

		return false;
	}

	//--------------------------------------------------------------------------------------
	bool InstanceExtensionList::onInstanceCreated()
	{
		bool result = true;

		for (auto * ext : m_registered)
			result |= ext->create();

		return result;
	}

	//--------------------------------------------------------------------------------------
	// Device extension list
	//--------------------------------------------------------------------------------------
	bool DeviceExtensionList::init()
	{
		super::init();

		auto * device = gfx::Device::get();

		VG_ASSERT_VULKAN(vkEnumerateDeviceExtensionProperties(device->getVulkanPhysicalDevice(), nullptr, &m_availableExtensionCount, nullptr));

		if (m_availableExtensionCount > 0)
		{
			m_availableExtensions = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * m_availableExtensionCount);
			VG_ASSERT_VULKAN(vkEnumerateDeviceExtensionProperties(device->getVulkanPhysicalDevice(), nullptr, &m_availableExtensionCount, m_availableExtensions));
			
			for (auto * ext : m_registered)
				ext->init();

			return true;
		}

		return false;
	}

	//--------------------------------------------------------------------------------------
	bool DeviceExtensionList::onDeviceCreated()
	{
		bool result = true;

		for (auto * ext : m_registered)
			result |= ext->create();

		return result;
	}

	//--------------------------------------------------------------------------------------
	// Extension
	//--------------------------------------------------------------------------------------
	PFN_vkGetDeviceProcAddr Extension::s_pfnGetDeviceProcAddr = nullptr;
	
	//--------------------------------------------------------------------------------------
	PFN_vkVoidFunction Extension::getVulkanInstanceProcAddress(const char * _func)
	{
		auto & instance = gfx::Device::get()->getVulkanInstance();
		auto addr = vkGetInstanceProcAddr(instance, _func);
		VG_ASSERT(addr, "Could not getVulkanInstanceProcAddress(\"%s\")", _func);
		return addr;
	}

	//--------------------------------------------------------------------------------------
	PFN_vkVoidFunction Extension::getVulkanDeviceProcAddress(const char * _func)
	{
		if (nullptr == s_pfnGetDeviceProcAddr)
			s_pfnGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)getVulkanInstanceProcAddress("vkGetDeviceProcAddr");
		VG_ASSERT(nullptr != s_pfnGetDeviceProcAddr);
		auto & device = gfx::Device::get()->getVulkanDevice();
		auto addr = s_pfnGetDeviceProcAddr(device, _func);
		VG_ASSERT(addr, "Could not getVulkanDeviceProcAddress(\"%s\")", _func);
		return addr;
	}

	//--------------------------------------------------------------------------------------
	// InstanceExtension
	//--------------------------------------------------------------------------------------
	bool InstanceExtension::enable(const char * _name)
	{
		auto & list = gfx::Device::get()->m_instanceExtensionList;
		
		if (m_enabled || list.addExtension(_name))
		{
			m_enabled = true;
			return true;
		}

		return false;
	}

	//--------------------------------------------------------------------------------------
	// DeviceExtension
	//--------------------------------------------------------------------------------------
	bool DeviceExtension::enable(const char * _name)
	{
		auto & list = gfx::Device::get()->m_deviceExtensionList;
		
		if (m_enabled || list.addExtension(_name))
		{
			m_enabled = true;
			return true;
		}

		return false;
	}
}