#pragma once

namespace vg
{
    namespace renderer
    {
        vg_enum_class(Font, vg::core::u8,
            UbuntuMono = 0,
            Rowndies,
            RubikMonoOne
        );

        vg_enum_class(FontStyle, vg::core::u8,
            Regular = 0,
            Bold,
            Italic,
            Light
        );

        vg_enum_class(Theme, vg::core::u8,
            ImGui_Classic,
            ImGui_Dark,
            ImGui_Light,
            VimontGames_Grey,
            VimontGames_Dark
        );
    }
}