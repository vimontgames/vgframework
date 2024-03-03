#pragma once

namespace ImGui
{
    void OpenFileDialog(const vg::core::string _popup, const vg::core::string _ext = ".*", const vg::core::string _relativePath = "", const vg::core::string _file = "");
    void SaveFileDialog(const vg::core::string _popup, const vg::core::string _ext = ".*", const vg::core::string _relativePath = "", const vg::core::string _file = "");
    void SelectFolderDialog(const vg::core::string _popup, const vg::core::string _relativePath = "");
    bool DisplayFileDialog(const vg::core::string _popup);
    bool IsFileDialogOK();
    void CloseFileDialog();
    vg::core::string GetFileDialogSelectedFile();
    vg::core::string GetFileDialogSelectedPath();

    bool FileDialogButton(const char * _label, const ImVec2 & _size = ImVec2(0, 0));
    bool FileDialogPathButton(const char * _label, const ImVec2 & _size = ImVec2(0, 0));
}