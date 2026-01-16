#include "renderer/Precomp.h"
#include "ImGui.h"
#include "ImGuiPass.h"
#include "imguiAdapter.h"
#include "core/Plugin/Plugin.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IWorld.h"
#include "core/IBaseScene.h"
#include "core/IGameObject.h"
#include "core/IComponent.h"
#include "core/File/File.h"
#include "engine/IEngine.h"
#include "gfx/Device/Device.h"
#include "renderer/IUIRenderer.h"
#include "renderer/Renderer.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "renderer/Options/RendererOptions.h"
#include "editor/IEditor.h"

#include "ImGui.hpp"
#include "ImGuiSettings.hpp"
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
    core::u64 ImGuiPass::GetCostEstimate(const RenderPassContext & _renderPassContext) const
    {
        return 1000;
    }
    
    //--------------------------------------------------------------------------------------
    void ImGuiPass::Setup(const gfx::RenderPassContext & _renderPassContext)
    {
        // ImGui windows require to read the viewport render targets
        Renderer * renderer = Renderer::get();

        for (uint j = 0; j < enumCount<gfx::ViewTarget>(); ++j)
        {
            gfx::ViewTarget target = (gfx::ViewTarget)j;

            if (renderer->IsGameMode())
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
        imGuiAdapter->beginFrame(_cmdList);
        imGuiAdapter->PushDefaultFont();

        auto * renderer = Renderer::get();

        if (renderer->IsEditor())
        {
            editor::IEditor * editor = getEngine()->GetEditor();
            VG_ASSERT(nullptr != editor);
            if (editor)
            {
                editor::GUIContext guiContext;
                guiContext.imgui = ImGui::GetCurrentContext();
                guiContext.filedialog = ImGuiFileDialog::Instance();
                editor->DrawGUI(guiContext);
            }
        }
        else
        {
            // When not editor, render game UI directly to backbuffer
            auto & gameViewports = renderer->GetViewports(gfx::ViewportTarget::Game);
            for (auto & gameViewport : gameViewports)
            {
                for (auto & pair : gameViewport->GetViewIDs())
                {
                    if (auto * view = renderer->GetView(pair.second))
                    {
                        if (auto * viewGUI = view->GetUIRenderer())
                            viewGUI->RenderFullscreen(RenderUIType::RenderUIType_3D);
                    }
                }


                if (auto * uiRenderer = gameViewport->GetUIRenderer())
                    uiRenderer->RenderFullscreen(RenderUIType::RenderUIType_2D);
            }
        }

        imGuiAdapter->PopFont();
        imGuiAdapter->render(_cmdList);
    }
}