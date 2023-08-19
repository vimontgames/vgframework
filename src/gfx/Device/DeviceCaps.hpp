namespace vg::gfx
{
	//--------------------------------------------------------------------------------------
	void DeviceCaps::init()
	{
		#if VG_ENABLE_RAYTRACING
		if (supportRaytracing)
			VG_LOG(core::Level::Info, "[Device] Raytracing is supported");
		else
			VG_LOG(core::Level::Warning, "[Device] Raytracing is not supported");
		#else
		VG_LOG(Level::Warning, "[Device] Raytracing is disabled. Compile with \"#define VG_ENABLE_RAYTRACING  1\" to enable\n");
		#endif
	}
}