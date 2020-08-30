#pragma once

#include "core/Object/Object.h"
#include "core/Pool/Pool.h"
#include "graphics/driver/FrameGraph/FrameGraph.h"

namespace vg::graphics::driver
{
	class SubPass;

	namespace base
	{
		class RenderPass : public core::Object
		{
		public:
			RenderPass();
			virtual ~RenderPass();

            void addSubPass(driver::SubPass * _subPass);
            const core::vector<driver::SubPass*> & getSubPasses() const;
			
		//private:
			core::vector<driver::SubPass*>		    m_subPasses;
			core::vector<const driver::Texture*>    m_attachments;
		};
	}
}

#include VG_GRAPHICSAPI_HEADER(RenderPass)

namespace vg::graphics::driver
{
	class RenderPass : public VG_GRAPHICSAPI::RenderPass
	{
		using super = VG_GRAPHICSAPI::RenderPass;

	public:
        void * operator new(size_t _size);
        void operator delete(void * _address);

		RenderPass();
		~RenderPass();

		void finalize();

    private:
        static core::Pool<RenderPass> s_memPool;
	};
}