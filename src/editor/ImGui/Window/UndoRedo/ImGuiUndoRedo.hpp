#include "ImGuiUndoRedo.h"
#include "editor/Editor_Consts.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiUndoRedo::ImGuiUndoRedo() :
        ImGuiWindow(style::icon::Undo, "", "ImGuiUndoRedo", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void displayElem(IUndoRedoEntry * elem)
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        auto * subs = elem->GetSubEntries();

        if (subs)
        {
            const string nodeName = fmt::sprintf("%s###0x%016X", elem->GetEntryName(), (u64)elem);
            bool open = ImGui::TreeNodeEx(nodeName.c_str(), ImGuiTreeNodeFlags_SpanAllColumns);           

            ImGui::TableNextColumn();
            Text("%s", elem->GetObjectName().c_str());

            ImGui::TableNextColumn();
            Text("%s", elem->GetDescription().c_str());

            if (open)
            {
                for (auto * sub : *subs)
                {
                    displayElem(sub);
                }
                ImGui::TreePop();
            }
        }
        else
        {
            Text("%s", elem->GetEntryName().c_str());

            ImGui::TableNextColumn();
            Text("%s", elem->GetObjectName().c_str());

            ImGui::TableNextColumn();
            Text("%s", elem->GetDescription().c_str());
        }
    };

    //--------------------------------------------------------------------------------------
    void ImGuiUndoRedo::DrawGUI()
    {
        if (ImGui::IconBegin(getIcon().c_str(), "Undo/Redo", &m_isVisible))
        {
            const auto * undoRedoManager = Kernel::getUndoRedoManager();
            auto * imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                const auto & undoStack = undoRedoManager->GetUndoStack();
                const auto & redoStack = undoRedoManager->GetRedoStack();

                imGuiAdapter->PushDefaultFont();

                const ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

                if (ImGui::BeginTable("3ways", 3, flags))
                {
                    // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
                    ImGui::TableSetupColumn("Entry", ImGuiTableColumnFlags_WidthFixed, 96.0f);
                    ImGui::TableSetupColumn("Object", ImGuiTableColumnFlags_WidthFixed, 96.0f);
                    ImGui::TableSetupColumn("Details");
                    ImGui::TableHeadersRow();

                    for (auto & elem : undoStack)
                    {
                        //ImGui::TableNextRow();
                        displayElem(elem);
                    }

                    // Redo stack is displayed in reverse order
                    ImGui::BeginDisabled(true);
                    for (auto i = (int)redoStack.size() - 1; i >= 0; --i)
                    {
                        //ImGui::TableNextRow();
                        auto & elem = redoStack[i];
                        displayElem(elem);
                    }
                    ImGui::EndDisabled();

                    ImGui::EndTable();
                }
                imGuiAdapter->PopFont();
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}