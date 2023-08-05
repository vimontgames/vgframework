#pragma once

#include "imgui/imgui.h"
#include "renderer/ImGui/Editors/ImguiEditor.h"

namespace vg::renderer
{
    template <typename K, class C> class ImGuiCustomHandler
    {
    public:
        template <class T> class AutoRegister
        {
        public:
            AutoRegister(const char * className)
            {
                m_handler = new T();
                s_ImGuiHandlers.push_back({ className, m_handler });
            }

            ~AutoRegister()
            {
                VG_SAFE_DELETE(m_handler);
            }

            C * m_handler = nullptr;
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

    #define VG_AUTO_REGISTER_IMGUI_CUSTOM_HANDLER(className, handler, keyType, handlerType) vg::renderer::ImGuiCustomHandler<keyType, handlerType>::AutoRegister<handler> autoRegister##handler(#className)
}