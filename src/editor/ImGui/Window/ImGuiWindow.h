#pragma once

#include "core/Object/Object.h"

struct ImColor;

namespace imgui_addons
{
    class ImGuiFileBrowser;
}

namespace vg::core
{
    class ISelection;
}

namespace vg::engine
{
    class IEngine;
}

namespace vg::renderer
{
    class IRenderer;
}

namespace vg::gfx
{
    struct RenderPassContext;
}

namespace vg::editor
{
    struct TreeNodeStackInfo
    {
        bool treeNodeIsCollapsingHeader = false;
        bool treeNodeOpen = false;
    };

    struct ObjectContext
    {
        core::vector<TreeNodeStackInfo> treeNodes;
        bool hide = false;
    };

    class ImGuiWindow : public core::Object
    {
    public:
        enum Flags : core::u32
        {
            None            = 0x00000000,
            StartVisible    = 0x00000001,
            AddMenuEntry    = 0x00000002
        };
                                                ImGuiWindow         (const core::string & _icon, const core::string & _path, const core::string & _name, Flags _flags);
        virtual                                 ~ImGuiWindow        () {}

        virtual void                            DrawGUI             () = 0;

        bool                                    isVisible           () const;
        void                                    setVisible          (bool _visible);

        VG_INLINE const core::string &          getIcon             () const;
        VG_INLINE const core::string &          getPath             () const;
        VG_INLINE const core::string &          getName             () const;
        VG_INLINE Flags                         getFlags            () const;

        VG_INLINE const core::string            getIconizedName     () const;

        static void                             displayObject       (core::IObject * _object);
        static void                             displayObject       (core::IObject * _object, ObjectContext & _context);

        static void                             displayProperty     (core::IObject * _object, const core::IProperty * _prop);
        static void                             displayProperty     (core::IObject * _object, const core::IProperty * _prop, ObjectContext & _context);

        static bool                             displayResource     (core::IResource * _resource, const core::IProperty * _prop, core::uint _index = 0);

        static bool                             displayFloat4x4     (const core::string & _label, core::float4x4 * _pFloat4x4);
        static bool                             displayU32          (const core::string & _label, core::u32 * _pU32);

        static imgui_addons::ImGuiFileBrowser & getFileBrowser      ();
        static core::string                     getFileBrowserExt   (const core::IResource * _resource);
        static core::string                     getDefaultFolder    (const core::IResource * _resource);
        static core::string                     getDefaultFolder    (const core::string & _resourceTypeName);

        static engine::IEngine *                getEngine           ();
        static core::ISelection *               getSelection        ();

    //protected:
        static void                             underLine           (const ImColor & _color);
        static void                             textURL             (const char * _text, const char * _url);

        static bool                             isItemClicked       ();
        static bool                             updateSelection     (core::IObject * _object);

        static void                             displayArrayObject  (core::IObject * _object, core::uint _index, const char * _name);

        template <typename T> static bool       displayEnum         (core::IObject * _object, const core::IProperty * _prop);
        template <typename T> static bool       displayEnumFlags    (core::IObject * _object, const core::IProperty * _prop);

        static core::string                     getButtonLabel      (core::string _baseName, core::IObject * _object);

    //protected:
        core::string                            m_icon;
        core::string                            m_name;
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