namespace vg::gfx
{
	//--------------------------------------------------------------------------------------
	void DeviceCaps::init()
	{
		#if VG_ENABLE_RAYTRACING
		if (supportRayTracing)
			VG_INFO("[Device] Raytracing is supported");
		else
			VG_WARNING("[Device] Raytracing is not supported");
		#else
		VG_WARNING("[Device] Raytracing is disabled. Compile with \"#define VG_ENABLE_RAYTRACING  1\" to enable\n");
		#endif
	}
}