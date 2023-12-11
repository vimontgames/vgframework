#include "editor/Precomp.h"
#include "EditorOptions.h"
#include "editor/Editor.h"
#include "renderer/IRenderer.h"

using namespace vg::core;

namespace vg::editor
{
    VG_REGISTER_OBJECT_CLASS(EditorOptions, "Editor Options");

    //--------------------------------------------------------------------------------------
    bool EditorOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(EditorOptions, renderer::GUITheme, m_guiTheme, "GUI Theme");

        registerPropertyGroupBegin(EditorOptions, "Gizmo");
        {
            registerPropertyEnum(EditorOptions, GizmoType, m_gizmo.m_type, "Type");
            registerPropertyEnum(EditorOptions, GizmoSpace, m_gizmo.m_space, "Space");

            registerPropertyGroupBegin(EditorOptions, "Snap Align");
            {
                registerOptionalProperty(EditorOptions, m_gizmo.m_snapTranslate, m_gizmo.m_translationSnap, "Translate");
                registerOptionalProperty(EditorOptions, m_gizmo.m_snapRotate, m_gizmo.m_rotationSnapInDegrees, "Rotate");
                registerOptionalProperty(EditorOptions, m_gizmo.m_snapScale, m_gizmo.m_scaleSnap, "Scale");
            }
        }
        registerPropertyGroupEnd(EditorOptions);

        // TODO: Move to menu or toolbar instead
        registerPropertyCallback(EditorOptions, load, "Load");
        registerPropertyCallbackEx(EditorOptions, save, "Save", IProperty::Flags::SameLine);

        return true;
    }

    static const char * filename = "Editor.xml";

    //--------------------------------------------------------------------------------------
    EditorOptions::EditorOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        load(this);
        Editor::get()->getRenderer()->GetImGuiAdapter()->SetGUITheme(m_guiTheme);
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::load(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        if (factory->loadFromXML(_object, filename))
        {
            auto * options = static_cast<EditorOptions *>(_object);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::save(IObject * _object)
    {
        const auto * factory = Kernel::getFactory();
        return factory->saveToXML(_object, filename);
    }

    //--------------------------------------------------------------------------------------
    void EditorOptions::OnPropertyChanged(IObject * _object, const core::IProperty & _prop, bool _notifyParent)
    {
        const char * name = _prop.getName();

        if (!strcmp(name, "m_guiTheme"))
        {
            auto imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
            imGuiAdapter->SetGUITheme(m_guiTheme);
        }
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::setGizmoType(GizmoType _gizmoType)
    {
        if (_gizmoType != m_gizmo.m_type)
        {
            m_gizmo.m_type = _gizmoType;
            VG_INFO("[Editor] Gizmo type changed to '%s'", asString(_gizmoType).c_str());
            return true;
        }
        return false;
    }  

    //--------------------------------------------------------------------------------------
    bool EditorOptions::setSnap(bool _enable)
    {
        switch (m_gizmo.m_type)
        {
            case GizmoType::Translate:
            if (_enable != m_gizmo.m_snapTranslate)
            {
                m_gizmo.m_snapTranslate = _enable;
                return true;
            }
            break;

            case GizmoType::Rotate:
            if (_enable != m_gizmo.m_snapRotate)
            {
                m_gizmo.m_snapRotate = _enable;
                return true;
            }
            break;

            case GizmoType::Scale:
            if (_enable != m_gizmo.m_snapScale)
            {
                m_gizmo.m_snapScale = _enable;
                return true;
            }
            break;
        }
        return false;
    }
    //--------------------------------------------------------------------------------------
    bool EditorOptions::getSnap() const
    {
        switch (m_gizmo.m_type)
        {
            default:
                VG_ASSERT_ENUM_NOT_IMPLEMENTED(m_gizmo.m_type);
                return false;

            case GizmoType::Translate:
                return m_gizmo.m_snapTranslate;

            case GizmoType::Rotate:
                return m_gizmo.m_snapRotate;

            case GizmoType::Scale:
                return m_gizmo.m_snapScale;
        }
    }
}