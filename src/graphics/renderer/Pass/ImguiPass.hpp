#include "ImguiPass.h"
#include "graphics/renderer/imgui/imguiAdapter.h"

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiPass::setup()
    {
        auto * renderer = Renderer::get();
        const auto & backbuffer = renderer->getBackbuffer()->getTexDesc();

        FrameGraph::TextureDesc desc;
        desc.width = backbuffer.width;
        desc.height = backbuffer.height;
        desc.format = backbuffer.format;

        //readRenderTarget()
        desc.initState = FrameGraph::Resource::InitState::Preserve;
        writeRenderTarget(0, "Backbuffer", desc);

        char buffer0[256] = "It's \"Dear ImGui\"";
        char buffer1[256] = "OK I get it ^^";

        float f = 3.1417f;
        ImGui::Text("Hello, world %d", 123);
        if (ImGui::Button("Save"))
        {
            // do stuff  
            VG_DEBUGPRINT("Save me!\n");
        }
        ImGui::InputText("text0", buffer0, IM_ARRAYSIZE(buffer0));
        ImGui::InputText("text1", buffer1, IM_ARRAYSIZE(buffer1));
        ImGui::SliderFloat("dummy", &f, 0.0f, 10.0f);
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::draw(driver::CommandList * _cmdList) const
    {
        Renderer::get()->m_imgui->render(_cmdList);
    }
}