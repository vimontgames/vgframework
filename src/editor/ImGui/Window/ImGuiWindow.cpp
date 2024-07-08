#include "editor/Precomp.h"
#include "core/IObject.h"
#include "core/Math/Math.h"
#include "core/File/File.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IWorld.h"
#include "core/IBaseScene.h"
#include "core/IGameObject.h"
#include "core/IComponent.h"
#include "core/ISelection.h"
#include "core/Misc/BitMask/BitMask.h"
#include "core/Misc/AABB/AABB.h"
#include "core/Object/DynamicProperties/DynamicProperties.h"

#include "renderer/IRenderer.h"

#include "engine/IEngine.h"
#include "engine/IResourceManager.h"

#include "editor/ImGui/ImGui.h"
#include "editor/Editor_Consts.h"
#include "editor/imgui/Extensions/imGuiExtensions.h"
#include "editor/imgui/Extensions/FileDialog/ImGuiFileDialog.h"

#include "ImGuiWindow.h"

#if !VG_ENABLE_INLINE
#include "ImGuiWindow.inl"
#endif

using namespace vg::core;
using namespace vg::gfx;
using namespace ImGui;

#include "editor/Editor.h"
#include "editor/ImGui/Window/FPS/ImGuiFPS.hpp"
#include "editor/ImGui/Window/Plugin/ImGuiPlugin.hpp"
#include "editor/ImGui/Window/Platform/ImGuiPlatform.hpp"
#include "editor/ImGui/Window/About/ImGuiAbout.hpp"
#include "editor/ImGui/Window/Input/ImGuiInput.hpp"
#include "editor/ImGui/Window/Time/ImGuiTime.hpp"
#include "editor/ImGui/Window/Inspector/ImGuiInspector.hpp"
#include "editor/ImGui/Window/Shader/ImGuiShader.hpp"
#include "editor/ImGui/Window/SceneList/ImGuiSceneList.hpp"
#include "editor/ImGui/Window/SceneList/World/ImGuiWorld.hpp"
#include "editor/ImGui/Window/SceneList/Prefab/ImGuiPrefab.hpp"
#include "editor/ImGui/Window/Resource/ImGuiResource.hpp"
#include "editor/ImGui/Window/View/ImGuiView.hpp"
#include "editor/ImGui/Window/View/EditorView/ImGuiEditorView.hpp"
#include "editor/ImGui/Window/View/PrefabView/ImGuiPrefabView.hpp"
#include "editor/ImGui/Window/View/GameView/ImGuiGameView.hpp"
#include "editor/ImGui/Window/Console/ImGuiConsole.hpp"
#include "editor/ImGui/Window/Statistics/ImGuiStatistics.hpp"

#include "editor/ImGui/ObjectHandler/ImGuiObjectHandler.h"
#include "editor/ImGui/Menu/Inspector/Component/ImGuiComponentInspectorMenu.h"

#include "shlobj_core.h"

namespace vg::editor
{
    const char * g_editFloatFormat = "%g";

    //--------------------------------------------------------------------------------------
    core::string ImGuiWindow::getFileBrowserExt(const core::IResource * _resource)
    {
        VG_ASSERT(_resource);
        string ext = "";
        bool first = true;
        const auto & extensions = _resource->getExtensions();
        for (uint e = 0; e < extensions.size(); ++e)
        {
            if (!first)
                ext += ",";
            ext += extensions[e];
            first = false;
        }
        if (ext == "")
            ext = "*.*";
        return ext;
    }

    //--------------------------------------------------------------------------------------
    engine::IEngine * ImGuiWindow::getEngine()
    {
        return Editor::get()->getEngine();
    }

    //--------------------------------------------------------------------------------------
    core::ISelection * ImGuiWindow::getSelection()
    {
        return Editor::get()->getEngine()->GetSelection();
    }

