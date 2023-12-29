#include "ImGuiConsole.h"
#include "editor/ImGui/Toolbar/ImGuiToolbar.h"

namespace vg::editor
{

    //--------------------------------------------------------------------------------------
    ImGuiConsole::ImGuiConsole() :
        ImGuiWindow(style::icon::Console, "", "Console", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
        init();
    }

    //--------------------------------------------------------------------------------------
    ImGuiConsole::~ImGuiConsole()
    {
        deinit();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiConsole::DrawGUI()
    {
        ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);

        if (IconBegin(style::icon::Console, "Console", &m_isVisible))
        {
            const auto warningColor = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
            const auto errorColor = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);

            // As a specific feature guaranteed by the library, after calling Begin() the last Item represent the title bar.
            // So e.g. IsItemHovered() will return true when hovering the title bar.
            // Here we create a context menu only available from the title bar.
            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Close Console"))
                    m_isVisible = false;
                ImGui::EndPopup();
            }

            //if (ImGui::Button("Clear")) 
            //    clear(); 
            //
            //ImGui::SameLine();
            //
            // Options menu
            if (ImGui::BeginPopup("Options"))
            {
                ImGui::Checkbox("Auto-scroll", &m_autoscroll);
                ImGui::EndPopup();
            }

            // Options, Filter
            if (ImGui::Button("Options"))
                ImGui::OpenPopup("Options");

            ImGui::SameLine();

            bool clearLog = false;
            bool copyToClipboard = false;// ImGui::Button("Copy");

            ImGui::SameLine();

