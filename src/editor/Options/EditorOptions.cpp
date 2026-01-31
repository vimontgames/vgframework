#include "editor/Precomp.h"
#include "EditorOptions.h"
#include "editor/Editor.h"
#include "renderer/IRenderer.h"
#include "renderer/ICameraSettings.h"

using namespace vg::core;

namespace vg::editor
{
    VG_REGISTER_CLASS(EditorOptions, "Editor Options");

    //--------------------------------------------------------------------------------------
    bool EditorOptions::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyEnum(EditorOptions, renderer::Theme, m_theme, "Theme");

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
            registerPropertyGroupEnd(EditorOptions);
        }
        registerPropertyGroupEnd(EditorOptions);

        registerPropertyGroupBegin(EditorOptions, "Camera");
        {
            registerPropertyObjectPtrEx(EditorOptions, m_cameraSettings, "Camera", PropertyFlags::Flatten);
            setPropertyDescription(EditorOptions, m_cameraSettings, "Editor camera settings");
        }
        registerPropertyGroupEnd(EditorOptions);

        registerPropertyGroupBegin(EditorOptions, "Console");
        {
            registerPropertyEnumBitfield(EditorOptions, core::LevelFlags, m_consoleOptions.m_levels, "Levels");
            setPropertyDescription(EditorOptions, m_consoleOptions.m_levels, "Select the VG_DEBUG/VG_WARNING/VG_ERROR messages visible in IDE output and console");
        }
        registerPropertyGroupEnd(EditorOptions);

        registerPropertyGroupBegin(EditorOptions, "Debug");
        {
            registerPropertyEnumBitfield(EditorOptions, EditorDebugFlags, m_editorDebugFlags, "Editor");
            setPropertyDescription(EditorOptions, m_editorDebugFlags, "Enable some editor debug features");

            registerPropertyEnumBitfield(EditorOptions, PropertiesDisplayFlags, m_propertiesDisplayFlags, "Properties");
            setPropertyDescription(EditorOptions, m_propertiesDisplayFlags, "Enable display of debug properties");
        }
        registerPropertyGroupEnd(EditorOptions);

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::IsDebugCulling() const
    { 
        return core::asBool(EditorDebugFlags::Culling & m_editorDebugFlags);
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::IsDebugInspector() const
    {
        return core::asBool(EditorDebugFlags::Inspector & m_editorDebugFlags);
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::IsDebugPicking() const
    {
        return core::asBool(EditorDebugFlags::Picking & m_editorDebugFlags);
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::IsDebugMatrixPropertyVisible() const
    {
        return core::asBool(PropertiesDisplayFlags::Matrix & m_propertiesDisplayFlags);
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::IsDebugRuntimePropertyVisible() const
    {
        return core::asBool(PropertiesDisplayFlags::Runtime & m_propertiesDisplayFlags);
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::IsDebugUIDPropertyVisible() const
    {
        return core::asBool(PropertiesDisplayFlags::UID & m_propertiesDisplayFlags);
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::IsMiscDebugPropertyVisible() const
    {
        return core::asBool(PropertiesDisplayFlags::Misc & m_propertiesDisplayFlags);
    }

    //--------------------------------------------------------------------------------------
    EditorOptions::EditorOptions(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        Load(false);

        m_cameraSettings->RegisterUID();

        Editor::get()->getRenderer()->GetImGuiAdapter()->SetGUITheme(m_theme);
    }

    //--------------------------------------------------------------------------------------
    bool EditorOptions::Load(bool _async)
    {
        if (_async)
        {
            return super::Load(true);
        }
        else
        {
            // Must be created before load
            VG_SAFE_RELEASE_ASYNC(m_cameraSettings);
            const auto * factory = Kernel::getFactory();
            m_cameraSettings = (renderer::ICameraSettings *)factory->CreateObject("CameraSettings");
            m_cameraSettings->SetParent(this);
            SetFile("Editor.xml");
            return super::Load(false);
        }
    }

    //--------------------------------------------------------------------------------------
    EditorOptions::~EditorOptions()
    {
        VG_SAFE_RELEASE(m_cameraSettings);
    }

    //--------------------------------------------------------------------------------------
    void EditorOptions::SetPropertyValue(const IProperty & _prop, void * _previousValue, void * _newValue)
    {
        if (&m_theme == _previousValue)
        {
            auto imGuiAdapter = Editor::get()->getRenderer()->GetImGuiAdapter();
            auto theme = *(renderer::Theme *)_newValue;
            imGuiAdapter->SetGUITheme(theme);
        }

        super::SetPropertyValue(_prop, _previousValue, _newValue);
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
    void EditorOptions::setNextGizmo()
    {
        const auto count = enumCount<GizmoType>();
        if ((int)m_gizmo.m_type + 1 == count)
            m_gizmo.m_type = (GizmoType)0;
        else
            m_gizmo.m_type = (GizmoType)((int)m_gizmo.m_type+1);
    }

    //--------------------------------------------------------------------------------------
    void EditorOptions::setPreviousGizmo()
    {
        const auto count = enumCount<GizmoType>();
        if ((int)m_gizmo.m_type == 0)
            m_gizmo.m_type = (GizmoType)(count-1);
        else
            m_gizmo.m_type = (GizmoType)((int)m_gizmo.m_type - 1);
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