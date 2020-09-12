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

            core::uint getUserPassCount() const;
            const UserPass * getUserPass(core::uint _index) const;

			core::vector<const UserPass*> m_userPasses;
		};
	}
}

#include VG_GFXAPI_HEADER(SubPass)

namespace vg::graphics::driver
{
	class SubPass : public VG_GFXAPI::SubPass
	{
		using super = VG_GFXAPI::SubPass;

	public:
		SubPass();
		~SubPass();
	};
}