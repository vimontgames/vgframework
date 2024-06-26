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
    class IDynamicProperty;
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

        static void                             displayObject       (core::IObject * _object);
        static void                             displayObject       (core::IObject * _object, ObjectContext & _context);

        static void                             displayProperty     (core::IObject * _object, const core::IProperty * _prop);
        static void                             displayProperty     (core::IObject * _object, const core::IProperty * _prop, ObjectContext & _context);

        static bool                             displayResource     (core::IResource * _resource, const core::IProperty * _prop, core::uint _index = 0);

        static bool                             displayFloat4x4     (core::IObject * _object, const core::IProperty * _prop);
        static bool                             displayU32          (const core::string & _label, core::u32 * _pU32);

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

        struct Context
        {
            bool                        readOnly                = false;
            bool                        isPrefabInstance        = false;
            bool                        isPrefabOverride        = false;
            bool                        canPrefabOverride       = false;

            //core::IObject *           object                  = nullptr;
            //core::IProperty *         prop                    = nullptr;
            core::IGameObject *         prefab                  = nullptr;
            core::IDynamicProperty *    propOverride            = nullptr;

            core::IObject *             optionalObject          = nullptr;
            core::IProperty *           optionalProp            = nullptr;
            core::IDynamicProperty *    optionalPropOverride    = nullptr;        
        };

        template <typename T> static bool       displayEnum         (core::IObject * _object, const core::IProperty * _prop, Context & _context);
        template <typename T> static bool       displayEnumFlags    (core::IObject * _object, const core::IProperty * _prop, Context & _context);

        static core::string                     getButtonLabel      (core::string _baseName, core::IObject * _object);

    private:
        static bool                             isPropertyVisible   (core::IProperty::Flags _flags);
        static core::string                     getPropertyLabel    (const core::string & _label);
        static ImVec4                           getPropertyColor    (const Context & _context);
        static void                             drawPropertyLabel   (const char * _label, const Context & _context);

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