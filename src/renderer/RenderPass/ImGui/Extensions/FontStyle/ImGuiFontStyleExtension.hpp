#include "ImGuiFontStyleExtension.h"
#include "renderer/ImGui_consts.h"
#include "renderer/IRenderer.h"

using namespace vg::renderer;

namespace ImGui
{
    //static Font g_font = Font::UbuntuMono;
    //static Style g_style = Style::Regular;
    //
    ////--------------------------------------------------------------------------------------
    //void PushFont(Font _font)
    //{
    //    g_font = _font;
    //
    //    //auto renderer = Editor::get()->getRenderer();
    //    auto renderer = Renderer::get();
    //    auto * imGuiAdapter = renderer->GetImGuiAdapter();
    //    PushFont(imGuiAdapter->GetFont(g_font, g_style));
    //}
    //
    ////--------------------------------------------------------------------------------------
    //void PushStyle(Style _style)
    //{
    //    g_style = _style;
    //
    //    //auto renderer = Editor::get()->getRenderer();
    //    auto renderer = Renderer::get();
    //    auto * imGuiAdapter = renderer->GetImGuiAdapter();
    //    PushFont(imGuiAdapter->GetFont(g_font, g_style));
    //}
    //
    ////--------------------------------------------------------------------------------------
    //void PopStyle()
    //{
    //    PopFont();
    //}
}