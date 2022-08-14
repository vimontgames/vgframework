#pragma once

struct ImColor;

namespace vg::engine
{
    class IEngine;
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

        virtual void                            display             () = 0;

        bool                                    isVisible           () const;
        void                                    setVisible          (bool _visible);

        const core::string &                    getName             () const;
        Flags                                   getFlags            () const;

        static void                             displayObject       (core::IObject * _object);
        static void                             underLine           (const ImColor & _color);
        static void                             textURL             (const char * _text, const char * _url);

        static bool                             isItemClicked       ();
        static bool                             updateSelection     (core::IObject * _object);
        static core::IObject *                  getSelectedObject   ();
        static void                             setSelectedObject   (core::IObject * _object);

    protected:
        static const vg::engine::IEngine *      getEngine           ();

        static void                             displayArrayObject  (core::IObject * _object, core::uint _index, const char * _name);
        static void                             displayProperty     (const core::IProperty * prop, core::IObject * _object);

        template <typename T> static bool       displayEnum         (core::IObject * _object, const core::IProperty * _prop);
        template <typename T> static bool       displayEnumFlags    (core::IObject * _object, const core::IProperty * _prop);

        static core::vector<core::IObject*> &   getSelectedObjects  ();
        static void                             setSelectedObjects  (core::vector<core::IObject*> & _objects);

        static core::string                     getPropertyLabel    (const core::IProperty * _prop);

    protected:
        static core::vector<core::IObject*>     s_selection;
        core::string                            m_name;
        Flags                                   m_flags;
        bool                                    m_isVisible = true;
    };
}