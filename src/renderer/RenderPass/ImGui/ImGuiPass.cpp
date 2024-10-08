#include "renderer/Precomp.h"
#include "ImGui.h"
#include "ImGuiPass.h"
#include "gfx/Device/Device.h"
#include "gfx/IUIRenderer.h"
#include "renderer/Renderer.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "imguiAdapter.h"
#include "renderer/Options/RendererOptions.h"
#include "engine/IEngine.h"
#include "core/Plugin/Plugin.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IWorld.h"
#include "core/IBaseScene.h"
#include "core/IGameObject.h"
#include "core/IComponent.h"
#include "core/File/File.h"
#include "editor/IEditor.h"

#include "ImGui.hpp"
#include "ImGuiAdapter.hpp"

using namespace vg::core;
using namespace vg::gfx;
using namespace vg::renderer;
using namespace ImGui;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // In case several windows are docked together, then the last declared will be the default selected
    //--------------------------------------------------------------------------------------
    ImGuiPass::ImGuiPass() :
        gfx::UserPass("ImGuiPass")
    {
        setUserPassType(RenderPassType::Graphic);
    }

    //--------------------------------------------------------------------------------------
    ImGuiPass::~ImGuiPass()
    {

    }
    
    //--------------------------------------------------------------------------------------
    void ImGuiPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        // ImGui windows require to read the viewport render targets
        Renderer * renderer = Renderer::get();

        for (uint j = 0; j < enumCount<gfx::ViewTarget>(); ++j)
        {
            gfx::ViewTarget target = (gfx::ViewTarget)j;

            if (renderer->IsFullscreen())
                continue;

            const auto & views = renderer->GetViews(target);
            for (uint i = 0; i < views.size(); ++i)
            {
                const auto * view = views[i];
                if (view)
                {
                    if (!view->IsRender())
                        continue;

                    ITexture * dest = view->GetRenderTarget();
                    if(dest)
                        readRenderTarget(dest->GetName());
                }
            }
        }

        if (HDR::None != renderer->GetHDR())
            writeRenderTarget(0, "HDROutput");
        else
            writeRenderTarget(0, "Backbuffer");
    }

    //--------------------------------------------------------------------------------------
    const vg::engine::IEngine * ImGuiPass::getEngine()
    {
        const auto * factory = Kernel::getFactory();
        return (const vg::engine::IEngine *) factory->GetSingleton("Engine");
    }
    
    //--------------------------------------------------------------------------------------
    void ImGuiPass::Render(const RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        auto * imGuiAdapter = Renderer::get()->getImGuiAdapter();
        imGuiAdapter->PushDefaultFont();

        #if VG_ENABLE_EDITOR
        editor::IEditor * editor = getEngine()->GetEditor();
        if (editor)
        {
            editor::GUIContext guiContext;
            guiContext.imgui = ImGui::GetCurrentContext();
            guiContext.filedialog = ImGuiFileDialog::Instance();

            editor->DrawGUI(guiContext);
        }
        #else
        // When no editor, render game UI directly to final target
        if (Renderer::get()->IsFullscreen())
        {
            auto * renderer = Renderer::get();
            auto & gameViewports = renderer->GetViewports(gfx::ViewportTarget::Game);
            for (auto & gameViewport : gameViewports)
            {
                if (auto * uiRenderer = gameViewport->GetUIRenderer())
                    uiRenderer->RenderFullscreen();
            }
        }
        #endif

        imGuiAdapter->PopFont();
        imGuiAdapter->render(_cmdList);
    }
}