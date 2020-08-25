#pragma once

#include "core/Object/Object.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"

namespace vg::graphics::driver
{
	namespace base
	{
		class SubPass : public core::Object
		{
		public:
			SubPass();
			virtual ~SubPass();

			core::vector<const UserPass*> m_userPasses;
		};
	}
}

#include VG_GRAPHICSAPI_HEADER(SubPass)

namespace vg::graphics::driver
{
	class SubPass : public VG_GRAPHICSAPI::SubPass
	{
		using super = VG_GRAPHICSAPI::SubPass;

	public:
		SubPass();
		~SubPass();
	};
}