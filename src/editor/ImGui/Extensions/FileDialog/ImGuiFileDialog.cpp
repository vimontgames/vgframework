#include "editor/Precomp.h"
#include "ImGuiFileDialog.h"
#include "core/File/File.h"
#include "ImGuiFileDialog/ImGuiFileDialog.h"

using namespace vg::core;
using namespace vg::editor;

namespace ImGui
{
    //--------------------------------------------------------------------------------------
    void OpenDialog(const vg::core::string _popup, const vg::core::string _ext, const vg::core::string _relativePath, const vg::core::string _file, bool _save, bool _folder)
    {
        auto * fileDialog = ImGuiFileDialog::Instance();

        string path = _relativePath;
        const auto root = io::getRootDirectory();
        if (!_relativePath._Starts_with(root))
            path = root + "/" + _relativePath;

        IGFD::FileDialogConfig config;
        config.path = path;
        config.flags = ImGuiFileDialogFlags_Modal;
        config.countSelectionMax = 1;

        if (_save)
            config.flags |= ImGuiFileDialogFlags_ConfirmOverwrite;

        fileDialog->OpenDialog(_popup, _popup, _folder ? nullptr : _ext.c_str(), config);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    }

    //--------------------------------------------------------------------------------------
    void SelectFolderDialog(const vg::core::string _popup, const vg::core::string _relativePath)
    {
        OpenDialog(_popup, "", _relativePath, "", false, true);
    }

    //--------------------------------------------------------------------------------------
    void OpenFileDialog(const vg::core::string _popup, const vg::core::string _ext, const vg::core::string _relativePath, const vg::core::string _file)
    {
        OpenDialog(_popup, _ext, _relativePath, _file, false, false);
    }

    //--------------------------------------------------------------------------------------
    void SaveFileDialog(const vg::core::string _popup, const vg::core::string _ext, const vg::core::string _relativePath, const vg::core::string _file)
    {
        OpenDialog(_popup, _ext, _relativePath, _file, true, false);
    }

    //--------------------------------------------------------------------------------------
    bool DisplayFileDialog(const vg::core::string _popup)
    {
        auto * fileDialog = ImGuiFileDialog::Instance();
        return fileDialog->Display(_popup, ImGuiWindowFlags_NoCollapse, style::dialog::Size);
    }

    //--------------------------------------------------------------------------------------
    bool IsFileDialogOK()
    {
        auto * fileDialog = ImGuiFileDialog::Instance();
        return fileDialog->IsOk();
    }

    //--------------------------------------------------------------------------------------
    void CloseFileDialog()
    {
        auto * fileDialog = ImGuiFileDialog::Instance();
        fileDialog->Close();
    }

    //--------------------------------------------------------------------------------------
    vg::core::string GetFileDialogSelectedFile()
    {
        auto * fileDialog = ImGuiFileDialog::Instance();
        return fileDialog->GetFilePathName();
    }

    //--------------------------------------------------------------------------------------
    vg::core::string GetFileDialogSelectedPath()
    {
        auto * fileDialog = ImGuiFileDialog::Instance();
        return fileDialog->GetCurrentPath();
    }

    //--------------------------------------------------------------------------------------
    bool FileDialogButton(const char * _label, const ImVec2 & _size)
    {
        ImVec2 size = _size;
        if (size.x == 0)
            size.x = style::button::SizeSmall.x;
        return ImGui::Button(_label, size);
    }

    //--------------------------------------------------------------------------------------
    bool FileDialogPathButton(const char * _label, const ImVec2 & _size)
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_WindowBg));
        
        ImVec2 size = _size;
        bool result = ImGui::Button(_label, size);

        ImGui::PopStyleColor(1);
        ImGui::PopStyleVar(2); 

        return result;
    }
}