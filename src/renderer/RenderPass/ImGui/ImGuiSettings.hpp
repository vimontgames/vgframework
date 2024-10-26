#include "ImGuiSettings.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    void ImGuiSettings::init()
    {
        ImGuiSettingsHandler ini_handler;
        ini_handler.TypeName = "VimontGames";
        ini_handler.TypeHash = ImHashStr("VimontGames");
        ini_handler.ReadOpenFn = onReadOpen;
        ini_handler.ReadLineFn = onReadLine;
        ini_handler.WriteAllFn = onWriteAll;
        ImGui::GetCurrentContext()->SettingsHandlers.push_back(ini_handler);

        ImGui::LoadIniSettingsFromDisk(ImGui::GetIO().IniFilename);
    }

    //--------------------------------------------------------------------------------------
    void * ImGuiSettings::onReadOpen(ImGuiContext * _imGuiContext, ImGuiSettingsHandler * _imGuiSettingsHandler, const char * _name)
    {
        auto & customState = ImGuiSettings::get()->m_customImGuiData;
        customState[_name] = CustomImGuiData();
        return &customState[_name];
    }

    //--------------------------------------------------------------------------------------
    void ImGuiSettings::onReadLine(ImGuiContext * _imGuiContext, ImGuiSettingsHandler * _imGuiSettingsHandler, void * _entry, const char * _line)
    {
        CustomImGuiData * data = (CustomImGuiData *)_entry;
        int isOpen;
        if (sscanf(_line, "Open=%d", &isOpen) == 1)
            data->isOpen = (isOpen != 0);
    }

    //--------------------------------------------------------------------------------------
    // Function to write data to `imgui.ini`
    //--------------------------------------------------------------------------------------
    void ImGuiSettings::onWriteAll(ImGuiContext * _imGuiContext, ImGuiSettingsHandler * _imGuiSettingsHandler, ImGuiTextBuffer * _outputTextBuffer)
    {
        const auto & customState = ImGuiSettings::get()->m_customImGuiData;
        for (const auto & pair : customState)
        {
            _outputTextBuffer->appendf("[%s][%s]\n", _imGuiSettingsHandler->TypeName, pair.first.c_str());
            _outputTextBuffer->appendf("Open=%d\n", pair.second.isOpen ? 1 : 0);
        }
    }

    //--------------------------------------------------------------------------------------
    CustomImGuiData & ImGuiSettings::getCustomData(const core::string & _name)
    {
        return m_customImGuiData[_name];
    }
}