            auto * logger = Kernel::getLogger();
            logger->Lock();
            {
                m_filter.Draw("Filter", 256);

                ImGui::SameLine();

                uint infoCount = 0, warningCount = 0, errorCount = 0;
                const auto count = logger->GetLogCount();
                for (uint i = 0; i < count; i++)
                {
                    const auto & item = logger->GetLog(i);
                    switch (item.level)
                    {
                        case Level::Info:
                            infoCount += item.count;
                            break;

                        case Level::Warning:
                            warningCount += item.count;
                            break;

                        case Level::Error:
                            errorCount += item.count;
                            break;
                    }
                }

                auto buttonSize = ImGuiToolbar::ComputeButtonSize();
                buttonSize.x *= 2;

                ImVec2 size = GetWindowContentRegionSize();
                ImGuiStyle & style = ImGui::GetStyle();

                char infoLabel[256];
                sprintf_s(infoLabel, "%s %u", style::icon::Info, infoCount);
                auto infoLabelWidth = ImGui::CalcTextSize(infoLabel).x + style.FramePadding.x * 2.0f;

                char warningLabel[256];
                sprintf_s(warningLabel, "%s %u", style::icon::Warning, warningCount);
                auto warningLabelWidth = ImGui::CalcTextSize(warningLabel).x + style.FramePadding.x * 2.0f;

                char errorLabel[256];
                sprintf_s(errorLabel, "%s %u", style::icon::Error, errorCount);
                auto errorLabelWidth = ImGui::CalcTextSize(errorLabel).x + style.FramePadding.x * 2.0f;

                float offset = size.x - infoLabelWidth - warningLabelWidth - errorLabelWidth - style.FramePadding.x;

                PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0, 0.5f));
                {
                    ImGui::SetCursorPosX(offset);
    
                    if (ImGui::TooltipButton(infoLabel, m_showInfo, true, "Show Infos"))
                        m_showInfo = !m_showInfo;

                    ImGui::SameLine();
    
                    if (ImGui::TooltipButton(warningLabel, m_showWarnings, true, "Show Warnings"))
                        m_showWarnings = !m_showWarnings;

                    ImGui::SameLine();
                  
                    if (ImGui::TooltipButton(errorLabel, m_showErrors, true, "Show Errors"))
                        m_showErrors = !m_showErrors;
                }
                PopStyleVar();

                //ImGui::SameLine();
                //
                //if (ImGui::SmallButton("Add Info"))
                //    VG_INFO("[Console] something went wrong");
                //
                //ImGui::SameLine();
                //
                //if (ImGui::SmallButton("Add Warning"))
                //    VG_WARNING("[Console] something went wrong");
                //
                //ImGui::SameLine();
                //
                //if (ImGui::SmallButton("Add Error")) 
                //    VG_ERROR("[Console] something went wrong");
             
                ImGui::Separator();

                // Reserve enough left-over height for 1 separator + 1 input text
                const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
                ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
                if (ImGui::BeginPopupContextWindow())
                {
                    if (ImGui::Selectable("Clear"))
                        clearLog = true;
                    if (ImGui::Selectable("Copy"))
                        copyToClipboard = true;
                    ImGui::EndPopup();
                }

                // Display every line as a separate entry so we can change their color or add custom widgets.
                // If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
                // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping
                // to only process visible items. The clipper will automatically measure the height of your first item and then
                // "seek" to display only items in the visible area.
                // To use the clipper we can replace your standard loop:
                //      for (int i = 0; i < Items.Size; i++)
                //   With:
                //      ImGuiListClipper clipper;
                //      clipper.Begin(Items.Size);
                //      while (clipper.Step())
                //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
                // - That your items are evenly spaced (same height)
                // - That you have cheap random access to your elements (you can access them given their index,
                //   without processing all the ones before)
                // You cannot this code as-is if a filter is active because it breaks the 'cheap random-access' property.
                // We would need random-access on the post-filtered list.
                // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices
                // or offsets of items that passed the filtering test, recomputing this array when user changes the filter,
                // and appending newly elements as they are inserted. This is left as a task to the user until we can manage
                // to improve this example code!
                // If your items are of variable height:
                // - Split them into same height items would be simpler and facilitate random-seeking into your list.
                // - Consider using manual call to IsRectVisible() and skipping extraneous decoration from your items.
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
                if (copyToClipboard)
                    ImGui::LogToClipboard();
            
                //const auto count = logger->GetLogCount();
                for (uint i = 0; i < count; i++)
                {
                    const auto & item = logger->GetLog(i);

                    switch (item.level)
                    {
                        case Level::Info:
                            if (!m_showInfo)
                                continue;
                            break;

                        case Level::Warning:
                            if (!m_showWarnings)
                                continue;
                            break;

                        case Level::Error:
                            if (!m_showErrors)
                                continue;
                            break;
                    }
  
                    string fullmsg;
                    
                    if (item.count > 1)
                        fullmsg = fmt::sprintf("[%s] %s (x%u)", item.category, item.message, item.count);
                    else
                        fullmsg = fmt::sprintf("[%s] %s", item.category, item.message);

                    if (!m_filter.PassFilter(fullmsg.c_str()))
                        continue;

                    ImVec4 color;
                    bool has_color = false;

                    switch (item.level)
                    {
                        case Level::Error:
                            color = errorColor; 
                            has_color = true; 
                            break;

                        case Level::Warning:
                            color = warningColor;
                            has_color = true;
                            break;
                    }

                    if (has_color)
                        ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::TextUnformatted(fullmsg.c_str());
                    if (has_color)
                        ImGui::PopStyleColor();
                }
            }
            logger->Unlock();

            if (copyToClipboard)
                ImGui::LogFinish();

            if (clearLog)
                clear();

            if (m_scrollToBottom || (m_autoscroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            m_scrollToBottom = false;

            ImGui::PopStyleVar();
            ImGui::EndChild();
            ImGui::Separator();

            // Command-line
            bool reclaim_focus = false;
            ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
            PushItemWidth(-1);
            if (ImGui::InputText("###Input", m_inputBuffer, IM_ARRAYSIZE(m_inputBuffer), input_text_flags, &textEditCallbackStub, (void *)this))
            {
                char * s = m_inputBuffer;
                Strtrim(s);
                if (s[0])
                    execute(s);
                strcpy(s, "");
                reclaim_focus = true;
            }
            PopItemWidth();

            // Auto-focus on window apparition
            ImGui::SetItemDefaultFocus();
            if (reclaim_focus)
                ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

            End();
        }
    }

    //--------------------------------------------------------------------------------------
    void ImGuiConsole::init()
    {
        memset(m_inputBuffer, 0, sizeof(m_inputBuffer));
        m_historyPos = -1;

        // "CLASSIFY" is here to provide the test case where "C"+[tab] completes to "CL" and display multiple matches.
        m_commands.push_back("HELP");
        m_commands.push_back("HISTORY");
        m_commands.push_back("CLEAR");
        m_commands.push_back("CLASSIFY");
        m_autoscroll = true;
        m_scrollToBottom = false;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiConsole::deinit()
    {
        clear();
        for (int i = 0; i < m_history.Size; i++)
            free(m_history[i]);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiConsole::clear()
    {
        Kernel::getLogger()->Clear();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiConsole::execute(const char * command_line)
    {
        VG_INFO("# %s", command_line);

        // Insert into history. First find match and delete it so it can be pushed to the back.
        // This isn't trying to be smart or optimal.
        m_historyPos = -1;
        for (int i = m_history.Size - 1; i >= 0; i--)
            if (Stricmp(m_history[i], command_line) == 0)
            {
                free(m_history[i]);
                m_history.erase(m_history.begin() + i);
                break;
            }
        m_history.push_back(Strdup(command_line));

        // Process command
        if (Stricmp(command_line, "CLEAR") == 0)
        {
            clear();
        }
        else if (Stricmp(command_line, "HELP") == 0)
        {
            VG_INFO("Commands:");
            for (int i = 0; i < m_commands.Size; i++)
                VG_INFO("- %s", m_commands[i]);
        }
        else if (Stricmp(command_line, "HISTORY") == 0)
        {
            int first = m_history.Size - 10;
            for (int i = first > 0 ? first : 0; i < m_history.Size; i++)
                VG_INFO("%3d: %s\n", i, m_history[i]);
        }
        else
        {
            VG_ERROR("Unknown command: '%s'\n", command_line);
        }

        // On command input, we scroll to bottom even if AutoScroll==false
        m_scrollToBottom = true;
    }

    //--------------------------------------------------------------------------------------
    // In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
    //--------------------------------------------------------------------------------------
    int ImGuiConsole::textEditCallbackStub(ImGuiInputTextCallbackData * data)
    {
        ImGuiConsole * console = (ImGuiConsole *)data->UserData;
        return console->textEditCallback(data);
    }

    //--------------------------------------------------------------------------------------
    int ImGuiConsole::textEditCallback(ImGuiInputTextCallbackData * data)
    {
        //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
        switch (data->EventFlag)
        {
            case ImGuiInputTextFlags_CallbackCompletion:
            {
                // Example of TEXT COMPLETION

                // Locate beginning of current word
                const char * word_end = data->Buf + data->CursorPos;
                const char * word_start = word_end;
                while (word_start > data->Buf)
                {
                    const char c = word_start[-1];
                    if (c == ' ' || c == '\t' || c == ',' || c == ';')
                        break;
                    word_start--;
                }

                // Build a list of candidates
                ImVector<const char *> candidates;
                for (int i = 0; i < m_commands.Size; i++)
                    if (Strnicmp(m_commands[i], word_start, (int)(word_end - word_start)) == 0)
                        candidates.push_back(m_commands[i]);

                if (candidates.Size == 0)
                {
                    // No match
                    VG_INFO("No match for \"%.*s\"!\n", (int)(word_end - word_start), word_start);
                }
                else if (candidates.Size == 1)
                {
                    // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing.
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0]);
                    data->InsertChars(data->CursorPos, " ");
                }
                else
                {
                    // Multiple matches. Complete as much as we can..
                    // So inputing "C"+Tab will complete to "CL" then display "CLEAR" and "CLASSIFY" as matches.
                    int match_len = (int)(word_end - word_start);
                    for (;;)
                    {
                        int c = 0;
                        bool all_candidates_matches = true;
                        for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                            if (i == 0)
                                c = toupper(candidates[i][match_len]);
                            else if (c == 0 || c != toupper(candidates[i][match_len]))
                                all_candidates_matches = false;
                        if (!all_candidates_matches)
                            break;
                        match_len++;
                    }

                    if (match_len > 0)
                    {
                        data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                        data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                    }

                    // List matches
                    VG_INFO("Possible matches:\n");
                    for (int i = 0; i < candidates.Size; i++)
                        VG_INFO("- %s\n", candidates[i]);
                }

                break;
            }
            case ImGuiInputTextFlags_CallbackHistory:
            {
                // Example of HISTORY
                const int prev_history_pos = m_historyPos;
                if (data->EventKey == ImGuiKey_UpArrow)
                {
                    if (m_historyPos == -1)
                        m_historyPos = m_history.Size - 1;
                    else if (m_historyPos > 0)
                        m_historyPos--;
                }
                else if (data->EventKey == ImGuiKey_DownArrow)
                {
                    if (m_historyPos != -1)
                        if (++m_historyPos >= m_history.Size)
                            m_historyPos = -1;
                }

                // A better implementation would preserve the data on the current input line along with cursor position.
                if (prev_history_pos != m_historyPos)
                {
                    const char * history_str = (m_historyPos >= 0) ? m_history[m_historyPos] : "";
                    data->DeleteChars(0, data->BufTextLen);
                    data->InsertChars(0, history_str);
                }
            }
        }
        return 0;
    }
}