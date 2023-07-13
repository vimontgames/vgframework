#pragma once

#include "imgui/imgui.h"
#include "graphics/renderer/Pass/ImguiPass/Windows/ImguiEditor.h"

namespace vg::graphics::renderer
{
    template <typename K, class C> class ImGuiCustomHandler
    {
    public:
        template <class T> class AutoRegister
        {
        public:
            AutoRegister(const char * className)
            {
                s_ImGuiHandlers.push_back({ className, new T() });
            }

            ~AutoRegister()
            {

            }
        };

        static C * Find(K key)
        {
            for (core::uint i = 0; i < s_ImGuiHandlers.count(); ++i)
            {
                if (s_ImGuiHandlers[i].key == key)
                    return s_ImGuiHandlers[i].handler;
            }
            return nullptr;
        }

    protected:
        struct HandlerInfo
        {
            K key;
            C * handler;
        };
        static core::vector<HandlerInfo> s_ImGuiHandlers;
    };

    #define VG_AUTO_REGISTER_IMGUI_CUSTOM_HANDLER(className, handler, keyType, handlerType) vg::graphics::renderer::ImGuiCustomHandler<keyType, handlerType>::AutoRegister<handler> autoRegister##handler(#className)
}