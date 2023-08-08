#include "renderer/Precomp.h"
#include "ImguiPass.h"
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
#include "imgui/imgui.h"
#include "editor/IEditor.h"

using namespace vg::core;
using namespace vg::gfx;
using namespace vg::renderer;
using namespace ImGui;

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // In case several windows are docked together, then the last declared will be the default selected
    //--------------------------------------------------------------------------------------
    ImguiPass::ImguiPass() :
        gfx::UserPass("ImGuiPass")
    {

    }

    //--------------------------------------------------------------------------------------
    ImguiPass::~ImguiPass()
    {

    }
    
    //--------------------------------------------------------------------------------------
    void ImguiPass::setup(const gfx::RenderContext & _renderContext, double _dt)
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
    const vg::engine::IEngine * ImguiPass::getEngine()
    {
        const auto * factory = Kernel::getFactory();
        return (const vg::engine::IEngine *) factory->getSingleton("Engine");
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayEngineWindow()
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
    void ImguiPass::displayRendererWindow()
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
    void ImguiPass::draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {
        Renderer::get()->getImGuiAdapter()->render(_cmdList);
    }
}