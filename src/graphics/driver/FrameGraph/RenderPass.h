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
			RenderPass(const RenderPassKey & _key);
			virtual ~RenderPass();

            void addSubPass(driver::SubPass * _subPass);
            const core::vector<driver::SubPass*> & getSubPasses() const;

            const RenderPassKey & getRenderPassKey() const { return m_renderPassKey; }
			
		//private:
            RenderPassKey                           m_renderPassKey;
			core::vector<driver::SubPass*>		    m_subPasses;
			core::vector<const driver::Texture*>    m_attachments;
		};
	}
}

#include VG_GFXAPI_HEADER(RenderPass)

namespace vg::graphics::driver
{
	class RenderPass : public VG_GFXAPI::RenderPass
	{
		using super = VG_GFXAPI::RenderPass;

	public:
        //void * operator new(size_t _size);
        //void operator delete(void * _address);

		RenderPass(const RenderPassKey & _key);
		~RenderPass();

		void finalize();

    private:
        static core::Pool<RenderPass> s_memPool;
	};
}