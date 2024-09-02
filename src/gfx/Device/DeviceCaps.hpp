namespace vg::gfx
{
    //--------------------------------------------------------------------------------------
	DeviceCaps::DeviceCaps()
	{
        
	}

	//--------------------------------------------------------------------------------------
	void DeviceCaps::init()
	{
		#if VG_ENABLE_RAYTRACING
		if (rayTracing)
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