#pragma once

#include "core/Object/Object.h"

namespace vg::graphics::driver
{
	namespace base
	{
		class SubPass : public core::Object
		{
		public:
			SubPass();
			virtual ~SubPass();

            void addUserPassInfo(const FrameGraph::UserPassInfo & _userPassInfo);
            const core::vector<FrameGraph::UserPassInfo> & getUserPassesInfos() const;

        private:
			core::vector<FrameGraph::UserPassInfo> m_userPassesInfos;
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
        const char * getClassName() const final { return "SubPass"; }

		SubPass();
		~SubPass();
	};
}