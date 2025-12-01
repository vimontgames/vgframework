#pragma once

#include "core/IResourceManager.h"
#include "core/IResourceMeta.h"
#include "gfx/ITexture.h"
#include "renderer/IRenderer.h"
#include "renderer/IImGuiAdapter.h"
#include "editor/Editor.h"
#include "engine/IEngine.h"

namespace vg::editor
{
    using namespace vg::core;

    class ImGuiTextureResourceHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        // Texture resource preview
        //--------------------------------------------------------------------------------------
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext) final override
        {
            IResourceManager * rm = Editor::get()->getEngine()->GetResourceManager();
            const auto * factory = Kernel::getFactory();

            bool changed = false;

            IResource * _resource = VG_SAFE_STATIC_CAST(IResource, _object);
            IObject * resourceObject = _resource->GetObject();
            IResourceMeta * resourceMeta = nullptr;

            if (resourceObject)
            {
                const auto * classDesc = factory->GetClassDescriptor(_object->GetClassName(), false);
                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);
                    changed |= ImGuiWindow::displayProperty(_object, prop, _objectContext);
                }
                resourceMeta = rm->GetOrCreateResourceMeta(_resource);
            }
            else if (nullptr == resourceObject)
            {
                const auto texturePreviewSize = ImGuiWindow::getImGuiPreviewSize();

                if (!strcmp(_resource->GetClassName(), "TextureResource"))
                {
                    // Dummy texture preview (should display actual replacement texture or display nothing : a black square is useless)
                    //ImDrawList * draw_list = ImGui::GetWindowDrawList();
                    //ImVec2 cursor_pos = ImGui::GetCursorScreenPos();
                    //draw_list->AddRectFilled(cursor_pos, cursor_pos + texturePreviewSize, IM_COL32(0, 0, 0, 255));
                    //ImGui::Dummy(texturePreviewSize);
                }
            }

            const string metaLabel = fmt::sprintf("Metadata###%u", _resource->GetUID());

            if (resourceMeta)
            {
                if (ImGui::TreeNodeEx(metaLabel.c_str(), ImGuiTreeNodeFlags_None))
                {
                    changed |= ImGuiWindow::displayObject(resourceMeta);
                    ImGui::TreePop();
                }
            }
            else
            {
                // Dummy meta
                //if (!strcmp(_resource->GetClassName(), "TextureResource"))
                //{
                //    ImGui::BeginDisabled();
                //    const float backupAlpha = ImGui::GetStyle().Alpha;
                //    ImGui::GetStyle().Alpha = 0;
                //    const auto * factory = Kernel::getFactory();
                //    resourceMeta = (IResourceMeta *)factory->CreateObject("TextureResourceMeta");
                //
                //    if (ImGui::TreeNodeEx(metaLabel.c_str(), ImGuiTreeNodeFlags_None))
                //    {
                //        changed |= ImGuiWindow::displayObject(resourceMeta);
                //        ImGui::TreePop();
                //    }
                //
                //    ImGui::GetStyle().Alpha = backupAlpha;
                //    ImGui::EndDisabled();
                //    VG_SAFE_DELETE(resourceMeta);
                //}
            }

            return changed;
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(TextureResource, ImGuiTextureResourceHandler);
}