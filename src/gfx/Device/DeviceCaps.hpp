namespace vg::gfx
{
	//--------------------------------------------------------------------------------------
	void DeviceCaps::init()
	{
		#if VG_ENABLE_RAYTRACING
		if (supportRaytracing)
			VG_DEBUGPRINT("[Device] Raytracing is supported\n");
		else
			VG_DEBUGPRINT("[Device] Raytracing is not supported\n");
		#else
			VG_DEBUGPRINT("[Device] Raytracing is disabled. Compile with \"#define VG_ENABLE_RAYTRACING  1\" to enable\n");
		#endif
	}
}