#pragma once

namespace vg::renderer
{
    enum class GUITheme : core::u8
    {
        Classic,
        Dark,
        Light,
        Custom
    };

    class ImGuiThemesManager
    {
    public:
        static void set(GUITheme _theme);

    private:
        static void resetStyle();

        static void styleColorsClassic();
        static void styleColorsDark();
        static void styleColorsLight();
        static void styleColorsCustom();
    };
}