#include "renderer/Precomp.h"
#include "ImGui.h"
#include "ImGuiPass.h"
#include "gfx/Device/Device.h"
#include "renderer/Renderer.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "imguiAdapter.h"
#include "renderer/Options/RendererOptions.h"
#include "engine/IEngine.h"
#include "core/Plugin/Plugin.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IUniverse.h"
#include "core/IScene.h"
#include "core/IGameObject.h"
#include "corE/IComponent.h"
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
    void ImGuiPass::Setup(const gfx::RenderPassContext & _renderContext, double _dt)
    {
        // ImGui windows require to read the viewport render targets
        Renderer * renderer = Renderer::get();

        for (uint j = 0; j < enumCount<gfx::ViewTarget>(); ++j)
        {
            const auto & views = renderer->GetViews((gfx::ViewTarget)j);
            for (uint i = 0; i < views.size(); ++i)
            {
                const auto * view = views[i];
                if (view)
                {
                    if (!view->IsVisible())
                        continue;

                    ITexture * dest = view->GetRenderTarget();
                    readRenderTarget(dest->getName());
                }
            }
        }
        
        writeRenderTarget(0, "Backbuffer");       
    }

    //--------------------------------------------------------------------------------------
    const vg::engine::IEngine * ImGuiPass::getEngine()
    {
        const auto * factory = Kernel::getFactory();
        return (const vg::engine::IEngine *) factory->getSingleton("Engine");
    }
    
    //--------------------------------------------------------------------------------------
    void ImGuiPass::Render(const RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        editor::IEditor * editor = getEngine()->GetEditor();
        if (editor)
        {
            editor::GUIContext guiContext;
            guiContext.ptr = ImGui::GetCurrentContext();
            editor->DrawGUI(guiContext);
        }

        Renderer::get()->getImGuiAdapter()->render(_cmdList);
    }
}