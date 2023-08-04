#pragma once

struct ImColor;

namespace imgui_addons
{
    class ImGuiFileBrowser;
}

namespace vg::engine
{
    class IEngine;
}

namespace vg::graphics::driver
{
    struct RenderContext;
}

namespace vg::graphics::renderer
{
    class ImguiEditor
    {
    public:
        enum Flags : core::u32
        {
            None            = 0x00000000,
            StartVisible    = 0x00000001,
            AddMenuEntry    = 0x00000002
        };
                                                ImguiEditor         (const core::string & _name, Flags _flags);
                                                virtual ~ImguiEditor() {}

        virtual void                            update              (double _dt) {};
        virtual void                            display             () = 0;

        bool                                    isVisible           () const;
        void                                    setVisible          (bool _visible);

        const core::string &                    getName             () const;
        Flags                                   getFlags            () const;

        static void                             displayObject       (core::IObject * _object);
        static void                             displayProperty     (const core::IProperty * _prop, core::IObject * _object);

        static imgui_addons::ImGuiFileBrowser & getFileBrowser      ();

        // TODO: move to editor DLL
        static core::IObject *                  getSelectedObject   ();
        static void                             setSelectedObject   (core::IObject * _object);
        static core::vector<core::IObject*> &   getSelectedObjects  ();
        static void                             setSelectedObjects  (core::vector<core::IObject*> & _objects);
        static bool                             isSelectedObject    (core::IObject* _object);
        static bool                             removeFromSelection (core::IObject * _object);
        static bool                             addToSelection      (core::IObject * _object);

    protected:
        static void                             underLine           (const ImColor & _color);
        static void                             textURL             (const char * _text, const char * _url);

        static bool                             isItemClicked       ();
        static bool                             updateSelection     (core::IObject * _object);

        const vg::engine::IEngine *             getEngine           () const;
        vg::engine::IEngine *                   getEngine           ();

        static void                             displayArrayObject  (core::IObject * _object, core::uint _index, const char * _name);
        static bool                             displayResource     (core::IResource * _resource);

        template <typename T> static bool       displayEnum         (core::IObject * _object, const core::IProperty * _prop);
        template <typename T> static bool       displayEnumFlags    (core::IObject * _object, const core::IProperty * _prop);

        static core::string                     getPropertyLabel    (const core::IProperty * _prop);
        static core::string                     getButtonLabel      (core::string _baseName, core::IObject * _object);

    protected:
        static core::vector<core::IObject*>     s_selection;
        core::string                            m_name;
        Flags                                   m_flags;
        bool                                    m_isVisible = true;
        core::uint                              m_selected = 0;
        const char *                            m_popup = nullptr;
    };
}