#include "ImguiInput.h"
#include "core/IInput.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiInput::ImGuiInput() :
        ImGuiWindow(style::icon::Input, "", "Input", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiInput::DrawGUI()
    {
        if (IconBegin(getIcon().c_str(), getName().c_str(), &m_isVisible))
        {
            const IInput * input = Kernel::getInput();

            if (!input->IsInputEnabled(InputType::Keyboard))
                ImGui::PushStyleColor(ImGuiCol_Text, getDisabledTextColor());

            if (ImGui::CollapsingHeader("Keyboard", ImGuiTreeNodeFlags_DefaultOpen))
            {
                Columns(2, "Keyboard", true);
                float columnWidth[2] = { 196, 128 };

                SetColumnWidth(0, columnWidth[0]);
                SetColumnWidth(1, columnWidth[1]);

                for (uint b = 0; b < enumCount<core::Key>(); ++b)
                {
                    auto key = (core::Key)b;

                    if (input->IsKeyJustPressed(key) || input->IsKeyJustReleased(key) || input->IsKeyPressed(key))
                        Text("%s", asString(key).c_str());
                }

                NextColumn();

                for (uint b = 0; b < enumCount<core::Key>(); ++b)
                {
                    auto key = (core::Key)b;

                    if (input->IsKeyJustPressed(key))
                        Text("JustPressed");
                    else if (input->IsKeyJustReleased(key))
                        Text("JustReleased");
                    else if (input->IsKeyPressed(key))
                        Text("Pressed");
                }
            }

            if (!input->IsInputEnabled(InputType::Keyboard))
                ImGui::PopStyleColor();

            Columns(1, nullptr, true);

            if (!input->IsInputEnabled(InputType::Mouse))
                ImGui::PushStyleColor(ImGuiCol_Text, getDisabledTextColor());
            
            if (ImGui::CollapsingHeader("Mouse", ImGuiTreeNodeFlags_DefaultOpen))
            {
                Columns(2, "Mouse", true);
                float columnWidth[2] = { 196, 128 };
    
                SetColumnWidth(0, columnWidth[0]);
                SetColumnWidth(1, columnWidth[1]);

                Text("MousePos");

                for (uint b = 0; b < enumCount<core::MouseButton>(); ++b)
                {
                    auto button = (core::MouseButton)b;

                    if (input->IsMouseButtonJustPressed(button) || input->IsMouseButtonJustReleased(button) || input->IsMouseButtonPressed(button))
                        Text("%s Button", asString(button).c_str());
                }

                NextColumn();

                const uint2 mousePos = input->GetMousePos();

                Text("(%u, %u)", (uint)mousePos.x, (uint)mousePos.y);

                for (uint b = 0; b < enumCount<core::MouseButton>(); ++b)
                {
                    auto button = (core::MouseButton)b;

                    if (input->IsMouseButtonJustPressed(button))
                        Text("JustPressed");
                    else if (input->IsMouseButtonJustReleased(button))
                        Text("JustReleased");
                    else if (input->IsMouseButtonPressed(button))
                        Text("Pressed");
                }
            }

            if (!input->IsInputEnabled(InputType::Mouse))
                ImGui::PopStyleColor();

            Columns(1, nullptr, true);

            if (!input->IsInputEnabled(InputType::Joypads))
                ImGui::PushStyleColor(ImGuiCol_Text, getDisabledTextColor());

            const auto joyCount = input->GetJoyCount();
            for (uint i = 0; i < joyCount; ++i)
            {
                char joyLabel[256];
                sprintf_s(joyLabel, "JoyPad %u", i);

                Columns(1, joyLabel, true);

                if (ImGui::CollapsingHeader(joyLabel, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    Columns(2, joyLabel, true);
                    float columnWidth[2] = { 196, 128 };

                    uint id = i;

                    SetColumnWidth(0, columnWidth[0]);
                    SetColumnWidth(1, columnWidth[1]);

                    Text("LeftStick");
                    Text("RightStick");

                    for (uint b = 0; b < enumCount<core::JoyButton>(); ++b)
                    {
                        auto button = (core::JoyButton)b;
                        if (input->IsJoyButtonJustPressed(id, button) || input->IsJoyButtonJustReleased(id, button) || input->IsJoyButtonPressed(id, button))
                            Text("%s", asString(button).c_str());
                    }

                    NextColumn();

                    const float2 & joyLeftDir = input->GetJoyLeftStickDir(id);
                    Text("(%.2f, %.2f)", (float)joyLeftDir.x, (float)joyLeftDir.y);

                    const float2 & joyRightDir = input->GetJoyRightStickDir(id);
                    Text("(%.2f, %.2f)", (float)joyRightDir.x, (float)joyRightDir.y);

                    for (uint b = 0; b < enumCount<core::JoyButton>(); ++b)
                    {
                        auto button = (core::JoyButton)b;

                        if (input->IsJoyButtonJustPressed(id, button))
                            Text("JustPressed");
                        else if (input->IsJoyButtonJustReleased(id, button))
                            Text("JustReleased");
                        else if (input->IsJoyButtonPressed(id, button))
                            Text("Pressed");
                    }
                }
            }

            if (!input->IsInputEnabled(InputType::Joypads))
                ImGui::PopStyleColor();
        }
        End();
    }
}