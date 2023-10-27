#pragma once

#include "editor/ImGui/Window/ImGuiWindow.h"

namespace vg::editor
{
    class ImGuiConsole : public ImGuiWindow
    {
    public:
        ImGuiConsole();
        ~ImGuiConsole();

        virtual void DrawGUI() final override;

    protected:
        void init();
        void deinit();

        void clear();

        void execute(const char * command_line);

        static int textEditCallbackStub(ImGuiInputTextCallbackData * data);
        int textEditCallback(ImGuiInputTextCallbackData * data);

        // Portable helpers
        static int   Stricmp(const char * s1, const char * s2) { int d; while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; } return d; }
        static int   Strnicmp(const char * s1, const char * s2, int n) { int d = 0; while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) { s1++; s2++; n--; } return d; }
        static char * Strdup(const char * s) { IM_ASSERT(s); size_t len = strlen(s) + 1; void * buf = malloc(len); IM_ASSERT(buf); return (char *)memcpy(buf, (const void *)s, len); }
        static void  Strtrim(char * s) { char * str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }

    private:
        char                    m_inputBuffer[256];
        ImVector<const char *>  m_commands;
        ImVector<char *>        m_history;
        int                     m_historyPos;    // -1: new line, 0..History.Size-1 browsing history.
        ImGuiTextFilter         m_filter;
        bool                    m_autoscroll;
        bool                    m_scrollToBottom;

        bool                    m_showInfo = true;
        bool                    m_showWarnings = true;
        bool                    m_showErrors = true;
    };
}