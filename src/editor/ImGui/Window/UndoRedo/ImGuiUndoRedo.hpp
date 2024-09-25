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
    void ImGuiUndoRedo::DrawGUI()
    {
        if (ImGui::IconBegin(getIcon().c_str(), "Undo/Redo", &m_isVisible))
        {
            const auto * undoRedoManager = Kernel::getUndoRedoManager();
            auto * imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
            static const float columnWidth[] = { 128, 196 };

            ImGui::BeginChild(ImGui::getObjectLabel("ChildWindow", this).c_str());
            {
                const auto & undoStack = undoRedoManager->GetUndoStack();
                const auto & redoStack = undoRedoManager->GetRedoStack();

                ImGui::Columns(2, "Undo/Redo", true);                
                SetColumnWidth(0, columnWidth[0]);
                SetColumnWidth(1, columnWidth[1]);

                imGuiAdapter->PushDefaultFont();
                imGuiAdapter->PushStyle(renderer::FontStyle::Bold);

                for (auto & elem : undoStack)
                    Text("%s", elem->GetName().c_str());

                NextColumn();

                for (auto & elem : undoStack)
                    Text("%s", elem->GetDescription().c_str());

                imGuiAdapter->PopStyle();

                ImGui::Columns(1);    
                //ImGui::Separator();

                ImGui::Columns(2, "Undo/Redo", true);
                SetColumnWidth(0, columnWidth[0]);
                SetColumnWidth(1, columnWidth[1]);

                // Redo stack is displayed in reverse order
                for (auto i = (int)redoStack.size() - 1; i >= 0; --i)
                {
                    auto & elem = redoStack[i];
                    Text("%s", elem->GetName().c_str());
                }
                NextColumn();

                // Redo stack is displayed in reverse order
                for (auto i = (int)redoStack.size() - 1; i >= 0; --i)
                {
                    auto & elem = redoStack[i];
                    Text("%s", elem->GetDescription().c_str());
                }

                ImGui::Columns(1);
                imGuiAdapter->PopFont();
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
}