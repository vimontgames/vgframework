#pragma once

namespace vg::renderer
{
    vg_enum_class(vg::renderer, Font, core::u8,
        UbuntuMono = 0,
        Rowndies,
        RubikMonoOne
    );

    vg_enum_class(vg::renderer, FontStyle, core::u8,
        Regular = 0,
        Bold,
        Italic,
        Light
    );

    vg_enum_class(vg::renderer, Theme, core::u8,
        ImGui_Classic,
        ImGui_Dark,
        ImGui_Light,
        VimontGames_Grey,
        VimontGames_Dark
    );
}