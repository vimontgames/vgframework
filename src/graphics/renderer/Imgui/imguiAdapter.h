#pragma once

#include "imgui/imgui.h"
#include "graphics/driver/BindlessTable/BindlessTable_consts.h"

namespace vg::graphics
{
    namespace driver
    {
        class Device;
        class CommandList;
    }

    namespace renderer
    {
        class ImguiAdapter
        {
        public:
            ImguiAdapter(core::WinHandle _winHandle, driver::Device & _device);
            ~ImguiAdapter();

            void beginFrame();
            void render(driver::CommandList * _cmdList);

        private:
            driver::BindlessTextureSrvHandle m_fontTexSRVHandle;
        };
    }
}