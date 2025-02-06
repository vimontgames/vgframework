#pragma once

#include "core/Object/Object.h"

namespace vg::gfx
{
	namespace base
	{
		class SubPass : public core::Object
		{
		public:
			SubPass();
			virtual ~SubPass();

            void addUserPassInfo(const UserPassInfo & _userPassInfo);
            const core::vector<UserPassInfo> & getUserPassesInfos() const;

        private:
			core::vector<UserPassInfo> m_userPassesInfos;
		};
	}
}

#ifdef VG_DX12
#include "dx12/SubPass_dx12.h"
#elif defined(VG_VULKAN)
#include "vulkan/SubPass_vulkan.h"
#else
#error Undefined GFXAPI
#endif

namespace vg::gfx
{
	class SubPass : public VG_GFXAPI::SubPass
	{
		using super = VG_GFXAPI::SubPass;

	public:
        const char * GetClassName() const final { return "SubPass"; }

		SubPass();
		~SubPass();
	};
}