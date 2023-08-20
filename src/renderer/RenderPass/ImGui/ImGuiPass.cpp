#include "renderer/Precomp.h"
#include "ImGui.h"
#include "ImGuiPass.h"
#include "gfx/Device/Device.h"
#include "renderer/Renderer.h"
#include "renderer/Geometry/Batch/Batch.h"
#include "imguiAdapter.h"
#include "renderer/Options/DisplayOptions.h"
#include "engine/IEngine.h"
#include "core/Plugin/Plugin.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IUniverse.h"
#include "core/IScene.h"
#include "core/IGameObject.h"
#include "corE/IComponent.h"
#include "core/File/File.h"
#include "core/Math/Math.h"
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

    }

    //--------------------------------------------------------------------------------------
    ImGuiPass::~ImGuiPass()
    {

    }
    
    //--------------------------------------------------------------------------------------
    void ImGuiPass::setup(const gfx::RenderPassContext & _renderContext, double _dt)
    {
        writeRenderTarget(0, "Backbuffer");

        editor::IEditor * editor = getEngine()->GetEditor();
        if (editor)
        {
            editor::GUIContext guiContext;
            guiContext.ptr = ImGui::GetCurrentContext();
            editor->DrawGUI(guiContext);
        }        
    }

    //--------------------------------------------------------------------------------------
    const vg::engine::IEngine * ImGuiPass::getEngine()
    {
        const auto * factory = Kernel::getFactory();
        return (const vg::engine::IEngine *) factory->getSingleton("Engine");
    }

    //--------------------------------------------------------------------------------------
    void ImGuiPass::displayEngineWindow()
    {
        //if (ImGui::Begin("Engine", &m_isEngineWindowVisible))
        //{
        //    const auto * factory = Kernel::getFactory();
        //    core::IObject * engine = (core::IObject *)getEngine();
        //    if (engine)
        //    {
        //        const char * className = engine->getClassName();
        //        const auto * classDesc = factory->getClassDescriptor(className);
        //        const char * classDisplayName = classDesc->getClassDisplayName();
        //
        //        ImguiEditor::displayObject(engine);
        //    }
        //}
        //ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiPass::displayRendererWindow()
    {
        //if (ImGui::Begin("Renderer", &m_isRendererWindowVisible))
        //{
        //    const auto * factory = Kernel::getFactory();
        //    IObject * renderer = factory->getSingleton("Renderer");
        //    if (renderer)
        //        ImguiEditor::displayObject(renderer);
        //}
        //ImGui::End();
    }
    
    //--------------------------------------------------------------------------------------
    void ImGuiPass::draw(const RenderPassContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        Renderer::get()->getImGuiAdapter()->render(_cmdList);
    }
}