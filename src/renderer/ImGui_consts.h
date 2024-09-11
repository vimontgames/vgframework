#pragma once

namespace vg
{
    namespace renderer
    {
        vg_enum_class(Font, vg::core::u8,
            UbuntuMono
            //Awesome
        );

        vg_enum_class(Style, vg::core::u8,
            Regular = 0,
            Bold,
            Italic
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