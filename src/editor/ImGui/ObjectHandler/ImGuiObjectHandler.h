#pragma once

namespace vg::editor
{
    struct ObjectContext;
    struct PropertyContext;

    class ImGuiObjectHandler 
    {
    public:
        virtual ~ImGuiObjectHandler() = default;

        // Return 'true' if object is edited
        virtual bool displayObject(core::IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext = nullptr) = 0;
    
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
                // ImGui handlers must be explicitly destroyed using Destroy before ImGui shutdown in case they perform ImGui allocs (e.g. ImGuiTextFiler) 
            }

            ImGuiObjectHandler * m_handler = nullptr;
        };

        static ImGuiObjectHandler * Find(const core::string & _key)
        {
            for (auto i = 0; i < s_ImGuiHandlers.size(); ++i)
            {
                if (s_ImGuiHandlers[i].key == _key)
                    return s_ImGuiHandlers[i].handler;
            }
            return nullptr;
        }

        static void Destroy()
        {
            for (auto i = 0; i < s_ImGuiHandlers.size(); ++i)
                VG_SAFE_DELETE(s_ImGuiHandlers[i].handler);
            s_ImGuiHandlers.clear();
        }

    protected:
        struct HandlerInfo
        {
            core::string key;
            ImGuiObjectHandler * handler;
        };
        static core::vector<HandlerInfo> s_ImGuiHandlers;
    };
    
    #define VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(className, handler) vg::editor::ImGuiObjectHandler::AutoRegister<handler> autoRegister##className##handler(#className)  
}