    //--------------------------------------------------------------------------------------
    ImGuiWindow::ImGuiWindow(const core::string & _icon, const core::string & _path, const string & _name, Flags _flags) :
        super(_name),
        m_icon(_icon),
        m_path(_path),
        m_flags(_flags),
        m_isVisible(asBool(_flags & Flags::StartVisible))
    {

    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::isVisible() const
    {
        return m_isVisible;
    }
    
    //--------------------------------------------------------------------------------------
    void ImGuiWindow::setVisible(bool _visible)
    {
        m_isVisible = _visible;
        ImGui::SetWindowFocus(getIconizedName().c_str());
    }

    //--------------------------------------------------------------------------------------
    string ImGuiWindow::getButtonLabel(string _baseName, IObject * _object)
    {
        return _baseName + "##" + _object->GetClassName() + "##" + to_string((u64)&_object); // TODO: Object GUID?
    }

    template <typename T> struct TypeToDynamicPropertyTypeEnum;
    template <> struct TypeToDynamicPropertyTypeEnum<bool> { using type = core::DynamicPropertyBool; };

    template <> struct TypeToDynamicPropertyTypeEnum<float> { using type = core::DynamicPropertyFloat; };
    template <> struct TypeToDynamicPropertyTypeEnum<float2> { using type = core::DynamicPropertyFloat2; };
    template <> struct TypeToDynamicPropertyTypeEnum<float3> { using type = core::DynamicPropertyFloat3; };
    template <> struct TypeToDynamicPropertyTypeEnum<float4> { using type = core::DynamicPropertyFloat4; };
    template <> struct TypeToDynamicPropertyTypeEnum<float4x4> { using type = core::DynamicPropertyFloat4x4; };

    template <> struct TypeToDynamicPropertyTypeEnum<core::u8> { using type = core::DynamicPropertyU8; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::u16> { using type = core::DynamicPropertyU16; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::u32> { using type = core::DynamicPropertyU32; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::u64> { using type = core::DynamicPropertyU64; };

    template <> struct TypeToDynamicPropertyTypeEnum<core::uint2> { using type = core::DynamicPropertyUInt2; };

    template <> struct TypeToDynamicPropertyTypeEnum<core::i8> { using type = core::DynamicPropertyI8; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::i16> { using type = core::DynamicPropertyI16; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::i32> { using type = core::DynamicPropertyI32; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::i64> { using type = core::DynamicPropertyI64; };

    template <typename T> bool equals(T a, T b)                 {  return a == b; }

    template <> bool equals(core::uint2 a, core::uint2 b) { return all(a == b); }
    template <> bool equals(core::uint3 a, core::uint3 b) { return all(a == b); }
    template <> bool equals(core::uint4 a, core::uint4 b) { return all(a == b); }

    template <> bool equals(core::float2 a, core::float2 b)     { return all(a == b); }
    template <> bool equals(core::float3 a, core::float3 b)     { return all(a == b); }
    template <> bool equals(core::float4 a, core::float4 b)     { return all(a == b); }
    template <> bool equals(core::float4x4 a, core::float4x4 b) { return all(a == b); }

    //--------------------------------------------------------------------------------------
    template <typename T> bool storeProperty(T * _out, T _value, IObject * _object, const IProperty * _prop, Context & _context)
    {
        if (!_context.readOnly && !equals(*_out,_value))
        {
            if (_context.isPrefabInstance && !_context.isPrefabOverride)
            {
                if (_context.propOverride = _context.prefab->CreateDynamicProperty(_object, _prop))
                {
                    ((typename TypeToDynamicPropertyTypeEnum<T>::type *)_context.propOverride)->SetValue(_value);
                    _context.propOverride->Enable(true);

                    if (_context.optionalPropOverride)
                        _context.optionalPropOverride->Enable(true);

                    _object = (IObject *)_context.propOverride;
                    _context.isPrefabOverride = true;

                    return true;
                } 
                else
                {
                    VG_ASSERT(false, "[Factory] Could not create DynamicProperty \"%s\" for class \"%s\"", _prop->getName(), _object->GetClassName());

                    return false;
                }
            }
            
            if (_context.isPrefabOverride)
            {
                VG_ASSERT(_context.originalObject != _object);
                *_out = _value;
            }
            else
            {
                VG_ASSERT(_context.originalObject == _object);
                _object->SetPropertyValue(*_prop, _out, &_value);
            }

            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool displayEnumRecur(string _enumName, uint _e, T * _pEnum, bool _readonly)
    {
        bool changed = false;

        auto it = _enumName.find_first_of("_");
        if (it != string::npos)
        {
            string category = _enumName.substr(0, it);
            if (ImGui::BeginMenu(category.c_str()))
            {
                string name = _enumName.substr(it + 1);
                bool res = displayEnumRecur(name, _e, _pEnum, _readonly);
                ImGui::EndMenu();
                return res;
            }
        }
        else
        {
            if (ImGui::Selectable(_enumName.c_str()))
            {
                if (!_readonly)
                {
                    *_pEnum = _e;
                    changed = true;
                }
            }
        }

        return changed;
    };

    //--------------------------------------------------------------------------------------
    template <typename T> bool ImGuiWindow::displayEnum(core::IObject * _object, const core::IProperty * _prop, Context & _context)
    {
        const auto displayName = _prop->getDisplayName();
        const auto offset = _prop->getOffset();
        const auto flags = _prop->getFlags();

        const bool readonly = asBool(IProperty::Flags::ReadOnly & flags);
        ImGui::BeginDisabled(readonly);

        T * pEnum = (T*)(uint_ptr(_object) + offset);
        int enumVal = (int)*pEnum;
        T temp = *pEnum;

        string preview = "<Invalid>";
        for (uint e = 0; e < _prop->getEnumCount(); ++e)
        {
            if (enumVal == _prop->getEnumValue(e))
            {
                preview = _prop->getEnumName(e);

                auto it = preview.find_first_of("_");
                while (string::npos != it)
                {
                    preview = preview.replace(it, 1, "/");
                    it = preview.find_first_of("_");
                }
                break;
            }
        }

        bool changed = false;

        string enumLabel = ImGui::getObjectLabel(_prop->getDisplayName(), _prop);
        if (ImGui::BeginCombo(getPropertyLabel(enumLabel).c_str(), preview.c_str(), ImGuiComboFlags_HeightLarge))
        {
            for (uint e = 0; e < _prop->getEnumCount(); ++e)
            {
                const string enumName = _prop->getEnumName(e);
                if (displayEnumRecur(enumName, e, &temp, readonly))
                {
                    if (storeProperty<T>((T *)pEnum, temp, _object, _prop, _context))
                        changed = true;
                }
            }
            ImGui::EndCombo();
        }

        drawPropertyLabel(_prop->getDisplayName(), _context);

        ImGui::EndDisabled();
        return changed;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool ImGuiWindow::displayEnumFlags(core::IObject * _object, const core::IProperty * _prop, Context & _context)
    {
        const auto displayName = _prop->getDisplayName();
        const auto offset = _prop->getOffset();
        const auto flags = _prop->getFlags();

        const bool readonly = asBool(IProperty::Flags::ReadOnly & flags);
        ImGui::BeginDisabled(readonly);

        T * pEnum = (T*)(uint_ptr(_object) + offset);
        int enumVal = (int)*pEnum;

        bool edited = false, changed = false;
        string preview;

        bool first = true, found = false;
        for (uint e = 0; e < _prop->getEnumCount(); ++e)
        {
            if (enumVal & (1 << e))
            {
                found = true;

                if (first)
                    first = false;
                else
                    preview += "|";

                preview += _prop->getEnumName(e);
            }
        }
        if (!found)
            preview = "<None>";

        string enumLabel = ImGui::getObjectLabel(_prop->getDisplayName(), _prop);
        if (ImGui::BeginCombo(getPropertyLabel(enumLabel).c_str(), preview.c_str(), ImGuiComboFlags_None))
        {
            for (uint e = 0; e < _prop->getEnumCount(); ++e)
            {
                bool value = ((enumVal >> e) & 1) ? true : false;
                const char * name = _prop->getEnumName(e);
                if (ImGui::Checkbox(name, &value))
                {
                    if (!readonly)
                    {
                        if (value)
                            enumVal |= 1 << e;
                        else
                            enumVal &= ~(1 << e);
                    }
                    edited = true;
                }
            }
            ImGui::EndCombo();

            if (edited)
            {
                if (storeProperty<T>((T *)pEnum, enumVal, _object, _prop, _context))
                {
                    *pEnum = enumVal;
                    changed = true;
                }
            }
        }

        drawPropertyLabel(_prop->getDisplayName(), _context);

        ImGui::EndDisabled();
        return changed;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::displayObject(core::IObject * _object)
    {
        ObjectContext context;
        displayObject(_object, context);
    }
    //--------------------------------------------------------------------------------------
    void ImGuiWindow::displayProperty(core::IObject * _object, const core::IProperty * _prop)
    {
        ObjectContext context;
        displayProperty(_object, _prop, context);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::displayObject(core::IObject * _object, ObjectContext & _context)
    {
        const char * className = _object->GetClassName();

        const auto * factory = Kernel::getFactory();
        const auto * classDesc = factory->getClassDescriptor(className);

        if (!classDesc)
            return;

        if (_context.treeNodes.size() > 0)
        {
            auto & nodeInfo = _context.treeNodes[_context.treeNodes.size() - 1];
            if (!nodeInfo.treeNodeOpen)
                return;
        }
        auto availableWidth = GetContentRegionAvail().x;
        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

        // TODO: Custom Object edit
        if (!ImGuiObjectHandler::display(_object))
        {
            const char * classDisplayName = classDesc->GetClassDisplayName();
        
            for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->GetPropertyByIndex(i);
                ImGuiWindow::displayProperty(_object, prop, _context);
            }
        }

        ImGui::PopItemWidth();
    }

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::displayArrayObject(IObject * _object, core::uint _index, const char * _name)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_Text));

        string treeNodeName;

        if (_name)
            treeNodeName = "[" + string(_name) + "]";
        else if (_object)
            treeNodeName = fmt::sprintf("[%u] %s", _index, _object->getName());
        else
            treeNodeName = "[" + to_string(_index) + "]";

        auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

        const bool isComponent = nullptr != _object && _object->GetClassDesc() && asBool(IClassDesc::Flags::Component & _object->GetClassDesc()->GetFlags());
        if (isComponent)
        {
            displayObject(_object);
        }
        else
        {
            if (treeNodeName.length() == 0)
                treeNodeName = "[" + to_string(_index) + "]";

            if (ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
            {
                updateSelection(_object);

                if (_object)
                    displayObject(_object);
                //else
                //    ImGui::TextDisabled("<Null>");

                ImGui::TreePop();
            }
            else
                updateSelection(_object);
        }

        ImGui::PopStyleColor();
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::isPropertyVisible(IProperty::Flags _flags)
    {
        const bool invisible = asBool(IProperty::Flags::NotVisible & _flags);
        const bool debug = asBool(IProperty::Flags::Debug & _flags);
        const bool showDebug = EditorOptions::get()->IsDebugPropertyVisible();

        return !invisible && (!debug || showDebug);
    }

    //--------------------------------------------------------------------------------------
    string ImGuiWindow::getPropertyLabel(const string & _label)
    {
        return fmt::sprintf("##%s", _label);
    }

    //--------------------------------------------------------------------------------------
    ImVec4 ImGuiWindow::getPropertyColor(const Context & _context)
    {
        ImGui::SameLine();

        static bool test = false;
        if (test)
            return ImGui::GetStyleColorVec4(ImGuiCol_Button);

        if (_context.isPrefabInstance)
        {
            if (_context.isPrefabOverride)
                return ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive);
            else if (_context.canPrefabOverride)
                return ImGui::GetStyleColorVec4(ImGuiCol_Text);
            else
                return ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
        }
        else
        {
            if (_context.readOnly)
                return ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
            else
                return ImGui::GetStyleColorVec4(ImGuiCol_Text);
        }
    };

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::drawPropertyLabel(const char * _label, const Context & _context)
    {
        auto x = ImGui::GetCursorPosX();

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Text, getPropertyColor(_context));

        // ugly workaround
        if (x>100)
            ImGui::SetCursorPosX(x);

        ImGui::Text(_label);
        ImGui::PopStyleColor();
    };

    //--------------------------------------------------------------------------------------
    // Find parent GameObject and Component if any
    //--------------------------------------------------------------------------------------
    IGameObject * findParentGameObject(const core::IObject * _object)
    {
        const IObject * object = _object;
        while (object)
        {
            if (auto * gameobject = dynamic_cast<const IGameObject *>(object))
                return (IGameObject*)gameobject;

            object = object->getParent();
        }
        return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::displayProperty(core::IObject * _object, const IProperty * _prop, ObjectContext & _objectContext)
    {
        VG_ASSERT(nullptr != _prop);

        const auto * factory = Kernel::getFactory();
        const auto cursorPosY = GetCursorPosY();
        const auto availableWidth = GetContentRegionAvail().x;
        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

        // TODO: Custom property edit
        //if (ImGuiPropertyHandler::display(_prop, _object))
        //    return; 

        IGameObject * gameobject = findParentGameObject(_object);

        Context context;
        context.originalObject      = _object;
        context.originalProp        = _prop;
        context.isPrefabInstance    = false;   // Property is from an instanced Prefab (cannot be edited directly)
        context.isPrefabOverride    = false;   // Prefab has overrides
        context.canPrefabOverride   = false;  // Property can be override for this Prefab Instance
        context.prefab              = nullptr;           // Original Prefab
        context.propOverride        = nullptr;     // The dynamic property override if any

        if (gameobject)
        {
            context.prefab = gameobject->GetParentPrefab();
            context.isPrefabInstance = nullptr != context.prefab;

            if (nullptr != context.prefab)
            {
                context.canPrefabOverride = context.prefab->CanOverrideProperty(_object, _prop);

                if (context.canPrefabOverride)
                {
                    if (auto * propList = context.prefab->GetDynamicPropertyList(_object))
                    {
                        if (context.propOverride = (IDynamicProperty *)propList->GetProperty(_prop))
                        {
                            if (context.propOverride->IsEnable())
                            {
                                _object = (IObject *)context.propOverride;
                                _prop = context.propOverride->GetProperty();
                                context.isPrefabOverride = true;
                            }
                        }
                    }
                }
            }
        }

        const auto type = _prop->getType();
        const auto name = _prop->getName();
        const auto displayName = _prop->getDisplayName();
        const auto label = ImGui::getObjectLabel(displayName, _prop);
        const auto offset = _prop->getOffset();
        const auto flags = _prop->getFlags();

        if (_objectContext.treeNodes.size() > 0)
        {
            auto nodeInfo = _objectContext.treeNodes[_objectContext.treeNodes.size() - 1];
            if (!nodeInfo.treeNodeOpen)
            {
                if (IProperty::Type::LayoutElement != type)
                    return;
            }
        }

        if (_objectContext.hide && (type != IProperty::Type::LayoutElement || !(asBool(flags & IProperty::Flags::Optional))))
            return;

        if (!isPropertyVisible(flags))
            return;

        if (asBool(IProperty::Flags::SameLine & flags))
            ImGui::SameLine();

        const bool hexa = asBool(IProperty::Flags::Hexadecimal & flags);

        bool optional = false, optionalChanged = false;

        context.optionalObject = nullptr;
        context.optionalProp = nullptr;
        context.optionalPropOverride = nullptr;

        if (asBool(IProperty::Flags::Optional & flags))
        {
            // check previous property is bool
            const IClassDesc * classDesc = context.originalObject->GetClassDesc();
            context.optionalObject = context.originalObject;
            auto * previousProp = classDesc->GetPreviousProperty(context.originalProp->getName());
            context.optionalProp = previousProp;

            if (context.isPrefabInstance /* && canPrefabOverride*/)
            {
                // Use override if it already exists but do not create it yet
                if (context.optionalPropOverride = context.prefab->GetDynamicProperty(context.originalObject, previousProp))
                {
                    context.optionalObject = (IObject *)context.optionalPropOverride;
                    context.optionalProp = context.optionalPropOverride->GetProperty();
                }
            }

            if (context.optionalProp)
            {
                VG_ASSERT(asBool(IProperty::Flags::NotVisible & context.optionalProp->getFlags()) || _prop->getType() == IProperty::Type::LayoutElement, "[Factory] Property used for optional variable \"%s\" should be %s", _prop->getName(), asString(IProperty::Flags::NotVisible).c_str());
                
                if (context.optionalProp->getType() == IProperty::Type::Bool)
                {
                    bool * b = context.optionalProp->GetPropertyBool(context.optionalObject);
                    bool temp = *b;

                    if (_prop->getType() != IProperty::Type::LayoutElement)
                    {
                        if (ImGui::Checkbox(ImGui::getObjectLabel("", context.optionalProp).c_str(), &temp))
                        {
                            optionalChanged = true;

                            if (context.isPrefabInstance /* && canPrefabOverride*/)
                            {
                                // Create prop override for bool 
                                const IClassDesc * classDesc = context.originalObject->GetClassDesc();
                                IProperty * originalOptionalProp = classDesc->GetPreviousProperty(context.originalProp->getName());
                            
                                // Create if needed
                                if (context.optionalPropOverride = context.prefab->CreateDynamicProperty(context.originalObject, originalOptionalProp))
                                {
                                    context.optionalObject = (IObject *)context.optionalPropOverride;
                                    context.optionalProp = context.optionalPropOverride->GetProperty();
                            
                                    ((core::DynamicPropertyBool *)context.optionalPropOverride)->SetValue(temp);
                                    context.optionalPropOverride->Enable(true);
                            
                                    if (context.propOverride)
                                        context.propOverride->Enable(true);
                                }
                            }

                            *b = temp;
                        }
                        ImGui::SameLine();

                        auto availableWidth = GetContentRegionAvail().x;
                        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

                        optional = true;

                        if (!*b)
                            ImGui::BeginDisabled(true);
                    }
                }                
            }
        }

        bool changed = false;

        // create temp property to store previous value
        //void * previousValue = nullptr;
        //Property previousPropValue = *(Property*)_prop;
        //switch (_prop->getType())
        //{
        //case IProperty::Type::Bool:
        //    previousValue = malloc(_prop->getSizeOf());
        //    break;
        //}

        context.readOnly = asBool(IProperty::Flags::ReadOnly & flags) || (context.isPrefabInstance && !context.isPrefabOverride && !context.canPrefabOverride);

        ImGuiInputTextFlags imguiInputTextflags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (context.readOnly)
            imguiInputTextflags = ImGuiInputTextFlags_ReadOnly;

        ImGuiInputTextFlags imguiNumberTextInputFlag = imguiInputTextflags;
        if (hexa)
            imguiNumberTextInputFlag |= ImGuiInputTextFlags_CharsHexadecimal;
        //else
        //    imguiNumberTextInputFlag |= ImGuiInputTextFlags_CharsDecimal;

        const bool flatten = asBool(IProperty::Flags::Flatten & flags);
        const bool isEnumArray = asBool(IProperty::Flags::EnumArray & flags);
        const auto enumArrayTreeNodeFlags = /*ImGuiTreeNodeFlags_OpenOnArrow |*/ ImGuiTreeNodeFlags_DefaultOpen;

        //if (isPrefabInstance)
        //{
        //    if (isPrefabOverride)
        //        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
        //    else
        //        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        //}

        ///*if (isPrefabOverride)
        //    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
        //else*/ if (readOnly)
        //    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

        //ImGui::BeginDisabled(readOnly);
        {
            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case IProperty::Type::LayoutElement:
                {
                    auto type = _prop->GetLayoutElementType();
                    switch (type)
                    {
                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                            break;

                        case IProperty::LayoutElementType::Separator:
                            ImGui::SeparatorText(_prop->getDisplayName());
                            break;

                        case IProperty::LayoutElementType::GroupBegin:
                        {
                            if (asBool(IProperty::Flags::Optional & flags))
                            {
                                // optional group
                                VG_ASSERT(_prop->getOffset() != 0);
                                //_context.hide = !*_prop->GetPropertyBool(_object);
                                ImGui::BeginDisabled(!*_prop->GetPropertyBool(_object));
                            }
                            else
                            {
                                if (_objectContext.treeNodes.size() > 0 || dynamic_cast<IComponent *>(_object) || dynamic_cast<IComponent *>(_object->getParent()))
                                {
                                    TreeNodeStackInfo & newInfo = _objectContext.treeNodes.push_empty();

                                    newInfo.treeNodeOpen = ImGui::TreeNodeEx(ImGui::getObjectLabel(_prop->getDisplayName(), _prop).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
                                    newInfo.treeNodeIsCollapsingHeader = false;
                                }
                                else
                                {
                                    TreeNodeStackInfo & newInfo = _objectContext.treeNodes.push_empty();

                                    newInfo.treeNodeOpen = ImGui::CollapsingHeader(ImGui::getObjectLabel(_prop->getDisplayName(), _prop).c_str(), ImGuiTreeNodeFlags_DefaultOpen);
                                    newInfo.treeNodeIsCollapsingHeader = true;
                                }
                            }
                        }
                        break;

                        case IProperty::LayoutElementType::GroupEnd:
                        {
                            if (asBool(IProperty::Flags::Optional & flags))
                            {
                                ImGui::EndDisabled();
                            }
                            else
                            {
                                if (_objectContext.treeNodes.size() > 0)
                                {
                                    auto & nodeInfo = _objectContext.treeNodes[_objectContext.treeNodes.size() - 1];

                                    if (nodeInfo.treeNodeOpen)
                                    {
                                        if (!nodeInfo.treeNodeIsCollapsingHeader)
                                            ImGui::TreePop();
                                    }

                                    _objectContext.treeNodes.erase(_objectContext.treeNodes.begin() + _objectContext.treeNodes.size() - 1);
                                }
                            }
                        }
                        break;
                    }
                }
                break;

                case IProperty::Type::BitMask:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    BitMask * pBitMask = _prop->GetPropertyBitMask(_object);

                    string enumLabel = ImGui::getObjectLabel(_prop->getDisplayName(), _prop);
                    string preview = pBitMask->toString();

                    if (ImGui::BeginCombo(enumLabel.c_str(), preview.c_str(), ImGuiComboFlags_HeightLarge))
                    {
                        const auto bitCount = pBitMask->getBitCount();
                        const auto names = pBitMask->getNames();

                        for (uint i = 0; i < bitCount; ++i)
                        {
                            bool value = pBitMask->getBitValue(i);
                            string bitName;
                            
                            if (i < names.size() && !names[i].empty())
                                bitName = fmt::sprintf("[%u] %s", i, names[i]);
                            else 
                                bitName = fmt::sprintf("Bit %u", i);
                            
                            if (ImGui::Checkbox(bitName.c_str(), &value))
                            {
                                if (!context.readOnly)
                                {
                                    if (value)
                                        pBitMask->setBitValue(i, true);
                                    else
                                        pBitMask->setBitValue(i, false);
                                }
                                changed = true;
                            }
                        }
                        ImGui::EndCombo();
                    }
                }
                break;

                case IProperty::Type::Bool:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    bool * pBool = _prop->GetPropertyBool(_object);
                    bool temp = *pBool;

                    if (ImGui::Checkbox(getPropertyLabel(label).c_str(), &temp))
                    {
                        if (storeProperty<bool>(pBool, temp, _object, _prop, context))
                            changed = true;
                    }
                    drawPropertyLabel(_prop->getDisplayName(), context);
                };
                break;

                case IProperty::Type::EnumU8:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnum<u8>(_object, _prop, context);
                    break;

                case IProperty::Type::EnumU16:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnum<u16>(_object, _prop, context);
                    break;

                case IProperty::Type::EnumU32:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnum<u32>(_object, _prop, context);
                    break;

                case IProperty::Type::EnumFlagsU8:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnumFlags<u8>(_object, _prop, context);
                    break;

                case IProperty::Type::EnumFlagsU16:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnumFlags<u16>(_object, _prop, context);
                    break;

                case IProperty::Type::EnumFlagsU32:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnumFlags<u32>(_object, _prop, context);
                    break;

                case IProperty::Type::EnumFlagsU64:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnumFlags<u64>(_object, _prop, context);
                    break;

                case IProperty::Type::Uint8:
                {
                    if (isEnumArray)
                    {
                        char temp[1024];
                        sprintf_s(temp, "%s (%u)", label.c_str(), _prop->getEnumCount());
                        if (ImGui::TreeNodeEx(temp, enumArrayTreeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->getEnumCount(); ++e)
                            {
                                const string enumLabel = ImGui::getObjectLabel(_prop->getEnumName(e), _prop + e);
                                u8 * pU8 = _prop->GetPropertyUint8(_object, e);
                                i32 temp = (u8)*pU8;
                                if (asBool(IProperty::Flags::HasRange & flags))
                                    changed |= ImGui::SliderInt(enumLabel.c_str(), &temp, max((int)0, (int)_prop->getRange().x), min((int)255, (int)_prop->getRange().y), "%d", imguiInputTextflags);
                                else
                                    changed |= ImGui::InputInt(enumLabel.c_str(), &temp, 1, 16, imguiInputTextflags);
                                if (changed)
                                    *pU8 = (u8)temp;
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        u8 * pU8 = _prop->GetPropertyUint8(_object);
                        i32 temp = (u8)*pU8;
                        bool edited = false;

                        if (asBool(IProperty::Flags::HasRange & flags))
                        {
                            if (ImGui::SliderInt(getPropertyLabel(label).c_str(), &temp, max((int)0, (int)_prop->getRange().x), min((int)255, (int)_prop->getRange().y), "%d", imguiInputTextflags))
                                edited = true;
                        }
                        else
                        {
                            if (ImGui::InputInt(getPropertyLabel(label).c_str(), &temp, 1, 16, imguiInputTextflags))
                                edited = true;
                        }

                        drawPropertyLabel(_prop->getDisplayName(), context);

                        if (edited)
                        {
                            if (storeProperty<u8>(pU8, temp, _object, _prop, context))
                                changed = true;
                        }
                    }
                };
                break;

                case IProperty::Type::Int8:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    i8 * pI8 = (i8 *)(uint_ptr(_object) + offset);
                    i32 temp = (u8)*pI8;
                    bool edited = false;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        edited = ImGui::SliderInt(getPropertyLabel(label).c_str(), &temp, max((int)-128, (int)_prop->getRange().x), min((int)127, (int)_prop->getRange().y), "%d", imguiInputTextflags);
                    else
                        edited = ImGui::InputInt(getPropertyLabel(label).c_str(), &temp, 1, 16, imguiInputTextflags);

                    drawPropertyLabel(_prop->getDisplayName(), context);

                    if (edited)
                    {
                        if (storeProperty<i8>(pI8, temp, _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Uint16:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    i16 * pU16 = (i16 *)(uint_ptr(_object) + offset);
                    i32 temp = (u16)*pU16;

                    bool edited = false;
                    if (asBool(IProperty::Flags::HasRange & flags))
                    {
                        if (ImGui::SliderInt(getPropertyLabel(label).c_str(), &temp, max((int)0, (int)_prop->getRange().x), min((int)65535, (int)_prop->getRange().y), "%d", imguiInputTextflags))
                            edited = true;
                    }
                    else
                    {
                        if (ImGui::InputInt(getPropertyLabel(label).c_str(), &temp, 1, 16, imguiInputTextflags))
                            edited = true;
                    }

                    drawPropertyLabel(_prop->getDisplayName(), context);

                    if (edited)
                    {
                        if (storeProperty<u16>((u16 *)pU16, (u16)temp, _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Int16:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    i16 * pI16 = (i16 *)(uint_ptr(_object) + offset);
                    i32 temp = (i16)*pI16;
                    bool edited = false;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        edited = ImGui::SliderInt(getPropertyLabel(label).c_str(), &temp, max((int)-32768, (int)_prop->getRange().x), min((int)32767, (int)_prop->getRange().y), "%d", imguiInputTextflags);
                    else
                        edited = ImGui::InputInt(getPropertyLabel(label).c_str(), &temp, 1, 16, imguiInputTextflags);

                    drawPropertyLabel(_prop->getDisplayName(), context);

                    if (edited)
                    {
                        if (storeProperty<i16>((i16 *)pI16, (i16)temp, _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Int32:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    i32 * pI32 = _prop->GetPropertyInt32(_object);
                    i32 temp = *pI32;
                    bool edited = false;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        edited = ImGui::SliderInt(getPropertyLabel(label).c_str(), &temp, max((int)-2147483648, (int)_prop->getRange().x), min(+2147483647, (int)_prop->getRange().y), "%d", imguiInputTextflags);
                    else
                        edited = ImGui::InputInt(getPropertyLabel(label).c_str(), &temp, 1, 16, imguiInputTextflags);

                    drawPropertyLabel(_prop->getDisplayName(), context);

                    if (edited)
                    {
                        if (storeProperty<i32>((i32 *)pI32, (i32)temp, _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Int64:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    i64 * pI64 = _prop->GetPropertyInt64(_object);
                    i64 temp = *pI64;
                    i64 minRange = (u64)_prop->getRange().x;
                    i64 maxRange = (u64)_prop->getRange().y;
                    bool edited = false;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        edited = ImGui::SliderScalar(getPropertyLabel(label).c_str(), ImGuiDataType_S64, &temp, &minRange, &maxRange, nullptr, imguiInputTextflags);
                    else
                        edited = ImGui::InputScalar(getPropertyLabel(label).c_str(), ImGuiDataType_S64, &temp, nullptr, nullptr, nullptr, imguiInputTextflags);

                    drawPropertyLabel(_prop->getDisplayName(), context);

                    if (edited)
                    {
                        if (storeProperty(pI64, temp, _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Uint32:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    u32 * pU32 = _prop->GetPropertyUint32(_object);
                    u32 temp = *pU32;

                    const u32 smallStep = 1;
                    const u32 bigStep = 100;
                    u32 minRange = (u32)max(0, _prop->getRange().x);
                    u32 maxRange = (u32)_prop->getRange().y;
                    bool edited = false;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        edited = ImGui::SliderScalar(getPropertyLabel(label).c_str(), ImGuiDataType_U32, &temp, &minRange, &maxRange, hexa ? "%08X" : "%d", imguiNumberTextInputFlag);
                    else
                        edited = ImGui::InputScalar(getPropertyLabel(label).c_str(), ImGuiDataType_U32, &temp, &smallStep, &bigStep, hexa ? "%08X" : "%d", imguiNumberTextInputFlag);

                    drawPropertyLabel(_prop->getDisplayName(), context);

                    if (edited)
                    {
                        if (storeProperty<u32>(pU32, temp, _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Uint64:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    u64 * pU64 = _prop->GetPropertyUint64(_object);
                    u64 temp = *pU64;
                    u64 minRange = (u64)_prop->getRange().x;
                    u64 maxRange = (u64)_prop->getRange().y;
                    bool edited = false;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        edited = ImGui::SliderScalar(getPropertyLabel(label).c_str(), ImGuiDataType_U64, &temp, &minRange, &maxRange, nullptr, imguiInputTextflags);
                    else
                        edited = ImGui::InputScalar(getPropertyLabel(label).c_str(), ImGuiDataType_U64, &temp, nullptr, nullptr, nullptr, imguiInputTextflags);

                    drawPropertyLabel(_prop->getDisplayName(), context);

                    if (edited)
                    {
                        if (storeProperty(pU64, temp, _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Uint2:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    u32 * pU32 = (u32*)_prop->GetPropertyUintN(_object, 2);
                    
                    u32 temp[2];
                    for (uint i = 0; i < 2; ++i)
                        temp[i] = pU32[i];

                    u32 minRange = (u32)_prop->getRange().x;
                    u32 maxRange = (u32)_prop->getRange().y;

                    bool edited = false;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        edited = ImGui::SliderScalarN(getPropertyLabel(label).c_str(), ImGuiDataType_U32, &temp, 2, &minRange, &maxRange, "%u", imguiInputTextflags);
                    else
                        edited = ImGui::InputScalarN(getPropertyLabel(label).c_str(), ImGuiDataType_U32, &temp, 2, &minRange, &maxRange, "%u", imguiInputTextflags);

                    drawPropertyLabel(_prop->getDisplayName(), context);

                    if (edited)
                    {
                        if (storeProperty((uint2*)pU32, uint2(temp[0], temp[1]), _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Float:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    float * pFloat = _prop->GetPropertyFloat(_object);
                    float temp = *pFloat;
                    bool edited = false;

                    if (asBool(IProperty::Flags::HasRange & flags))
                    {
                        if (ImGui::SliderFloat(getPropertyLabel(label).c_str(), &temp, _prop->getRange().x, _prop->getRange().y, g_editFloatFormat))
                            edited = true;
                    }
                    else
                    {
                        if (ImGui::InputFloat(getPropertyLabel(label).c_str(), &temp, 0.1f, 1.0f, g_editFloatFormat, imguiInputTextflags))
                            edited = true;
                    }

                    drawPropertyLabel(_prop->getDisplayName(), context);

                    if (edited)
                    {
                        if (storeProperty(pFloat, temp, _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Float2:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    float * pFloat2 = (float *)_prop->GetPropertyFloat2(_object);

                    float temp[2];
                    for (uint i = 0; i < 2; ++i)
                        temp[i] = pFloat2[i];

                    bool edited = false;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        edited = ImGui::SliderFloat2(getPropertyLabel(label).c_str(), (float *)&temp, _prop->getRange().x, _prop->getRange().y, g_editFloatFormat);
                    else
                        edited = ImGui::InputFloat2(getPropertyLabel(label).c_str(), (float *)&temp, g_editFloatFormat, imguiInputTextflags);

                    drawPropertyLabel(displayName, context);

                    if (edited)
                    {
                        if (storeProperty((float2 *)pFloat2, float2(temp[0], temp[1]), _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Float3:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    float * pFloat3 = (float *)_prop->GetPropertyFloat3(_object);

                    float temp[3];
                    for (uint i = 0; i < 3; ++i)
                        temp[i] = pFloat3[i];

                    bool edited = false;

                    if (asBool(IProperty::Flags::Color & flags))
                    {
                        ImGuiColorEditFlags colorEditFlags = 0;

                        if (asBool(IProperty::Flags::HDR & flags))
                            colorEditFlags |= ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float;

                        if (ImGui::ColorEdit3(getPropertyLabel(label).c_str(), (float *) &temp, colorEditFlags))
                            edited = true;
                    }
                    else
                    {
                        if (ImGui::InputFloat3(getPropertyLabel(label).c_str(), (float *) &temp, g_editFloatFormat, imguiInputTextflags))
                            edited = true;
                    }

                    drawPropertyLabel(displayName, context);

                    if (edited)
                    {
                        if (storeProperty<float3>((float3*)pFloat3, float3(temp[0], temp[1], temp[2]), _object, _prop, context))
                            changed = true;
                    }
                };
                break;

                case IProperty::Type::Float4:
                {
                    float * pFloat4 = (float *)_prop->GetPropertyFloat4(_object);

                    if (isEnumArray)
                    {
                        char temp[1024];
                        sprintf_s(temp, "%s (%u)", label.c_str(), _prop->getEnumCount());
                        if (ImGui::TreeNodeEx(temp, enumArrayTreeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->getEnumCount(); ++e)
                            {
                                if (asBool(IProperty::Flags::Color & flags))
                                    changed |= ImGui::ColorEdit4(_prop->getEnumName(e), pFloat4 + e * 4);
                                else
                                    changed |= ImGui::InputFloat4(_prop->getEnumName(e), pFloat4 + e * 4, g_editFloatFormat, imguiInputTextflags);
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        float temp[4];
                        for (uint i = 0; i < 4; ++i)
                            temp[i] = pFloat4[i];

                        bool edited = false;

                        if (asBool(IProperty::Flags::Color & flags))
                        {
                            ImGuiColorEditFlags colorEditFlags = 0;

                            if (asBool(IProperty::Flags::HDR & flags))
                                colorEditFlags |= ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float;                              

                            if (ImGui::ColorEdit4(getPropertyLabel(label).c_str(), (float *)&temp, colorEditFlags))
                                edited = true;
                        }
                        else
                        {
                            if (ImGui::InputFloat4(getPropertyLabel(label).c_str(), (float *)&temp, g_editFloatFormat, imguiInputTextflags))
                                edited = true;
                        }

                        drawPropertyLabel(displayName, context);

                        if (edited)
                        {
                            if (storeProperty((core::float4*)pFloat4, float4(temp[0], temp[1], temp[2], temp[3]), _object, _prop, context))
                                changed = true;
                        }
                    }
                };
                break;

                case IProperty::Type::Float4x4:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayFloat4x4(_object, _prop, context); 
                }
                break;

                case IProperty::Type::String:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    bool selectPath = false; 

                    string * pString = _prop->GetPropertyString(_object);
                    char buffer[1024];

                    const bool isFolder = asBool(IProperty::Flags::IsFolder & flags);
                    const bool isFile = asBool(IProperty::Flags::IsFile & flags);

                    if (isFolder || isFile)
                    {
                        auto availableWidth = GetContentRegionAvail().x;
                        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth - style::button::SizeSmall.x);
                        sprintf_s(buffer, pString->c_str());
                        if (ImGui::InputText(getObjectLabel("", displayName, _object).c_str(), buffer, countof(buffer), imguiInputTextflags))
                            *pString = buffer;
                        ImGui::PopItemWidth();

                        ImGui::SameLine();
                        auto x = ImGui::GetCursorPosX();
                        ImGui::SetCursorPosX(x + style::button::SizeSmall.x);
                        ImGui::Text(_prop->getDisplayName());
                        auto x2 = ImGui::GetCursorPosX();
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(x - 4);

                        if (isFolder)
                        {
                            const string selectFolderString = "Select Folder";
                            if (ImGui::Button(style::icon::Folder, style::button::SizeSmall))
                            {
                                ImGui::SelectFolderDialog(selectFolderString);
                            }

                            if (ImGui::DisplayFileDialog(selectFolderString))
                            {
                                if (ImGui::IsFileDialogOK())
                                {
                                    const string newFolder = io::getRelativePath(ImGui::GetFileDialogSelectedPath());

                                    if (newFolder != *pString)
                                    {
                                        *pString = newFolder;
                                        changed = true;
                                    }
                                }

                                ImGui::CloseFileDialog();
                            }
                        }
                        else if (isFile)
                        {
                            const string selectFileString = "Select File";

                            if (ImGui::Button(style::icon::File, style::button::SizeSmall))
                            {
                                const auto defaultFolder = _prop->getDefaultFolder();

                                // TODO: other extensions deduced from default folder?
                                string ext = ".*";
                                if (strstr(defaultFolder, "World"))
                                    ext = ".world";

                                ImGui::OpenFileDialog(selectFileString, ext, defaultFolder);
                            }

                            if (ImGui::DisplayFileDialog(selectFileString))
                            {
                                if (ImGui::IsFileDialogOK())
                                {
                                    const string newFile = io::getRelativePath(ImGui::GetFileDialogSelectedFile());

                                    if (newFile != *pString)
                                    {
                                        *pString = newFile;
                                        changed = true;
                                    }                                    
                                }

                                ImGui::CloseFileDialog();
                            }
                        }
                    }
                    else
                    {
                        sprintf_s(buffer, pString->c_str());
                        if (ImGui::InputText(getPropertyLabel(label).c_str(), buffer, countof(buffer), imguiInputTextflags))
                        {
                            if (*pString != buffer)
                            {
                                if (context.isPrefabInstance && !context.isPrefabOverride)
                                {
                                    // Create if needed
                                    if (context.propOverride = context.prefab->CreateDynamicProperty(_object, _prop))
                                    {
                                        ((core::DynamicPropertyString *)context.propOverride)->SetValue(buffer);
                                        context.propOverride->Enable(true);
                                        if (context.optionalPropOverride)
                                            context.optionalPropOverride->Enable(true);
                                    }
                                }
                                else
                                {
                                    // Save in property
                                    *pString = buffer;
                                }

                                changed = true;
                            }
                        }

                        drawPropertyLabel(displayName, context);
                    }                    
                }
                break;

                case IProperty::Type::Callback:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    IProperty::Callback pFunc = _prop->GetPropertyCallback();

                    if (ImGui::Button(displayName))
                        pFunc(_object);
                }
                break;

                case IProperty::Type::ObjectVector:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    const uint sizeOf = _prop->getSizeOf();
                    const size_t count = _prop->GetPropertyObjectVectorCount(_object);
                    const byte * data = _prop->GetPropertyObjectVectorData(_object);

                    string treeNodeName = fmt::sprintf("%s[%u]", displayName, count); 

                    auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

                    if (ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
                    {
                        for (uint i = 0; i < count; ++i)
                        {
                            IObject * pObject = (IObject *)(data + sizeOf * i);
                            displayArrayObject(pObject, i, nullptr);
                        }
                        ImGui::TreePop();
                    }
                }
                break;

                case IProperty::Type::ObjectPtrVector:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    auto * vec = _prop->GetPropertyObjectPtrVector(_object);
                    const uint count = vec->count();

                    string treeNodeName = fmt::sprintf("%s[%u]", displayName, count);

                    auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

                    //if (count > 0)
                    {
                        if (!strcmp(displayName, "Components"))
                        {
                            auto dragAndDropInterline = [=](IComponent * component, style::draganddrop::Type type)
                            {
                                //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                                {
                                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - style::draganddrop::interlineSize.y - 1);

                                    string label = fmt::sprintf("###%s %s", asString(type), component->getName().c_str());
                                    ImGui::InvisibleButton(label.c_str(), style::draganddrop::interlineSize);

                                    // debug
                                    //{
                                    //    u32 color;
                                    //    if (type == style::draganddrop::Type::BeforeNode)
                                    //        color = 0x7F0000FF;
                                    //    else
                                    //        color = 0x7F00FF00;
                                    //
                                    //    ImDrawList * draw_list = ImGui::GetWindowDrawList();
                                    //    float x0 = ImGui::GetCursorScreenPos().x;
                                    //    float y0 = ImGui::GetCursorScreenPos().y - 4 - 3;
                                    //    float x1 = x0 + 1024;
                                    //    float y1 = y0 + 4;
                                    //    draw_list->AddRect(ImVec2(x0, y0), ImVec2(x1, y1), color);
                                    //}
                                }
                                //ImGui::PopStyleVar(1);
                                ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);

                                if (ImGui::BeginDragDropTarget())
                                {
                                    ImGuiDragDropFlags target_flags = 0;
                                    target_flags |= ImGuiDragDropFlags_AcceptBeforeDelivery;    // Don't wait until the delivery (release mouse button on a target) to do something
                                    target_flags |= ImGuiDragDropFlags_AcceptNoDrawDefaultRect; // Don't display the yellow rectangle
                                    if (const ImGuiPayload * payload = ImGui::AcceptDragDropPayload("Component", target_flags))
                                    {
                                        IComponent * from = *(IComponent **)payload->Data;
                                        IComponent * to = component;

                                        ImDrawList * draw_list = ImGui::GetWindowDrawList();
                                        float x0 = ImGui::GetCursorScreenPos().x;
                                        float y0 = ImGui::GetCursorScreenPos().y - style::draganddrop::interlineSize.y / 2;
                                        float x1 = x0 + 1024;
                                        float y1 = y0 + style::draganddrop::interlineSize.y / 2;

                                        const auto interlineColor = ImGui::GetStyleColorVec4(ImGuiCol_SeparatorActive);
                                        u32 intColor = GetColorU32(interlineColor);
                                        draw_list->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), intColor);

                                        if (payload->Delivery)
                                        {
                                            if (from != to)
                                            {
                                                VG_INFO("[Inspector] Drag and drop component \"%s\" %s component \"%s\"", from->getName().c_str(), asString(type).c_str(), to->getName().c_str());

                                                IGameObject * toParent = dynamic_cast<IGameObject *>(to->getParent());
                                                IGameObject * fromParent = dynamic_cast<IGameObject *>(from->getParent());
                                                IGameObject * parent = nullptr;
                                                if (fromParent == toParent)
                                                    parent = fromParent;

                                                if (parent)
                                                {
                                                    VG_SAFE_INCREASE_REFCOUNT(from);
                                                    parent->RemoveComponent(from);
                                                    uint index = parent->GetComponentIndex(to);

                                                    if (style::draganddrop::Type::BeforeNode == type)
                                                        parent->AddComponent(from, index);
                                                    else if (style::draganddrop::Type::AfterNode == type)
                                                        parent->AddComponent(from, index + 1);

                                                    VG_SAFE_RELEASE(from);
                                                }
                                            }
                                        }
                                    }

                                    ImGui::EndDragDropTarget();
                                }
                            };

                            auto availableWidth = ImGui::GetContentRegionAvail().x;

                            for (uint i = 0; i < count; ++i)
                            {
                                IComponent * pComponent = (IComponent *)(*vec)[i];

                                if (i == 0)
                                    dragAndDropInterline(pComponent, style::draganddrop::BeforeNode);
                                
                                auto * classDesc = pComponent->GetClassDesc();
                                string componentShortName = fmt::sprintf("%s %s", classDesc->GetIcon(), classDesc->GetClassDisplayName()); // pComponent->GetClassName();

                                // Remove "Component" at the end of class name if present
                                {
                                    auto nPos = componentShortName.find("Component");
                                    if (-1 != nPos)
                                        componentShortName.erase(nPos);
                                }

                                // Remove "Behaviour" at the end of class name if present
                                {
                                    auto nPos = componentShortName.find("Behaviour");
                                    if (-1 != nPos)
                                        componentShortName.erase(nPos);
                                }

                                const bool debugInspector = EditorOptions::get()->IsDebugInspector();
                                if (debugInspector)
                                    componentShortName += fmt::sprintf(" (0x%016X)", (u64)pComponent);

                                ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

                                const bool isOpen = asBool(ObjectFlags::Opened & pComponent->GetObjectFlags());
                                ImGui::SetNextItemOpen(isOpen);

                                const bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", componentShortName, pComponent).c_str(), nullptr, ImGuiTreeNodeFlags_InvisibleArrow | ImGuiTreeNodeFlags_AllowItemOverlap);

                                // Save Open/Close state
                                if (open != isOpen)
                                    pComponent->SetObjectFlags(ObjectFlags::Opened, open);

                                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_AcceptNoDrawDefaultRect | ImGuiDragDropFlags_SourceNoHoldToOpenOthers))
                                {
                                    ImGui::SetDragDropPayload("Component", &pComponent, sizeof(IComponent *));
                                    ImGui::EndDragDropSource();
                                }

                                static ImGuiComponentInspectorMenu componentInspectorMenu;
                                componentInspectorMenu.Display(pComponent);

                                IGameObject * parent = dynamic_cast<IGameObject *>(pComponent->getParent());
                                bool isParentGameObjectEnabled = parent && asBool(InstanceFlags::Enabled & parent->GetInstanceFlags());
                                bool isComponentEnabled = asBool(ComponentFlags::Enabled & pComponent->GetComponentFlags());

                                ImGui::BeginDisabled(!isParentGameObjectEnabled);
                                CollapsingHeaderLabel(collapsingHeaderPos, componentShortName, isComponentEnabled);
                                ImGui::EndDisabled();

                                if (CollapsingHeaderCheckbox(collapsingHeaderPos, isComponentEnabled, pComponent,style::icon::Checked, style::icon::Unchecked, fmt::sprintf("%s %s component", isComponentEnabled ? "Disable" : "Enable", classDesc->GetClassDisplayName()).c_str()))
                                {
                                    pComponent->SetComponentFlags(ComponentFlags::Enabled, !isComponentEnabled);
                                    changed = true;
                                }

                                if (CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, pComponent, style::icon::Trashcan, fmt::sprintf("Remove %s component", classDesc->GetClassDisplayName())))
                                {
                                    componentInspectorMenu.removeComponent(pComponent);
                                    changed = true;
                                }

                                // Invisible selectable for interline
                                dragAndDropInterline(pComponent, style::draganddrop::AfterNode);                        

                                if (open)
                                    displayArrayObject(pComponent, i, nullptr);
                            }
                        }
                        else
                        {
                            auto availableWidth = ImGui::GetContentRegionAvail().x;
                            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

                            ImGuiStyle & style = ImGui::GetStyle();

                            ImGui::PushStyleColor(ImGuiCol_Header, style.Colors[ImGuiCol_WindowBg]);
                            ImGui::PushStyleColor(ImGuiCol_HeaderActive, style.Colors[ImGuiCol_ButtonActive]);
                            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style.Colors[ImGuiCol_ButtonHovered]);
                            ImGui::PushStyleColor(ImGuiCol_Button, style.Colors[ImGuiCol_Header]);
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, style.Colors[ImGuiCol_HeaderActive]);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style.Colors[ImGuiCol_HeaderHovered]);
                            ImGui::PushID("CollapsingHeader");
                            bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel(treeNodeName, _object).c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);
                            ImGui::PopID();
                            ImGui::PopStyleColor(6);

                            bool add = false, remove = false;

                            const char * interfaceName = _prop->getInterface();
                            const auto * factory = Kernel::getFactory();
                            const vector<IClassDesc *> elemClassDescs = factory->getClassDescriptors();
                            vector<IClassDesc*> compatibleElemDescs;
                            for (auto & elemDesc : elemClassDescs)
                            {
                                if (factory->IsA(elemDesc->GetClassName(), interfaceName))
                                    compatibleElemDescs.push_back(elemDesc);
                            }

                            const IClassDesc * elemDesc = nullptr;

                            if (compatibleElemDescs.size() > 0)
                            {
                                //if (compatibleElemDescs.size() == 1)
                                {
                                    elemDesc = compatibleElemDescs[0];

                                    ImGui::BeginDisabled(!elemDesc);
                                    if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Plus, fmt::sprintf("Add %s", elemDesc->GetClassName()), 0))
                                        add = true;
                                    ImGui::EndDisabled();

                                    ImGui::BeginDisabled(vec->size() <= 0);
                                    if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Minus, fmt::sprintf("Remove %s", elemDesc->GetClassName()), 1))
                                        remove = true;
                                    ImGui::EndDisabled();
                                }
                            }

                            if (open) //ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
                            {
                                for (uint i = 0; i < count; ++i)
                                {
                                    IObject * pObject = (*vec)[i];
                                    displayArrayObject(pObject, i, nullptr);
                                }
                                //ImGui::TreePop();
                            }

                            if (add && nullptr != elemDesc)
                            {
                                auto * obj = factory->createObject(elemDesc->GetClassName(), fmt::sprintf("[%u]", vec->size()));
                                vec->push_back(obj);
                            }

                            if (remove && vec->size() > 0)
                            {
                                auto * elem = vec->back();
                                VG_SAFE_RELEASE(elem);
                                vec->pop_back();
                            }
                        }
                    }
                }
                break;

                case IProperty::Type::ObjectPtrDictionary:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    dictionary<IObject *> * dic = _prop->GetPropertyObjectPtrDictionary(_object);
                    const uint count = (uint)dic->size();

                    string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

                    auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

                    if (count > 0 && ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
                    {
                        uint index = 0;
                        for (auto it = dic->begin(); it != dic->end(); ++it)
                        {
                            IObject * pObject = (*it).second;
                            displayArrayObject(pObject, index, (*it).first.c_str());
                            index++;
                        }

                        ImGui::TreePop();
                    }
                }
                break;

                case IProperty::Type::Object:
                case IProperty::Type::ObjectPtr:
                {
                    bool ref = (type == IProperty::Type::ObjectPtr);
                    IObject * pObject = ref ? *_prop->GetPropertyObjectPtr(_object) : _prop->GetPropertyObject(_object);

                    string treeNodeName = displayName;

                    if (isEnumArray)
                    {
                        char label[1024];
                        sprintf_s(label, "%s (%u)", displayName, _prop->getEnumCount());
                        if (ImGui::TreeNodeEx(label, enumArrayTreeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->getEnumCount(); ++e)
                            {
                                pObject = ref ? *_prop->GetPropertyObjectPtr(_object, e) : _prop->GetPropertyObject(_object, e);

                                if (ImGui::TreeNodeEx(_prop->getEnumName(e), ImGuiTreeNodeFlags_DefaultOpen))
                                {
                                    if (nullptr != pObject)
                                        displayObject(pObject);

                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_Text));

                        auto treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;

                        if (nullptr != pObject)
                        {
                            auto classDesc = pObject->GetClassDesc();
                            VG_ASSERT(classDesc);
                            if (classDesc && asBool(IClassDesc::Flags::Component & classDesc->GetFlags()))
                                treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;
                        }

                        ImGui::BeginDisabled(pObject == nullptr);
                        {
                            bool needTreeNode = strcmp(_prop->getName(), "m_object") && ref && !flatten;
                            bool treenNodeOpen = !needTreeNode || ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags);

                            static int ObjectRightClicMenuIndex = -1;
                            bool needOpenPopup = false;

                            if (treenNodeOpen)
                            {
                                if (pObject)
                                    displayObject(pObject);

                                if (needTreeNode)
                                    ImGui::TreePop();
                            }
                        }
                        ImGui::EndDisabled();

                        ImGui::PopStyleColor();
                    }
                }
                break;

                case IProperty::Type::ResourceVector:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    const uint sizeOf = _prop->getSizeOf();
                    const size_t count = _prop->GetPropertyResourceVectorCount(_object);
                    const byte * data = _prop->GetPropertyResourceVectorData(_object);

                    string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

                    if (ImGui::CollapsingHeader(treeNodeName.c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        ImGui::Indent();
                        for (uint i = 0; i < count; ++i)
                        {
                            IResource * pResource = (IResource *)(data + sizeOf * i);

                            string name = displayName + (string)"[" + to_string(i) + (string)"]";
                            if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                            {
                                ImGui::Indent();
                                displayResource(pResource, _prop, 0, context);
                                ImGui::Unindent();
                                ImGui::TreePop();
                            }
                        }
                        ImGui::Unindent();
                    }
                }
                break;

                case IProperty::Type::Resource:
                case IProperty::Type::ResourcePtr:
                {
                    const bool ref = type == IProperty::Type::ResourcePtr;

                    if (isEnumArray)
                    {
                        char label[1024];
                        sprintf_s(label, "%s (%u)", displayName, _prop->getEnumCount());
                        if (ImGui::TreeNodeEx(label, enumArrayTreeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->getEnumCount(); ++e)
                            {
                                auto pResource = ref ? *_prop->GetPropertyResourcePtr(_object, e) : _prop->GetPropertyResource(_object, e);

                                if (ImGui::TreeNodeEx(_prop->getEnumName(e), /*ImGuiTreeNodeFlags_OpenOnArrow |*/ ImGuiTreeNodeFlags_DefaultOpen))
                                {
                                    if (nullptr != pResource)
                                        changed |= displayResource(pResource, _prop, e, context);

                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        IResource * pResource = ref ? *_prop->GetPropertyResourcePtr(_object) : _prop->GetPropertyResource(_object);
                        changed |= displayResource(pResource, _prop, 0, context);
                    }
                }
                break;
            }
        }
        //ImGui::EndDisabled();

        //ImGui::SameLine();
        //ImGui::Text(label.c_str());

        if (optionalChanged)
        {
            if (context.prefab && context.optionalPropOverride != nullptr)
            {
                auto & children = context.prefab->GetChildren();
                for (uint i = 0; i < children.size(); ++i)
                    context.prefab->OverrideGameObjectProperties(children[i], context.optionalPropOverride);
            }
            else
            {
                _object->OnPropertyChanged(context.optionalObject, *context.optionalProp);
            }
        }

        if (changed)
        {
            if (context.prefab && context.propOverride != nullptr)
            {
                auto & children = context.prefab->GetChildren();
                for (uint i = 0; i < children.size(); ++i)
                    context.prefab->OverrideGameObjectProperties(children[i], context.propOverride);
            }
            else
            {
                _object->OnPropertyChanged(_object, *_prop);
            }
        }

        if (optional)
        {
            if (_prop->getType() != IProperty::Type::LayoutElement)
            {
                ImGui::PopItemWidth();
                bool * b = context.optionalProp->GetPropertyBool(context.optionalObject);
                if (!*b)
                    ImGui::EndDisabled();
            }
        }

        //VG_SAFE_DELETE(previousValue);

        if (context.isPrefabInstance && context.canPrefabOverride && !context.readOnly)
        {
            const auto saveCurY = ImGui::GetCursorPosY();
            auto deltaY = max(saveCurY - cursorPosY, style::button::SizeSmall.y);

            ImGuiContext & g = *GImGui;
            auto * window = g.CurrentWindow;

            auto availableWidth2 = window->Size.x - window->WindowPadding.x - (window->ScrollbarY ? ImGui::GetStyle().ScrollbarSize : 0);
            availableWidth2 = max(availableWidth, availableWidth2);

            ImGui::SameLine();
            ImGui::SetCursorPosX(availableWidth2 - style::button::SizeSmall.x);
            ImGui::SetCursorPosY(cursorPosY);

            bool checked = context.isPrefabOverride && context.propOverride->IsEnable();
            //ImGui::BeginDisabled(context.);
            if (ImGui::Checkbox(getObjectLabel("", "Override", _prop).c_str(), &checked))
            {
                if (checked)
                    context.prefab->ToggleOverride(context.originalObject, context.originalProp, true);
                else
                    context.prefab->ToggleOverride(context.originalObject, context.originalProp, false);

                if (context.optionalPropOverride)
                    context.optionalPropOverride->Enable(checked);
            }
            //ImGui::EndDisabled();
            ImGui::SetCursorPosY(cursorPosY + deltaY);

            if (ImGui::IsItemHovered())
            {
                if (context.isPrefabOverride)
                    ImGui::SetTooltip("Property \"%s\" is overridden", _prop->getName());
                else if (context.canPrefabOverride)
                    ImGui::SetTooltip("Property \"%s\" can be overriden", _prop->getName());
                else
                    ImGui::SetTooltip("Property \"%s\" cannot be overriden", _prop->getName());
            }
        }

        //if (isPrefabInstance)
        //    ImGui::PopStyleColor();

        //if (readOnly /* || isPrefabOverride*/)
        //    ImGui::PopStyleColor();
    }

    //--------------------------------------------------------------------------------------
    void BrowseToFile(const core::string& _filename)
    {
        string path = _filename;
        string rootDir = io::getRootDirectory();
        if (path.compare(0, rootDir.length(), rootDir) != 0)
            path = rootDir + "/" + path;

        // Replace "/" with "\"
        std::replace(path.begin(), path.end(), '/', '\\');

        wstring fullpath = wstring_convert(path);

        PIDLIST_ABSOLUTE pidl;
        HRESULT hr = ::SHParseDisplayName(fullpath.c_str(), nullptr, &pidl, 0, nullptr);
        VG_ASSERT(SUCCEEDED(hr), "Failed to parse display name. Error code: 0x%X\n", hr);
        if (SUCCEEDED(hr)) 
        {
            hr = ::SHOpenFolderAndSelectItems(pidl, 0, nullptr, 0);
            VG_ASSERT(SUCCEEDED(hr), "Failed to open Explorer and select the file. Error code : 0x%X", hr);
            CoTaskMemFree(pidl);
        }
    }

    //--------------------------------------------------------------------------------------
    // Display Resource Object
    // Display Resource properties 1st, then path and referenced object properties 2nd
    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::displayResource(core::IResource * _resource, const core::IProperty * _prop, core::uint _index, Context & _context)
    {
        const char * className = _resource->GetClassName();
        const auto * factory = Kernel::getFactory();
        const auto * classDesc = factory->getClassDescriptor(className);

        // Display all properties of the resource component
        {
            auto availableWidth = GetContentRegionAvail().x;
            ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

            if (!ImGuiObjectHandler::display(_resource))
            {
                const char * classDisplayName = classDesc->GetClassDisplayName();

                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);
                    if (strcmp(prop->getName(), "m_object"))
                        ImGuiWindow::displayProperty(_resource, prop);
                }
            }
            ImGui::PopItemWidth();
        }

        bool changed = false;

        const string resPath = _resource->GetResourcePath();

        char buffer[1024];
        sprintf_s(buffer, resPath.c_str());
        string label =  (string)"###" + to_string(uint_ptr(_resource));

        const float buttonWidth = style::button::SizeSmall.x;

        auto availableWidth = GetContentRegionAvail().x;
        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth - buttonWidth);
        if (ImGui::InputText(label.c_str(), buffer, countof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
            _resource->SetResourcePath(buffer);
        ImGui::PopItemWidth();

        string newFileButtonName = getButtonLabel("New", _resource);
        string openFileButtonName = getButtonLabel("Open", _resource);
        
        string saveFileButtonName = getButtonLabel("Save", _resource);
        string saveAsFileButtonName = getButtonLabel("Save As", _resource);

        string editFile = getButtonLabel("Edit", _resource);
        string openFolder = getButtonLabel("Open Containing Folder", _resource);

        string clearFileButtonName = getButtonLabel("Remove", _resource);
        string reimportFileButtonName = getButtonLabel("Reimport", _resource);

        bool createNewFile = false;
        bool openExistingFile = false;
        bool saveAsFile = false;
        bool saveFile = false;

        ImGui::PushID(_resource);
        ImGui::SameLine();
        auto x = ImGui::GetCursorPosX();
        ImGui::SetCursorPosX(x + buttonWidth);

        drawPropertyLabel(_prop->getDisplayName(), _context);

        auto x2 = ImGui::GetCursorPosX();
        ImGui::SameLine();
        string buttonLabel = (string)style::icon::File;// +(string)" " + (string)_prop->getDisplayName();
     
        ImGui::SetCursorPosX(x-4);        

        if (_context.readOnly)
            ImGui::BeginDisabled();

        if (ImGui::Button(buttonLabel.c_str(), style::button::SizeSmall))
        {
            //openExistingFile = true;
        }
        //ImGui::SetCursorPosX(x+24);
        
        if (ImGui::BeginPopupContextItem(nullptr, ImGuiPopupFlags_MouseButtonLeft))
        {
            if (_resource->CanCreateFile())
            {
                if (ImGui::MenuItem(newFileButtonName.c_str()))
                    createNewFile = true;
            }

            if (ImGui::MenuItem(openFileButtonName.c_str()))
                openExistingFile = true;  

            ImGui::Separator();
            ImGui::BeginDisabled(resPath.empty());
            {
                if (ImGui::MenuItem(editFile.c_str()))
                {
                    string cmd = fmt::sprintf("start %s/%s", io::getRootDirectory(), resPath);
                    system(cmd.c_str());
                }

                if (ImGui::MenuItem(openFolder.c_str()))
                {
                    //string cmd = fmt::sprintf("Explorer.exe /select,\"%s/%s\"", io::getRootDirectory(), resPath);
                    //system(cmd.c_str());
                    string path = fmt::sprintf("%s/%s", io::getRootDirectory(), resPath);
                    BrowseToFile(path.c_str());
                }
            }
            ImGui::EndDisabled();

            ImGui::Separator();

            if (_resource->CanSaveFile())
            {
                ImGui::BeginDisabled(nullptr == _resource->getObject());
                {
                    if (ImGui::MenuItem(saveFileButtonName.c_str()))
                    {
                        if (_resource->SaveFile(resPath))
                            changed = true;
                    }

                    if (ImGui::MenuItem(saveAsFileButtonName.c_str()))
                        saveAsFile = true;
                }
                ImGui::EndDisabled();

                ImGui::Separator();
            }

            ImGui::BeginDisabled(resPath.empty());
            {
                if (ImGui::MenuItem(clearFileButtonName.c_str()))
                    _resource->ClearResourcePath();

                if (ImGui::MenuItem(reimportFileButtonName.c_str()))
                {
                    auto rm = getEngine()->GetResourceManager();
                    rm->Reimport(_resource);
                }
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        if (_context.readOnly)
            ImGui::EndDisabled();

        // build extension list
        string ext = getFileBrowserExt(_resource);

        if (createNewFile)
        {
            ImGui::OpenFileDialog(newFileButtonName, ext, getDefaultFolder(_resource));
        }
        else if (openExistingFile)
        {
            ImGui::OpenFileDialog(openFileButtonName, ext, getDefaultFolder(_resource));
        }
        else if (saveAsFile)
        {
            ImGui::SaveFileDialog(saveAsFileButtonName, ext, getDefaultFolder(_resource));
        }

        // Create new file
        if (ImGui::DisplayFileDialog(newFileButtonName))
        {
            if (ImGui::IsFileDialogOK())
            {
                const string newFilePath = io::addExtensionIfNotPresent(ImGui::GetFileDialogSelectedFile(), _resource->getExtensions());
                if (_resource->CreateFile(newFilePath))
                {
                    _resource->SetResourcePath(newFilePath);
                    changed = true;
                }
            }

            ImGui::CloseFileDialog();
        }

        // Open existing file
        if (ImGui::DisplayFileDialog(openFileButtonName))
        {
            if (ImGui::IsFileDialogOK())
            {
                const string newFilePath = ImGui::GetFileDialogSelectedFile();
                if (_resource->GetResourcePath() != newFilePath)
                {
                    _resource->SetResourcePath(newFilePath);
                    changed = true;
                }
            }

            ImGui::CloseFileDialog();
        }

        // Save existing file
        if (ImGui::DisplayFileDialog(saveAsFileButtonName))
        {
            if (ImGui::IsFileDialogOK())
            {
                const string newFilePath = io::addExtensionIfNotPresent(ImGui::GetFileDialogSelectedFile(), _resource->getExtensions());
                if (_resource->SaveFile(newFilePath))
                {
                    _resource->SetResourcePath(newFilePath);
                    changed = true;
                }
            }

            ImGui::CloseFileDialog();
        }

        // Display all properties of the resource object
        IObject * resourceObject = _resource->getObject();
        if (resourceObject)
        {
            bool anyVisibleProperty = false;
            auto * objectClassDesc = factory->getClassDescriptor(resourceObject->GetClassName());

            for (uint i = 0; i < objectClassDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = objectClassDesc->GetPropertyByIndex(i);

                if (isPropertyVisible(prop->getFlags()) && strcmp(prop->getName(), "m_object"))
                {
                    anyVisibleProperty = true;
                    break;
                }
            }

            if (!strcmp(_resource->GetClassName(), "PrefabResource"))
                anyVisibleProperty = false;

            if (anyVisibleProperty)
            {
                char label[256];
                sprintf(label, "%s", _prop->getDisplayName());
                if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGuiWindow::displayObject(resourceObject);
                    ImGui::TreePop();
                }
            }
        }
        ImGui::PopID();
        ImGui::PopItemWidth();

        return changed;
    }

    //--------------------------------------------------------------------------------------
    core::string ImGuiWindow::getDefaultFolder(const core::string & _resourceTypeName)
    {
        const char * folder = nullptr;

        if (!strcmp(_resourceTypeName.c_str(), "MeshResource"))
            folder = "Meshes";
        else if (!strcmp(_resourceTypeName.c_str(), "TextureResource"))
            folder = "Textures";
        else if (!strcmp(_resourceTypeName.c_str(), "MaterialResource"))
            folder = "Materials";
        else if (!strcmp(_resourceTypeName.c_str(), "AnimationResource"))
            folder = "Animations";
        else if (!strcmp(_resourceTypeName.c_str(), "SceneResource"))
            folder = "Scenes";
        else if (!strcmp(_resourceTypeName.c_str(), "WorldResource"))
            folder = "Worlds";
        else if (!strcmp(_resourceTypeName.c_str(), "Prefabs"))
            folder = "Prefabs";

        if (!folder)
        {
            VG_WARNING("[Editor] No default folder for Resource type \"%s\"", _resourceTypeName.c_str());
            return fmt::sprintf("%s/data", io::getRootDirectory());
        }

        return fmt::sprintf("%s/data/%s", io::getRootDirectory(), folder);
    }

    //--------------------------------------------------------------------------------------
    string ImGuiWindow::getDefaultFolder(const IResource * _resource)
    {
        return getDefaultFolder(_resource->GetClassName());
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::isItemClicked()
    {
        if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
            return true;
        else
            return false;
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::updateSelection(core::IObject * _object)
    {
        ISelection * selection = getSelection();
        if (isItemClicked() && dynamic_cast<core::IGameObject*>(_object))
        {
            const bool isSelected = selection->IsSelectedObject(_object);

            if (ImGui::IsKeyDown(ImGuiMod_Ctrl))
            {
                if (isSelected)
                    selection->Remove(_object);
                else
                    selection->Add(_object);
            }
            else
            {
                selection->Clear();

                if (isSelected)
                    selection->Remove(_object);
                else
                    selection->Add(_object);
            }
            return true;
        }
        return false;
    }    

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::underLine(const ImColor & _color)
    {
        u32 color = _color;
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        min.y = max.y;
        ImGui::GetWindowDrawList()->AddLine(min, max, color, 1.0f);
    }

    //--------------------------------------------------------------------------------------
    // Add web link. Use '_text' as URL if no URL provided.
    //--------------------------------------------------------------------------------------
    void ImGuiWindow::textURL(const char * _text, const char * _url)
    {
        if (nullptr == _url)
            _url = _text;

        auto pos = ImGui::GetCursorPos();

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram ]);
        ImGui::Text(_text);
        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered())
        {
            ImGui::SameLine();
            ImGui::SetCursorPos(pos);
            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_PlotHistogramHovered]);
            ImGui::Text(_text);
            ImGui::PopStyleColor();

            underLine(ImGui::GetStyle().Colors[ImGuiCol_PlotHistogramHovered ]);

            if (ImGui::IsMouseClicked(0))
            {
                char tmp[1024];
                sprintf(tmp, "start %s", _url);
                system(tmp);
            }
            ImGui::SetTooltip(_url);
        }
        else
        {
            underLine(ImGui::GetStyle().Colors[ImGuiCol_PlotHistogram]);
        }
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::displayFloat4x4(core::IObject * _object, const core::IProperty * _prop, Context & _context)
    {
        bool changed = false;

        const auto displayName = _prop->getDisplayName();
        float4x4 * pFloat4x4 = _prop->GetPropertyFloat4x4(_object);
        float * pFloat = (float *)pFloat4x4;

        float temp[16];
        for (uint i = 0; i < countof(temp); ++i)
            temp[i] = pFloat[i];

        if (ImGui::TreeNode(getObjectLabel(displayName, _context.originalProp).c_str()))
        {
            bool edited = false;

            edited |= ImGui::InputFloat4(getPropertyLabel("I").c_str(), (float *)&temp[0], g_editFloatFormat, ImGuiInputTextFlags_EnterReturnsTrue);
            drawPropertyLabel("I", _context);

            edited |= ImGui::InputFloat4(getPropertyLabel("J").c_str(), (float *)&temp[4], g_editFloatFormat, ImGuiInputTextFlags_EnterReturnsTrue);
            drawPropertyLabel("J", _context);

            edited |= ImGui::InputFloat4(getPropertyLabel("K").c_str(), (float *)&temp[8], g_editFloatFormat, ImGuiInputTextFlags_EnterReturnsTrue);
            drawPropertyLabel("K", _context);

            edited |= ImGui::InputFloat4(getPropertyLabel("T").c_str(), (float *)&temp[12], g_editFloatFormat, ImGuiInputTextFlags_EnterReturnsTrue);
            drawPropertyLabel("T", _context);

            if (edited)
            {
                if (storeProperty(pFloat4x4, float4x4(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7], temp[8], temp[9], temp[10], temp[11], temp[12], temp[13], temp[14], temp[15]), _object, _prop, _context))
                    changed = true;
            }

            ImGui::TreePop();
        }

        return changed;
    }

}