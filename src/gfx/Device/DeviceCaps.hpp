namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
	VG_REGISTER_OBJECT_CLASS(DeviceCaps, "DeviceCaps");
    //--------------------------------------------------------------------------------------
 
    //--------------------------------------------------------------------------------------
    bool DeviceCaps::registerProperties(IClassDesc & _desc)
    {
        //super::registerProperties(_desc);

        registerPropertyEx(DeviceCaps, gpuName, "GPU", PropertyFlags::ReadOnly);
        setPropertyDescription(DeviceCaps, gpuName, "GPU model name");

		registerPropertyEnumEx(DeviceCaps, ShaderModel, shaderModel, "Shaders", PropertyFlags::ReadOnly);
		setPropertyDescription(DeviceCaps, shaderModel, "Highest shader model available");

        registerPropertyEx(DeviceCaps, rayTracing.supported, "RayTracing", PropertyFlags::ReadOnly);
        setPropertyDescription(DeviceCaps, rayTracing.supported, "Is ray tracing supported on hardware");

		registerPropertyEnumBitfieldEx(DeviceCaps, MSAA, msaa.modes, "MSAA", PropertyFlags::ReadOnly);
		setPropertyDescription(DeviceCaps, msaa.modes, "Supported multisampling modes");

        registerPropertyEnumBitfieldEx(DeviceCaps, HDR, hdr.modes, "HDR", PropertyFlags::ReadOnly);
        setPropertyDescription(DeviceCaps, hdr.modes, "Supported HDR modes");

        return true;
    }

    //--------------------------------------------------------------------------------------
	DeviceCaps::DeviceCaps(const string & _name, IObject * _parent)
	{
        
	}

	//--------------------------------------------------------------------------------------
	void DeviceCaps::init()
	{
		#if VG_ENABLE_RAYTRACING
		if (rayTracing.supported)
			VG_INFO("[Device] Raytracing is supported");
		else
			VG_WARNING("[Device] Raytracing is not supported");
		#else
		VG_WARNING("[Device] Raytracing is disabled. Compile with \"#define VG_ENABLE_RAYTRACING  1\" to enable\n");
		#endif

		if (hdr.isSupported(HDR::HDR10) && hdr.isSupported(HDR::HDR16))
		{
			VG_INFO("[Device] HDR10 & HDR16 are supported");
		}
		else if (hdr.isSupported(HDR::HDR10))
		{
			VG_INFO("[Device] HDR10 is supported");
			VG_WARNING("[Device] HDR16 is not supported");
		}
        else if (hdr.isSupported(HDR::HDR16))
        {
			VG_WARNING("[Device] HDR10 is not supported");
            VG_INFO("[Device] HDR16 is supported");
        }
		else
		{
			VG_WARNING("[Device] HDR10 & HDR16 are not supported");
		}
	}
}