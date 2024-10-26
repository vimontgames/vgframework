#pragma once

#include "core/Singleton/Singleton.h"

namespace vg::renderer
{
    class ImGuiSettings : public core::Singleton<ImGuiSettings>
    {
    public:
        void init();

        CustomImGuiData & getCustomData(const core::string & _name);

    private:
        static void * onReadOpen(ImGuiContext * _imGuiContext, ImGuiSettingsHandler * _imGuiSettingsHandler, const char * _name);
        static void onReadLine(ImGuiContext * _imGuiContext, ImGuiSettingsHandler * _imGuiSettingsHandler, void * _entry, const char * _line);
        static void onWriteAll(ImGuiContext * _imGuiContext, ImGuiSettingsHandler * _imGuiSettingsHandler, ImGuiTextBuffer * _outputTextBuffer);

    private:
        core::map<core::string, CustomImGuiData> m_customImGuiData;
    };
}
