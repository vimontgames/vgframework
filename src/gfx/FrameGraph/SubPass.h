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

#include VG_GFXAPI_HEADER(SubPass)

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