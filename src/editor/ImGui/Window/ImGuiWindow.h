#pragma once

#include "core/Object/Object.h"
#include "ImGuiObjectPropertyContext.h"

struct ImColor;

namespace imgui_addons
{
    class ImGuiFileBrowser;
}

namespace vg::core
{
    class ISelection;
    class IDynamicProperty;
}

namespace vg::engine
{
    class IEngine;
}

namespace vg::renderer
{
    class IRenderer;
    class IImGuiAdapter;
}

namespace vg::gfx
{
    struct RenderPassContext;
}

namespace vg::editor
{
    class ImGuiWindow : public core::Object
    {
    public:
        VG_CLASS_DECL(ImGuiWindow, core::Object);

        enum Flags : core::u32
        {
            None            = 0x00000000,
            StartVisible    = 0x00000001,
            AddMenuEntry    = 0x00000002
        };
                                                ImGuiWindow         (const core::string & _icon, const core::string & _path, const core::string & _name, Flags _flags);
        virtual                                 ~ImGuiWindow        () {}

        virtual core::string                    GetTitle            () const { VG_ASSERT_NOT_IMPLEMENTED(); return ""; } //= 0;
        virtual void                            DrawGUI             () = 0;

        bool                                    isVisible           () const;
        void                                    setVisible          (bool _visible);

        VG_INLINE const core::string &          getIcon             () const;
        VG_INLINE const core::string &          getPath             () const;
        VG_INLINE Flags                         getFlags            () const;

        VG_INLINE const core::string            getIconizedName     () const;

        static bool                             displayObject       (core::IObject * _object);
        static bool                             displayObject       (core::IObject * _object, ObjectContext & _objectContext);

        static bool                             displayProperty     (core::IObject * _object, const core::IProperty * _prop);
        static bool                             displayProperty     (core::IObject * _object, const core::IProperty * _prop, ObjectContext & _objectContext);

        static bool                             displayResource     (core::IResource * _resource, const core::IProperty * _prop, core::uint _index, PropertyContext & _propContext);

        static bool                             editBool            (core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext, const core::string & _label, float _availableWidth, bool _singleLine, core::uint _index = 0);
        static bool                             editFloat4x4        (core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext);
        static bool                             editString          (core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext, const core::string & _label, core::uint _index = 0);
        static bool                             editObjectHandle    (core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext);
        static bool                             editBitMask         (core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext);
        template <typename T> static bool       editEnum            (core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext);
        template <typename T> static bool       editEnumFlags       (core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext);

        static core::string                     getFileBrowserExt   (const core::IResource * _resource);
        static core::string                     getDefaultFolder    (const core::IResource * _resource);
        static core::string                     getDefaultFolder    (const core::string & _resourceTypeName);

        static engine::IEngine *                getEngine           ();
        static core::ISelection *               getSelection        ();

    //protected:
        static float                            getDragSpeedFloat   (const core::IProperty * _prop);
        static float                            getDragSpeedInt     (const core::IProperty * _prop);
        static void                             underLine           (const ImColor & _color);
        static void                             textURL             (const char * _text, const char * _url);

        static bool                             isItemClicked       ();
        static bool                             updateSelection     (core::IObject * _object);

        static void                             displayArrayObject  (core::IObject * _object, core::uint _index, const char * _name);

        static core::string                     getButtonLabel      (core::string _baseName, core::IObject * _object);

        static bool                             isPropertyVisible   (core::PropertyFlags _flags);
        static core::string                     getPropertyLabel    (const core::string & _label, core::uint _index = 0);
        static ImVec4                           getPropertyColor    (const PropertyContext & _propContext);
        static void                             drawPropertyLabel   (const PropertyContext & _propContext, const core::IProperty * _prop, core::uint _index = 0);
        static void                             drawPropertyLabel   (const PropertyContext & _propContext, const char * _label, const char * _description, core::uint _index = 0);

        static renderer::IImGuiAdapter *        getImGuiAdapter     ();

    protected:
        core::string                            m_icon;
        core::string                            m_path;
        Flags                                   m_flags;
        bool                                    m_isVisible = true;
        core::uint                              m_selected = 0;
        core::string                            m_popup;
    };
}

#if VG_ENABLE_INLINE
#include "ImGuiWindow.inl"
#endif