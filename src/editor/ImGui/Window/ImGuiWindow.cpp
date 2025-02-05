#include "editor/Precomp.h"
#include "core/IObject.h"
#include "core/Math/Math.h"
#include "core/File/File.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IResourceMeta.h"
#include "core/IWorld.h"
#include "core/IBaseScene.h"
#include "core/IGameObject.h"
#include "core/IComponent.h"
#include "core/ISelection.h"
#include "core/UndoRedo/UndoRedo.h"
#include "core/IResourceManager.h"
#include "core/Misc/BitMask/BitMask.h"
#include "core/Misc/AABB/AABB.h"
#include "core/Object/DynamicProperties/DynamicProperties.h"
#include "core/Object/ObjectHandle.h"
#include "core/Types/Traits.h"
#include "renderer/IRenderer.h"
#include "engine/IEngine.h"
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
#include "editor/ImGui/Menu/ObjectHandle/ObjectHandleMenu.h"
#include "editor/ImGui/ObjectHandler/ImGuiObjectHandler.h"
#include "editor/ImGui/Menu/Inspector/Component/ImGuiComponentInspectorMenu.h"

#include "editor/ImGui/Window/ImGuiObjectPropertyContext.hpp"
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
#include "editor/ImGui/Window/UndoRedo/ImGuiUndoRedo.hpp"

#include "shlobj_core.h"

namespace vg::editor
{
    const char * g_editFloatFormat = "%.3f";
    const char * g_editUIntFormat = "%u";
    const char * g_editIntFormat = "%i";

    //--------------------------------------------------------------------------------------
    renderer::IImGuiAdapter * ImGuiWindow::getImGuiAdapter()
    {
        return Editor::get()->getRenderer()->GetImGuiAdapter();
    }

    //--------------------------------------------------------------------------------------
    ImVec2 ImGuiWindow::getImGuiPreviewSize()
    {
        float width = ImGui::GetContentRegionAvail().x;

        // Anti-flickering hack
        if (ImGui::GetScrollMaxY() == 0.0f)
            width -= ImGui::GetStyle().ScrollbarSize;

        width -= style::label::PixelWidth;
        width -= style::button::SizeSmall.x;
        //width = min(width, 96.0f);

        return ImVec2(width, width);
    }

    //--------------------------------------------------------------------------------------
    core::string ImGuiWindow::getFileBrowserExt(const core::IResource * _resource)
    {
        VG_ASSERT(_resource);
        
        const auto & extensions = _resource->GetExtensions();
        string ext;

        if (extensions.size() == 0)
        {
            ext = "All files (*.*){.*}";
        }
        else
        {
            ext = fmt::sprintf("%s files (", _resource->GetClassDesc()->GetClassDisplayName());

            bool first = true;
            
            for (uint e = 0; e < extensions.size(); ++e)
            {
                if (!first)
                    ext += ",";
                ext += "*";
                ext += extensions[e];
                first = false;
            }

            ext += "){";

            first = true;
            const auto & extensions = _resource->GetExtensions();
            for (uint e = 0; e < extensions.size(); ++e)
            {
                if (!first)
                    ext += ",";
                ext += extensions[e];
                first = false;
            }

            ext += "}";
        }
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
        return fmt::sprintf("%s##%s_0x%016X", _baseName, _object->GetClassName(), (u64)_object);
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
    template <> struct TypeToDynamicPropertyTypeEnum<core::uint3> { using type = core::DynamicPropertyUInt3; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::uint4> { using type = core::DynamicPropertyUInt4; };

    template <> struct TypeToDynamicPropertyTypeEnum<core::int2> { using type = core::DynamicPropertyInt2; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::int3> { using type = core::DynamicPropertyInt3; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::int4> { using type = core::DynamicPropertyInt4; };

    template <> struct TypeToDynamicPropertyTypeEnum<core::i8> { using type = core::DynamicPropertyI8; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::i16> { using type = core::DynamicPropertyI16; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::i32> { using type = core::DynamicPropertyI32; };
    template <> struct TypeToDynamicPropertyTypeEnum<core::i64> { using type = core::DynamicPropertyI64; };

    template <> struct TypeToDynamicPropertyTypeEnum<string> { using type = core::DynamicPropertyString; };
    template <> struct TypeToDynamicPropertyTypeEnum<ObjectHandle> { using type = core::DynamicPropertyU32; };

    template <typename T> bool equals(T a, T b)                 {  return a == b; }

    template <> bool equals(core::uint2 a, core::uint2 b) { return all(a == b); }
    template <> bool equals(core::uint3 a, core::uint3 b) { return all(a == b); }
    template <> bool equals(core::uint4 a, core::uint4 b) { return all(a == b); }

    template <> bool equals(core::int2 a, core::int2 b) { return all(a == b); }
    template <> bool equals(core::int3 a, core::int3 b) { return all(a == b); }
    template <> bool equals(core::int4 a, core::int4 b) { return all(a == b); }

    template <> bool equals(core::float2 a, core::float2 b)     { return all(a == b); }
    template <> bool equals(core::float3 a, core::float3 b)     { return all(a == b); }
    template <> bool equals(core::float4 a, core::float4 b)     { return all(a == b); }
    template <> bool equals(core::float4x4 a, core::float4x4 b) { return all(a == b); }

    //--------------------------------------------------------------------------------------
    template <typename T> bool createDynamicPropertyIfNeeded(T * _out, T _value, IObject *& _object, const IProperty *& _prop, PropertyContext & _propContext)
    {
        if ((!_propContext.m_readOnly))
        {
            if (_propContext.m_isPrefabInstance && !_propContext.m_isPrefabOverride && !asBool(_prop->GetFlags() & PropertyFlags::Transient))
            {
                if (_propContext.m_propOverride = _propContext.m_prefab->CreateDynamicProperty(_object, _prop))
                {
                    ((typename TypeToDynamicPropertyTypeEnum<T>::type *)_propContext.m_propOverride)->SetValue(_value);
                    _propContext.m_propOverride->Enable(true);

                    if (_propContext.m_optionalPropOverride)
                        _propContext.m_optionalPropOverride->Enable(true);

                    _object = _propContext.m_propOverride;
                    _prop = _propContext.m_propOverride->GetProperty();

                    return true;
                }
                else
                {
                    VG_ASSERT(false, "[Factory] Could not create DynamicProperty \"%s\" for class \"%s\"", _prop->GetName(), _object->GetClassName());

                    return false;
                }
            }
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool storeProperty(T * _out, T _value, IObject * _object, const IProperty * _prop, PropertyContext & _propContext, EditingState _editingState)
    {
        if (EditingState::BeginEdit == _editingState || EditingState::EndEdit == _editingState || (!_propContext.m_readOnly && !equals(*_out,_value)))
        {
            if (_propContext.m_isPrefabInstance && !_propContext.m_isPrefabOverride && _propContext.m_canPrefabOverride)
            {
                if (_propContext.m_propOverride = _propContext.m_prefab->CreateDynamicProperty(_object, _prop))
                {
                    ((typename TypeToDynamicPropertyTypeEnum<T>::type *)_propContext.m_propOverride)->SetValue(_value);
                    _propContext.m_propOverride->Enable(true);

                    if (_propContext.m_optionalPropOverride)
                        _propContext.m_optionalPropOverride->Enable(true);

                    _object = (IObject *)_propContext.m_propOverride;
                    //_prop = _propContext.m_propOverride->GetProperty();
                    //_out = (T*)((uint_ptr)_object + _prop->GetOffset());
                    _propContext.m_isPrefabOverride = true;

                    //return true;
                } 
                else
                {
                    VG_ASSERT(false, "[Factory] Could not create DynamicProperty \"%s\" for class \"%s\"", _prop->GetName(), _object->GetClassName());

                    return false;
                }
            }

            #if VG_ENABLE_UNDO_REDO
            auto * undoRedoManager = Kernel::getUndoRedoManager();

            if (EditingState::BeginEdit == _editingState)
            {
             
            }
            #endif

            if (_propContext.m_isPrefabOverride)
            {
                VG_ASSERT(_propContext.m_originalObject != _object);
                *_out = _value;
            }
            else
            {
                VG_ASSERT(_propContext.m_originalObject == _object);
                _object->SetPropertyValue(*_prop, _out, &_value);
            }

            #if VG_ENABLE_UNDO_REDO
            if (!_propContext.m_readOnly)
            {
                if (EditingState::EndEdit == _editingState)
                {
                    // Finalize Undo/Redo entry after editing
                    VG_ASSERT(undoRedoManager->HasCurrentlyEditedEntry());
                    undoRedoManager->AfterChange();
                }
            }
            #endif

            return true;
        }
        return false;
    }  

    template <typename T> struct ImGuiDataTypeInfo;
    template <> struct ImGuiDataTypeInfo<u8> { static const ImGuiDataType_ type = ImGuiDataType_U8; };
    template <> struct ImGuiDataTypeInfo<u16> { static const ImGuiDataType_ type = ImGuiDataType_U16; };
    template <> struct ImGuiDataTypeInfo<u32> { static const ImGuiDataType_ type = ImGuiDataType_U32; };
    template <> struct ImGuiDataTypeInfo<u64> { static const ImGuiDataType_ type = ImGuiDataType_U64; };

    template <> struct ImGuiDataTypeInfo<i8> { static const ImGuiDataType_ type = ImGuiDataType_S8; };
    template <> struct ImGuiDataTypeInfo<i16> { static const ImGuiDataType_ type = ImGuiDataType_S16; };
    template <> struct ImGuiDataTypeInfo<i32> { static const ImGuiDataType_ type = ImGuiDataType_S32; };
    template <> struct ImGuiDataTypeInfo<i64> { static const ImGuiDataType_ type = ImGuiDataType_S64; };

    template <> struct ImGuiDataTypeInfo<float> { static const ImGuiDataType_ type = ImGuiDataType_Float; };

    template <typename T> EditingState undoRedoBeforeEdit(bool & edited, PropertyContext & _propContext, IObject * _object, const IProperty * _prop, typename vectorTraits<T>::type * temp,  typename vectorTraits<T>::type * _ptr, InteractionType _interactionType, bool _itemActive = false, bool _itemAfterEdit = false)
    {
        constexpr auto count = vectorTraits<T>::count;
        using S = typename vectorTraits<T>::type;

        bool dynPropertyJustCreated = false;
        if (edited)
        {
            // Create dynamic property (if needed) early so as not to fuck up the undo/redo logic
            S initVal[count];
            for (int i = 0; i < count; ++i)
                initVal[i] = _ptr[i];

            dynPropertyJustCreated = createDynamicPropertyIfNeeded((T *)_ptr, vectorTraits<T>::makeVector(initVal), _object, _prop, _propContext);

            if (dynPropertyJustCreated && InteractionType::Continuous == _interactionType)
            {
                for (int i = 0; i < count; ++i)
                    temp[i] = initVal[i];
            }
        }

        EditingState editingState = EditingState::Unknown;

        // After ImGui control & before Undo/Redo action
        #if VG_ENABLE_UNDO_REDO
        if (!dynPropertyJustCreated || InteractionType::Single == _interactionType)
        {
            UndoRedoTarget undoRedoTarget(_object, _prop);
            auto * undoRedoManager = Kernel::getUndoRedoManager();

            if (edited && !_propContext.m_readOnly)
            {
                bool recordUndoRedoBeginValue = false;
                if (InteractionType::Continuous == _interactionType)
                {
                    // For slider-like edits we backup value on begin change and after last change
                    if (edited & (_itemActive || ImGui::IsItemActive()))
                        recordUndoRedoBeginValue = true;

                }
                else
                {
                    // For checkbox-like edits (one shot) we create the undoRedo 'before' value immediatly
                    recordUndoRedoBeginValue = true;
                }

                if (recordUndoRedoBeginValue)
                {
                    if (undoRedoManager->GetCurrentUndoRedoTarget() != undoRedoTarget)
                    {
                        //VG_INFO("[Undo/Redo] Begin editing Property \"%s\" (0x%016X) from Object \"%s\" (0x%016X)", _prop->GetName(), _prop, _object->getName().c_str(), _object);
                        editingState = EditingState::BeginEdit;

                        // Backup value before editing
                        VG_ASSERT(false == undoRedoManager->HasCurrentlyEditedEntry());
                        undoRedoManager->BeforeChange(new UndoRedoPropertyEntry(_object, _prop, _propContext.m_originalObject, _propContext.m_prefab, _propContext.m_propOverride));
                        undoRedoManager->SetCurrentUndoRedoTarget(undoRedoTarget);
                    }
                    else
                    {
                        editingState = EditingState::Editing;
                    }
                }
            }
            
            if ( (_itemAfterEdit || ImGui::IsItemDeactivatedAfterEdit()) || (edited && InteractionType::Single == _interactionType))
            {
                //VG_INFO("[Undo/Redo] End editing Property \"%s\" (0x%016X) from Object \"%s\" (0x%016X)", _prop->GetName(), _prop, _object->getName().c_str(), _object);
                editingState = EditingState::EndEdit;
                undoRedoManager->ClearCurrentUndoRedoTarget();
                edited = true;
            }
        }
        #endif

        return editingState;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool editScalarProperty(PropertyContext & _propContext, const string & _label, IObject * _object, const IProperty * _prop, typename vectorTraits<T>::type * _ptr)
    {
        constexpr auto count = vectorTraits<T>::count;
        using S = typename vectorTraits<T>::type;

        S temp[count];
        for (int i = 0; i < count; ++i)
            temp[i] = _ptr[i];

        bool edited = false;

        const auto dragSpeed = scalarTraits<S>::is_integer ? ImGuiWindow::getDragSpeedInt(_propContext.m_originalObject, _prop) : ImGuiWindow::getDragSpeedFloat(_propContext.m_originalObject, _prop);
        const auto flags = _prop->GetFlags();

        auto editFormat = scalarTraits<S>::is_integer ? (scalarTraits<S>::is_signed ? g_editIntFormat : g_editUIntFormat) : g_editFloatFormat;

        bool uid = false;
        bool invalidUID = false;
        if (asBool(PropertyFlags::Hexadecimal & flags))
        {
            if (!strcmp(_prop->GetName(), "m_uid") || !strcmp(_prop->GetName(), "m_originalUID"))
            {
                uid = true;

                const auto * factory = Kernel::getFactory();
                if (!factory->FindByUID((UID)temp[0]))
                    invalidUID = true;

            }
            editFormat = "0x%0X";
        }

        VG_ASSERT(!asBool(PropertyFlags::Color & flags) || (count == 3 || count == 4));

        if (uid && invalidUID)
            ImGui::PushStyleColor(ImGuiCol_Text, ImGuiWindow::getImGuiAdapter()->GetErrorColor());

        if (_propContext.m_readOnly)
            ImGui::BeginDisabled(true);

        InteractionType interactionType = InteractionType::Continuous;

        if (asBool(PropertyFlags::Color & flags) && (count == 3 || count == 4))
        {
            ImGuiColorEditFlags colorEditFlags = 0;

            if (asBool(PropertyFlags::HDR & flags))
                colorEditFlags |= ImGuiColorEditFlags_HDR | ImGuiColorEditFlags_Float;

            switch (count)
            {
            case 3:
                edited = ImGui::ColorEdit3(ImGuiWindow::getPropertyLabel(_label).c_str(), (float *)&temp, colorEditFlags);
                break;

            case 4:
                edited = ImGui::ColorEdit4(ImGuiWindow::getPropertyLabel(_label).c_str(), (float *)&temp, colorEditFlags);
                break;
            }
        }
        else
        {
            if (asBool(PropertyFlags::HasRange & flags))
            {
                const S minRange = (S)_prop->GetRange(_propContext.m_originalObject).x;
                const S maxRange = (S)_prop->GetRange(_propContext.m_originalObject).y;
                edited = CustomDragScalarN(interactionType, ImGuiWindow::getPropertyLabel(_label).c_str(), ImGuiDataTypeInfo<S>::type, &temp, count, dragSpeed, &minRange, &maxRange, editFormat, ImGuiSliderFlags_NoRoundToFormat);
            }
            else
            {
                edited = CustomDragScalarN(interactionType, ImGuiWindow::getPropertyLabel(_label).c_str(), ImGuiDataTypeInfo<S>::type, &temp, count, dragSpeed, nullptr, nullptr, editFormat, ImGuiSliderFlags_NoRoundToFormat);
            }
        }

        if (_propContext.m_readOnly)
            edited = false;

        EditingState editingState = undoRedoBeforeEdit<T>(edited, _propContext, _object, _prop, (S*)&temp[0], _ptr, interactionType);

        if (_propContext.m_readOnly)
            ImGui::EndDisabled();

        string tooltip = _prop->GetDescription() ? (string)_prop->GetDescription() : "";

        if (invalidUID)
            tooltip += " (invalid)";

        ImGuiWindow::drawPropertyLabel(_propContext, _prop->GetDisplayName(), tooltip.c_str());

        if (invalidUID)
            ImGui::PopStyleColor();

        if (edited)
        {
            if (asBool(PropertyFlags::EulerAngle & flags))
            {
                for (uint i = 0; i < count; ++i)
                {
                    S & val = temp[i];
                    while (val < -180)
                        val += 180;
                    while (val > 180)
                        val -= 180;
                }
            }

            if (storeProperty((T *)_ptr, vectorTraits<T>::makeVector(temp), _object, _prop, _propContext, editingState))
                return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> struct EnumPair
    {
        typename scalarTraits<T>::larger_type value;
        core::string name;
    };

    //--------------------------------------------------------------------------------------
    std::string camelCaseToSpaces(const std::string & _camelCaseString) 
    {
        std::string result;
        for (size_t i = 0; i < _camelCaseString.size(); ++i) 
        {
            if (i > 0 && ((std::isupper(_camelCaseString[i]) && std::islower(_camelCaseString[i - 1])) || (std::isupper(_camelCaseString[i]) && i + 1 < _camelCaseString.size() && std::islower(_camelCaseString[i + 1])))) 
                result += ' ';

            result += _camelCaseString[i];
        }
        return result;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool editEnum_Recur(core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext, const vector<EnumPair<T>> & enumPairs, string _enumName, uint _index, T * _pEnum)
    {
        bool changed = false;

        if (_enumName.empty())
            return false;

        auto it = _enumName.find_first_of("_");
        if (it != string::npos)
        {
            string category = _enumName.substr(0, it);
            if (ImGui::BeginMenu(category.c_str()))
            {
                string name = _enumName.substr(it + 1);
                bool res = editEnum_Recur(_object, _prop, _propContext, enumPairs, name, _index, _pEnum);
                ImGui::EndMenu();
                return res;
            }
        }
        else
        {
            auto enumVal = (T)enumPairs[_index].value;
            const auto enumValueFlags = scalarTraits<T>::is_signed ? _prop->GetSignedEnumValueFlags(enumVal) : _prop->GetUnsignedEnumValueFlags(enumVal);
            const bool disabled = asBool(EnumValueFlags::Disabled & enumValueFlags);

            if (disabled)
                PushDisabledStyle(true);

            if (ImGui::Selectable(camelCaseToSpaces(_enumName).c_str()))
            {
                if (enumVal != *_pEnum && !_propContext.m_readOnly && !disabled)
                {
                    bool edited = true;
                    auto editingState = undoRedoBeforeEdit<T>(edited, _propContext, _object, _prop, (T *)&enumVal, _pEnum, InteractionType::Single);

                    if (storeProperty<T>((T *)_pEnum, enumVal, _object, _prop, _propContext, editingState))
                    {
                        *_pEnum = enumVal;
                        changed = true;
                    }
                }
            }

            if (disabled)
                PopDisabledStyle();
        }

        return changed;
    };

    //--------------------------------------------------------------------------------------
    template <typename T> core::string getEnumDisplayName(const core::IProperty * _prop, uint _index)
    {
        const string enumTypeName = _prop->GetEnumTypeName();
        const string enumName = _prop->GetEnumName(_index);
        const string pattern = enumTypeName + "_";
        auto it = enumName.find(pattern);
        if (it != string::npos)
        {
            const string shortName = enumName.substr(it + pattern.length());
            return shortName;
        }

        return enumName;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool ImGuiWindow::editEnum(core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext)
    {
        const auto displayName = _prop->GetDisplayName();
        const auto offset = _prop->GetOffset();
        const auto flags = _prop->GetFlags();

        const bool readonly = _propContext.m_readOnly || _prop->IsReadOnly(_propContext.m_originalObject);
        if (readonly)
            PushDisabledStyle(true);

        T * pEnum = (T *)(uint_ptr(_object) + offset);
        int enumVal = (int)*pEnum;

        const string enumTypeName = _prop->GetEnumTypeName();
        vector<EnumPair<T>> enumPairs;
        enumPairs.resize(_prop->GetEnumCount());
        for (uint e = 0; e < _prop->GetEnumCount(); ++e)
        {
            enumPairs[e].name = _prop->GetEnumName(e);

            if (enumPairs[e].name._Starts_with(enumTypeName + "_"))
                enumPairs[e].name = enumPairs[e].name.substr(enumTypeName.length()+1);

            enumPairs[e].value = scalarTraits<T>::is_signed ? _prop->GetSignedEnumValue(e) : _prop->GetUnsignedEnumValue(e);
        }

        if (asBool(PropertyFlags::AlphabeticalOrder & flags)) 
        {
            sort(enumPairs.begin(), enumPairs.end(), [](EnumPair<T> & a, EnumPair<T> & b)
            {
                // Always put "None" at the beginning of the list
                if (b.name == "None")
                    return false;
                else if (a.name == "None")
                    return true;
                else
                return (a.name < b.name);
            }
            );
        }

        string preview = "<Invalid>";
        for (uint e = 0; e < enumPairs.size(); ++e)
        {
            if (enumVal == enumPairs[e].value)
            {
                preview = getEnumDisplayName<T>(_prop, e);

                auto it = preview.find_last_of("_");
                if (string::npos != it && it != preview.length())
                    preview = camelCaseToSpaces(preview.substr(it+1));

                break;
            }
        }

        bool changed = false;

        string enumLabel = ImGui::getObjectLabel("", _prop->GetDisplayName(), _prop);
        if (ImGui::BeginCombo(enumLabel.c_str(), preview.c_str(), ImGuiComboFlags_HeightLarge))
        {
            for (uint e = 0; e < enumPairs.size(); ++e)
            {
                const string enumName = enumPairs[e].name;
                changed |= editEnum_Recur<T>(_object, _prop, _propContext, enumPairs, enumName, e, pEnum);
            }
            ImGui::EndCombo();
        }

        if (readonly)
            PopDisabledStyle();

        drawPropertyLabel(_propContext, _prop);
        
        return changed;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool ImGuiWindow::editEnumFlags(core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext)
    {
        const auto displayName = _prop->GetDisplayName();
        const auto offset = _prop->GetOffset();
        const auto flags = _prop->GetFlags();

        const bool readonly = _propContext.m_readOnly || _prop->IsReadOnly(_propContext.m_originalObject);
        if (readonly)
            PushDisabledStyle(true);

        T * pEnum = (T*)(uint_ptr(_object) + offset); 
        T enumVal = (T)*pEnum;

        bool edited = false, changed = false;
        string preview;

        bool first = true, found = false, allBitsSet = _prop->GetEnumCount() > 0;
        for (uint e = 0; e < _prop->GetEnumCount(); ++e)
        {
            const auto bit = scalarTraits<T>::is_signed ? _prop->GetSignedEnumValue(e) : _prop->GetUnsignedEnumValue(e);

            if (enumVal & bit)
            {
                string name = _prop->GetEnumName(e);
                std::replace(name.begin(), name.end(), '_', ' ');

                if (name[0] != 0)
                {
                    found = true;

                    if (first)
                        first = false;
                    else
                        preview += "|";

                    preview += camelCaseToSpaces(name);
                }
            }
            else
            {
                allBitsSet = false;
            }
        }

        //if (allBitsSet)
        //    preview = "All";
        //else if (!found)
        //    preview = "";

        string enumLabel = ImGui::getObjectLabel("", _prop->GetDisplayName(), _prop);
        if (ImGui::BeginCombo(getPropertyLabel(enumLabel).c_str(), preview.c_str(), ImGuiComboFlags_None))
        {
            for (uint e = 0; e < _prop->GetEnumCount(); ++e)
            {
                const auto bit = scalarTraits<T>::is_signed ? _prop->GetSignedEnumValue(e) : _prop->GetUnsignedEnumValue(e);

                bool value = (bit & enumVal) ? true : false;
                string name = _prop->GetEnumName(e);

                if (!name.empty())
                {
                    std::replace(name.begin(), name.end(), '_', ' ');

                    if (ImGui::Checkbox(camelCaseToSpaces(name).c_str(), &value))
                    {
                        if (!readonly)
                        {
                            if (value)
                                enumVal |= bit;
                            else
                                enumVal &= ~bit;
                        }
                        edited = true;
                    }
                }
            }
            ImGui::EndCombo();

            if (edited)
            {
                auto editingState = undoRedoBeforeEdit<T>(edited, _propContext, _object, _prop, (T *)&enumVal, pEnum, InteractionType::Single);

                if (storeProperty<T>((T *)pEnum, enumVal, _object, _prop, _propContext, editingState))
                {
                    *pEnum = enumVal;
                    changed = true;
                }
            }
        }

        drawPropertyLabel(_propContext, _prop);

        if (readonly)
            PopDisabledStyle();

        return changed;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool ImGuiWindow::editCurve(core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext)
    {
        const string enumLabel = ImGui::getObjectLabel(_prop->GetDisplayName(), _prop);
        FloatCurveData * pCurve = _prop->GetPropertyFloatCurveData(_object);

        ImGui::Text("TODO: editCurve");

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::displayObject(core::IObject * _object)
    {
        ObjectContext objectContext;
        return displayObject(_object, objectContext, nullptr);
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::displayProperty(core::IObject * _object, const core::IProperty * _prop)
    {
        ObjectContext objectContext;
        return displayProperty(_object, _prop, objectContext);
    }

    //--------------------------------------------------------------------------------------
    // Returns 'true' if any object property has changed
    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::displayObject(core::IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext)
    {
        const char * className = _object->GetClassName();

        const auto * factory = Kernel::getFactory();
        const auto * classDesc = factory->GetClassDescriptor(className, false);

        if (!classDesc)
            return false;

        // debug only
        if (EditorOptions::get()->IsDebugPropertyVisible())
            ImGui::Text("Parent = 0x%016X (%s)", _object->GetParent(), _object->GetClassName());

        if (_objectContext.m_treeNodes.size() > 0)
        {
            auto & nodeInfo = _objectContext.m_treeNodes[_objectContext.m_treeNodes.size() - 1];
            if (!nodeInfo.treeNodeOpen)
                return false;
        }
        auto availableWidth = GetContentRegionAvail().x;
        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

        // TODO: Custom Object edit
        ImGui::PushID(_object);
        bool changed = false;

        auto customDisplayHandler = ImGuiObjectHandler::Find(_object->GetClassName());
        if (nullptr != customDisplayHandler)
        {
            changed = customDisplayHandler->displayObject(_object, _objectContext, _propContext);
        }
        else
        {        
            for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->GetPropertyByIndex(i);
                changed |= ImGuiWindow::displayProperty(_object, prop, _objectContext);
            }
        }
        ImGui::PopID();

        ImGui::PopItemWidth();

        return changed;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::displayArrayObject(IObject * _object, core::uint _index, const char * _name)
    {
        string treeNodeName;

        if (_name)
            treeNodeName = "[" + string(_name) + "]";
        else if (_object)
            treeNodeName = fmt::sprintf("[%u] %s", _index, _object->GetName());
        else
            treeNodeName = "[" + to_string(_index) + "]";

        auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

        const bool isComponent = nullptr != _object && _object->GetClassDesc() && asBool(ClassDescFlags::Component & _object->GetClassDesc()->GetFlags());
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
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::isPropertyVisible(const core::IObject * _object, const core::IProperty * _prop)
    {
        const auto flags = _prop->GetFlags();
        const bool hidden = _prop->IsHidden(_object);
        const bool debug = asBool(PropertyFlags::Debug & flags);
        const bool showDebug = EditorOptions::get()->IsDebugPropertyVisible();

        return !hidden && (!debug || showDebug);
    }

    //--------------------------------------------------------------------------------------
    string ImGuiWindow::getPropertyLabel(const string & _label, core::uint _index)
    {
        return fmt::sprintf("##%s%u", _label, _index);
    }

    //--------------------------------------------------------------------------------------
    ImVec4 ImGuiWindow::getPropertyColor(const PropertyContext & _propContext)
    {
        auto * imGuiAdapter = getImGuiAdapter();

        if (asBool(PropertyFlags::Transient & _propContext.m_originalProp->GetFlags()))
            return imGuiAdapter->GetTransientPropertyColor();

        if (_propContext.m_isPrefabInstance) 
        {
            if (_propContext.m_isPrefabOverride)
                return imGuiAdapter->GetPrefabOverridePropertyColor();
            if (_propContext.m_readOnly)
                return ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
            else if (_propContext.m_canPrefabOverride)
                return imGuiAdapter->GetTextColor();
            else
                return ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
        }
        else
        {
            if (_propContext.m_readOnly)
                return ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
            else
                return imGuiAdapter->GetTextColor();
        }
    };

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::drawPropertyLabel(const PropertyContext & _propContext, const core::IProperty * _prop, core::uint _index)
    {
        drawPropertyLabel(_propContext, _prop->GetDisplayName(), _prop->GetDescription(), _index);
    }

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::drawPropertyLabel(const PropertyContext & _propContext, const char * _label, const char * _description, core::uint _index)
    {
        auto * imGuiAdapter = getImGuiAdapter();

        // ugly workaround
        auto x = ImGui::GetCursorPosX();
        ImGui::SameLine();
        if (x > 100)
            ImGui::SetCursorPosX(x);

        if (asBool(PropertyFlags::EnumArray & _propContext.m_originalProp->GetFlags()))
        {
            string name = _propContext.m_originalProp->GetEnumName(_index);
            std::replace(name.begin(), name.end(), '_', ' ');
            ImGui::Text(name.c_str());
        }
        else
        {
            ImGui::Text(_label);
        }

        drawPropertyTooltip(_propContext, _description, _index);
    };

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::drawPropertyTooltip(const PropertyContext & _propContext, const char * _description, core::uint _index)
    {
        if (ImGui::IsItemHovered())
        {
            if (BeginTooltipEx(ImGuiTooltipFlags_OverridePrevious, ImGuiWindowFlags_None))
            {
                auto * imGuiAdapter = getImGuiAdapter();

                PushDisabledStyle(false);
                ImGui::PushStyleColor(ImGuiCol_Text, imGuiAdapter->GetTextColor());
                ImGui::Text("(%s)%s::%s", asString(_propContext.m_originalProp->GetType()).c_str(), _propContext.m_originalProp->GetClassName(), _propContext.m_originalProp->GetName());

                imGuiAdapter->PushFontStyle(renderer::FontStyle::Italic);
                imGuiAdapter->PopFontStyle();

                if (_propContext.m_originalProp->GetType() != core::PropertyType::Callback)
                {
                    if (asBool(PropertyFlags::Transient & _propContext.m_originalProp->GetFlags()))
                    {
                        ImGui::TextColored(imGuiAdapter->GetTransientPropertyColor(), "Not saved");

                        if (_propContext.m_readOnly)
                        {
                            ImGui::SameLine();
                            ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), " Read-only");
                        }                            
                    }
                    else if (_propContext.m_isPrefabInstance)
                    {
                        if (_propContext.m_isPrefabOverride)
                            ImGui::TextColored(imGuiAdapter->GetPrefabOverridePropertyColor(), "Overriden");
                        else if (_propContext.m_readOnly)
                            ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "Read-only");
                        else if (_propContext.m_canPrefabOverride)
                        {
                            //ImGui::TextColored(imGuiAdapter->GetTextColor(), "This property can be overriden");
                        }
                        else
                            ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "Cannot override");
                    }
                    else
                    {
                        if (_propContext.m_readOnly)
                            ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled), "Read-only");
                    }
                }

                if (_description && '\0' != _description[0])
                {
                    imGuiAdapter->PushFontStyle(renderer::FontStyle::Italic);
                    ImGui::Text("%s", _description);
                    imGuiAdapter->PopFontStyle();
                }

                ImGui::PopStyleColor();
                PopDisabledStyle();
                EndTooltip();
            }
        }
    }

    //--------------------------------------------------------------------------------------
    float ImGuiWindow::getDragSpeedFloat(const core::IObject * _object, const IProperty * _prop)
    {
        const auto flags = _prop->GetFlags();

        if (asBool(PropertyFlags::EulerAngle & flags))
            return 5.0f;

        if (asBool(PropertyFlags::HasRange & flags))
        {
            const auto range = _prop->GetRange(_object);
            return (range.y - range.x) / 1000.0f;
        }
        else
            return 0.05f;
    }

    //--------------------------------------------------------------------------------------
    float ImGuiWindow::getDragSpeedInt(const core::IObject * _object, const IProperty * _prop)
    {
        //if (asBool(PropertyFlags::HasRange & _prop->getFlags()))
        //    return (_prop->getRange().y - _prop->getRange().x) / 1000.0f;
        //else
            return 1.0f;
    }

    //--------------------------------------------------------------------------------------
    ImGuiInputTextFlags getImGuiInputTextFlags(const PropertyContext & _propContext, const IProperty * _prop)
    {
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (_propContext.m_readOnly)
            flags |= ImGuiInputTextFlags_ReadOnly;
        return flags;
    }

    //--------------------------------------------------------------------------------------
    ImGuiInputTextFlags getImGuiInputNumberFlags(const PropertyContext & _propContext, const IProperty * _prop)
    {
        ImGuiInputTextFlags flags = getImGuiInputTextFlags(_propContext, _prop);

        if (asBool(PropertyFlags::Hexadecimal & _prop->GetFlags()))
            flags |= ImGuiInputTextFlags_CharsHexadecimal;

        return flags;
    }

    //--------------------------------------------------------------------------------------
    // Returns 'true' if property has changed
    // rem: In the case of optional properties, the function will also return 'true' if optional has changed
    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::displayProperty(core::IObject * _object, const IProperty * _prop, ObjectContext & _objectContext)
    {
        VG_ASSERT(nullptr != _prop);

        const ImGuiStyle & style = ImGui::GetStyle();

        const auto * factory = Kernel::getFactory();
        const auto cursorPosY = GetCursorPosY();
        const auto availableWidth = GetContentRegionAvail().x;
        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

        // TODO: Custom property edit
        //if (ImGuiPropertyHandler::display(_prop, _object))
        //    return; 

        PropertyContext propContext(_object, _prop);        

        const auto type = _prop->GetType();
        const auto name = _prop->GetName();
        const auto displayName = _prop->GetDisplayName();
        const auto label = ImGui::getObjectLabel(displayName, propContext.m_originalProp);
        const auto offset = _prop->GetOffset();
        const auto flags = _prop->GetFlags();

        if (_objectContext.m_treeNodes.size() > 0)
        {
            auto & nodeInfo = _objectContext.m_treeNodes[_objectContext.m_treeNodes.size() - 1];
            if (!nodeInfo.treeNodeOpen)
            {
                if (PropertyType::LayoutElement != type)
                    return false;
            }
        }

        if (!isPropertyVisible(propContext.m_originalObject, _prop))
        {
            // If hidden property is a group, hide everything til the group is closed
            if (type == PropertyType::LayoutElement)
            {
                auto layoutType = _prop->GetLayoutElementType();
                if (layoutType == PropertyLayoutElement::GroupBegin)
                    _objectContext.m_treeNodeHideLevel = (uint)_objectContext.m_treeNodes.size();
            }

            return false;
        }

        if (-1 != _objectContext.m_treeNodeHideLevel && _objectContext.m_treeNodeHideLevel <= _objectContext.m_treeNodes.size())
        {
            if (type != PropertyType::LayoutElement || _prop->GetLayoutElementType() != PropertyLayoutElement::GroupEnd)
                return false;
        }

        const IClassDesc * classDesc = propContext.m_originalObject->GetClassDesc();
        auto * previousProp = classDesc->GetPreviousProperty(propContext.m_originalProp->GetName());
        bool singleLine = false;
        // Render property next to the previous one
        if (asBool(PropertyFlags::SingleLine & flags))
        {
            singleLine = true;

            if ((previousProp && asBool(PropertyFlags::SingleLine & previousProp->GetFlags())))
                ImGui::SameLine();
        }
        const bool hexa = asBool(PropertyFlags::Hexadecimal & flags);

        bool optional = false, optionalChanged = false;

        propContext.m_optionalObject = nullptr;
        propContext.m_optionalProp = nullptr;
        propContext.m_optionalPropOverride = nullptr;

        if (asBool(PropertyFlags::Optional & flags))
        {
            // check previous property is bool
            
            propContext.m_optionalObject = propContext.m_originalObject;
            propContext.m_optionalProp = previousProp;

            if (propContext.m_isPrefabInstance)
            {
                // Use override if it already exists but do not create it yet
                if (propContext.m_optionalPropOverride = propContext.m_prefab->GetDynamicProperty(propContext.m_originalObject, previousProp))
                {
                    propContext.m_optionalObject = (IObject *)propContext.m_optionalPropOverride;
                    propContext.m_optionalProp = propContext.m_optionalPropOverride->GetProperty();
                }
            }

            if (propContext.m_optionalProp)
            {
                VG_ASSERT(asBool(PropertyFlags::Hidden & propContext.m_optionalProp->GetFlags()) || _prop->GetType() == PropertyType::LayoutElement, "[Factory] Property used for optional variable \"%s\" should be %s", _prop->GetName(), asString(PropertyFlags::Hidden).c_str());
                
                if (propContext.m_optionalProp->GetType() == PropertyType::Bool)
                {
                    bool * b = propContext.m_optionalProp->GetPropertyBool(propContext.m_optionalObject);
                    bool temp = *b;

                    if (_prop->GetType() != PropertyType::LayoutElement)
                    {
                        if (ImGui::Checkbox(ImGui::getObjectLabel("", propContext.m_optionalProp).c_str(), &temp))
                        {
                            optionalChanged = true;

                            if (propContext.m_isPrefabInstance )
                            {
                                // Create prop override for bool 
                                const IClassDesc * classDesc = propContext.m_originalObject->GetClassDesc();
                                IProperty * originalOptionalProp = classDesc->GetPreviousProperty(propContext.m_originalProp->GetName());
                            
                                // Create if needed
                                if (propContext.m_optionalPropOverride = propContext.m_prefab->CreateDynamicProperty(propContext.m_originalObject, originalOptionalProp))
                                {
                                    propContext.m_optionalObject = (IObject *)propContext.m_optionalPropOverride;
                                    propContext.m_optionalProp = propContext.m_optionalPropOverride->GetProperty();
                            
                                    ((core::DynamicPropertyBool *)propContext.m_optionalPropOverride)->SetValue(temp);
                                    propContext.m_optionalPropOverride->Enable(true);
                            
                                    if (propContext.m_propOverride)
                                        propContext.m_propOverride->Enable(true);
                                }
                            }

                            *b = temp;
                        }
                        ImGui::SameLine();

                        auto availableWidth = GetContentRegionAvail().x;
                        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

                        optional = true;

                        if (!*b)
                            ImGui::PushDisabledStyle(true);
                    }
                }                
            }
        }

        bool changed = false;

        propContext.m_readOnly = _prop->IsReadOnly(propContext.m_originalObject);

        if (propContext.m_isPrefabInstance && !propContext.m_isPrefabOverride)
        {
            if (!propContext.m_canPrefabOverride)
            {
                if (!asBool(PropertyFlags::Transient & flags))
                    propContext.m_readOnly = true;
            }
        }

        ImGuiInputTextFlags imguiInputTextflags = getImGuiInputTextFlags(propContext, _prop);

        const bool flatten = asBool(PropertyFlags::Flatten & flags);
        const bool isEnumArray = asBool(PropertyFlags::EnumArray & flags);
        const auto enumArrayTreeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;

        auto * imGuiAdapter = getImGuiAdapter();

        ImGui::PushStyleColor(ImGuiCol_Text, getPropertyColor(propContext));

        //ImGui::BeginDisabled(readOnly);
        {
            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case PropertyType::LayoutElement:
                {
                    auto type = _prop->GetLayoutElementType();
                    switch (type)
                    {
                        default:
                            VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                            break;

                        case PropertyLayoutElement::Separator:
                            ImGui::SeparatorText(_prop->GetDisplayName());
                            break;

                        case PropertyLayoutElement::GroupBegin:
                        {
                            if (_objectContext.m_treeNodes.size() == 0 || _objectContext.m_treeNodes.back().treeNodeOpen)
                            {
                                auto & newInfo = _objectContext.m_treeNodes.emplace_back();

                                bool * toggle = nullptr;
                                bool previous;
                                if (asBool(PropertyFlags::Optional & flags))
                                {
                                    toggle = _prop->GetPropertyBool(_object);
                                    previous = *toggle;
                                }

                                newInfo.treeNodeOpen = ImGui::PersistentCollapsingHeader(_prop->GetDisplayName(), _object, _prop, toggle);
                                newInfo.treeNodeIsCollapsingHeader = true;

                                if (toggle)
                                {
                                    if (*toggle != previous)
                                        optionalChanged = true;

                                    if (!*toggle)
                                    {
                                        ImGui::PushDisabledStyle(true);
                                        newInfo.treeNodeContentDisabled = true;
                                    }
                                }
                                ImGui::Indent();
                            }
                        }
                        break;

                        case PropertyLayoutElement::GroupEnd:
                        {
                            if (-1 == _objectContext.m_treeNodeHideLevel || _objectContext.m_treeNodeHideLevel > _objectContext.m_treeNodes.size())
                            {
                                if (_objectContext.m_treeNodes.size() > 0)
                                {
                                    ImGui::Unindent();

                                    auto & nodeInfo = _objectContext.m_treeNodes[_objectContext.m_treeNodes.size() - 1];

                                    if (nodeInfo.treeNodeOpen)
                                    {
                                        if (!nodeInfo.treeNodeIsCollapsingHeader)
                                            ImGui::TreePop();

                                        ImGui::Spacing();
                                    }

                                    if (nodeInfo.treeNodeContentDisabled)
                                        ImGui::PopDisabledStyle();

                                    _objectContext.m_treeNodes.erase(_objectContext.m_treeNodes.begin() + _objectContext.m_treeNodes.size() - 1);
                                }
                            }
                            else
                            {
                                _objectContext.m_treeNodeHideLevel = -1;
                            }
                        }
                        break;
                    }
                }
                break;

                case PropertyType::BitMask:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    if (!isEnumArray)
                        changed |= editBitMask(_object, _prop, propContext);
                }
                break;

                case PropertyType::Bool:
                {
                    if (isEnumArray)
                    {
                        char temp[1024];
                        sprintf_s(temp, "%s (%u)", label.c_str(), _prop->GetEnumCount());
                        if (ImGui::TreeNodeEx(temp, enumArrayTreeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->GetEnumCount(); ++e)
                            {
                                const string enumLabel = ImGui::getObjectLabel(_prop->GetEnumName(e), _prop + e);
                                changed |= editBool(_object, _prop, propContext, enumLabel, availableWidth, singleLine, e);
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        changed |= editBool(_object, _prop, propContext, label, availableWidth, singleLine);
                    }
                };
                break;

                // TODO: custom Property control instead?
                case PropertyType::FloatCurveData:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editCurve<float>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumU8:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnum<u8>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumI8:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnum<i8>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumU16:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnum<u16>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumI16:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnum<i16>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumU32:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnum<u32>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumU64:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnum<u64>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumI32:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnum<i32>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumFlagsU8:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnumFlags<u8>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumFlagsU16:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnumFlags<u16>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumFlagsU32:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnumFlags<u32>(_object, _prop, propContext);
                    break;

                case PropertyType::EnumFlagsU64:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editEnumFlags<u64>(_object, _prop, propContext);
                    break;

                case PropertyType::Uint8:
                {
                    if (isEnumArray)
                    {
                        char temp[1024];
                        sprintf_s(temp, "%s (%u)", label.c_str(), _prop->GetEnumCount());
                        if (ImGui::TreeNodeEx(temp, enumArrayTreeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->GetEnumCount(); ++e)
                            {
                                const string enumLabel = ImGui::getObjectLabel(_prop->GetEnumName(e), _prop + e);
                                u8 * pU8 = _prop->GetPropertyUint8(_object, e);
                                i32 temp = (u8)*pU8;
                                if (asBool(PropertyFlags::HasRange & flags))
                                    changed |= ImGui::SliderInt(enumLabel.c_str(), &temp, max((int)0, (int)_prop->GetRange(_object).x), min((int)255, (int)_prop->GetRange(_object).y), "%d", imguiInputTextflags);
                                else
                                    changed |= ImGui::InputInt(enumLabel.c_str(), &temp, 1, 16, imguiInputTextflags & ~ImGuiInputTextFlags_EnterReturnsTrue);
                                if (changed)
                                    *pU8 = (u8)temp;
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        changed = editScalarProperty<u8>(propContext, label, _object, _prop, _prop->GetPropertyUint8(_object));
                    }
                };
                break;

                case PropertyType::Int8:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<i8>(propContext, label, _object, _prop, _prop->GetPropertyInt8(_object));
                };
                break;

                case PropertyType::Uint16:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<u16>(propContext, label, _object, _prop, _prop->GetPropertyUint16(_object));
                };
                break;

                case PropertyType::Int16:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<i16>(propContext, label, _object, _prop, _prop->GetPropertyInt16(_object));
                };
                break;

                case PropertyType::Int32:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<i32>(propContext, label, _object, _prop, _prop->GetPropertyInt32(_object));
                };
                break;

                case PropertyType::Int64:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<i64>(propContext, label, _object, _prop, _prop->GetPropertyInt64(_object));
                };
                break;

                case PropertyType::Int2:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<int2>(propContext, label, _object, _prop, _prop->GetPropertyIntN(_object, 2));
                };
                break;

                case PropertyType::Int3:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<int3>(propContext, label, _object, _prop, _prop->GetPropertyIntN(_object, 3));
                };
                break;

                case PropertyType::Int4:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<int4>(propContext, label, _object, _prop, _prop->GetPropertyIntN(_object, 4));
                };
                break;

                case PropertyType::Uint32:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<u32>(propContext, label, _object, _prop, _prop->GetPropertyUint32(_object));
                };
                break;

                case PropertyType::Uint64:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<u64>(propContext, label, _object, _prop, _prop->GetPropertyUint64(_object));
                };
                break;

                case PropertyType::Uint2:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<uint2>(propContext, label, _object, _prop, _prop->GetPropertyUintN(_object, 2));
                };
                break;

                case PropertyType::Uint3:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<uint3>(propContext, label, _object, _prop, _prop->GetPropertyUintN(_object, 3));
                };
                break;

                case PropertyType::Uint4:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<uint4>(propContext, label, _object, _prop, _prop->GetPropertyUintN(_object, 4));
                };
                break;

                case PropertyType::Float:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<float>(propContext, label, _object, _prop, _prop->GetPropertyFloat(_object));
                };
                break;

                case PropertyType::Float2:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<float2>(propContext, label, _object, _prop, _prop->GetPropertyFloatN(_object, 2));
                };
                break;

                case PropertyType::Float3:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed = editScalarProperty<float3>(propContext, label, _object, _prop, _prop->GetPropertyFloatN(_object, 3));
                };
                break;

                case PropertyType::Float4:
                {
                    float * pFloat4 = (float *)_prop->GetPropertyFloat4(_object);

                    if (isEnumArray)
                    {
                        char temp[1024];
                        sprintf_s(temp, "%s (%u)", label.c_str(), _prop->GetEnumCount());
                        if (ImGui::TreeNodeEx(temp, enumArrayTreeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->GetEnumCount(); ++e)
                            {
                                if (asBool(PropertyFlags::Color & flags))
                                    changed |= ImGui::ColorEdit4(_prop->GetEnumName(e).c_str(), pFloat4 + e * 4);
                                else
                                    changed |= ImGui::InputFloat4(_prop->GetEnumName(e).c_str(), pFloat4 + e * 4, g_editFloatFormat, imguiInputTextflags);
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        changed = editScalarProperty<float4>(propContext, label, _object, _prop, _prop->GetPropertyFloatN(_object, 3));
                    }
                };
                break;

                case PropertyType::Float4x4:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= editFloat4x4(_object, _prop, propContext); 
                }
                break;

                case PropertyType::ObjectHandle:
                {
                    editObjectHandle(_object, _prop, propContext);
                }
                break;

                case PropertyType::String:
                {
                    if (isEnumArray)
                    {
                        char label[1024];
                        sprintf_s(label, "%s (%u)", displayName, _prop->GetEnumCount());
                        if (ImGui::TreeNodeEx(label))
                        {
                            for (uint e = 0; e < _prop->GetEnumCount(); ++e)
                            {
                                const string enumLabel = ImGui::getObjectLabel(_prop->GetEnumName(e), _prop + e);
                                changed |= editString(_object, _prop, propContext, enumLabel, e);
                            }

                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        changed |= editString(_object, _prop, propContext, label);
                    }
                }
                break;

                case PropertyType::Callback:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    IProperty::ActionCallback pFunc = _prop->GetPropertyActionCallback();

                    float textWidth = ImGui::CalcTextSize(displayName).x;
                    float minWidth = style::button::SizeSmall.x;
                    float buttonWidth = std::max(textWidth + ImGui::GetStyle().FramePadding.x * 2, minWidth);

                    if (ImGui::Button(displayName, ImVec2(buttonWidth,0)))
                        pFunc(_object);

                    drawPropertyTooltip(propContext, _prop->GetDescription());
                }
                break;

                case PropertyType::ObjectVector:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    const uint sizeOf = _prop->GetSizeOf();
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

                case PropertyType::ObjectPtrVector:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    auto * vec = _prop->GetPropertyObjectPtrVector(_object);
                    const auto count = vec->size();

                    string treeNodeName = fmt::sprintf("%s[%u]", displayName, count);

                    auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

                    //if (count > 0)
                    {
                        if (!strcmp(displayName, "Components"))
                        {
                            auto dragAndDropInterline = [=](IComponent * component, DragAndDropType type)
                            {
                                //ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                                {
                                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - style::draganddrop::interlineSize.y - 1);

                                    string label = fmt::sprintf("###%s %u", asString(type), component->GetUID(false) ? (uint_ptr)component->GetUID() : (uint_ptr)component);
                                    ImGui::InvisibleButton(label.c_str(), style::draganddrop::interlineSize);

                                    // debug
                                    //{
                                    //    u32 color;
                                    //    if (type == DragAndDropType::BeforeNode)
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
                                                VG_INFO("[Inspector] Drag and drop component \"%s\" %s component \"%s\"", from->GetName().c_str(), asString(type).c_str(), to->GetName().c_str());

                                                IGameObject * toParent = dynamic_cast<IGameObject *>(to->GetParent());
                                                IGameObject * fromParent = dynamic_cast<IGameObject *>(from->GetParent());
                                                IGameObject * parent = nullptr;
                                                if (fromParent == toParent)
                                                    parent = fromParent;

                                                if (parent)
                                                {
                                                    VG_SAFE_INCREASE_REFCOUNT(from);
                                                    parent->RemoveComponent(from);
                                                    uint index = parent->GetComponentIndex(to);

                                                    if (DragAndDropType::BeforeNode == type)
                                                        parent->AddComponent(from, index);
                                                    else if (DragAndDropType::AfterNode == type)
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
                                    dragAndDropInterline(pComponent, DragAndDropType::BeforeNode);
                                
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

                                const bool open = ImGui::CollapsingHeader(ImGui::getObjectLabel("", componentShortName, pComponent).c_str(), nullptr, /*ImGuiTreeNodeFlags_InvisibleArrow |*/ ImGuiTreeNodeFlags_AllowItemOverlap);

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

                                IGameObject * parent = dynamic_cast<IGameObject *>(pComponent->GetParent());
                                bool isParentGameObjectEnabled = parent && asBool(InstanceFlags::Enabled & parent->GetInstanceFlags());
                                bool isComponentEnabled = asBool(ComponentFlags::Enabled & pComponent->GetComponentFlags());

                                ImGui::BeginDisabled(!isParentGameObjectEnabled);
                                CollapsingHeaderLabel(collapsingHeaderPos, componentShortName, isComponentEnabled);
                                ImGui::EndDisabled();

                                if (CollapsingHeaderCheckbox(collapsingHeaderPos, isComponentEnabled, pComponent,style::icon::Checked, style::icon::Unchecked, fmt::sprintf("%s %s component", isComponentEnabled ? "Disable" : "Enable", classDesc->GetClassDisplayName()).c_str()))
                                {
                                    pComponent->EnableComponentFlags(ComponentFlags::Enabled, !isComponentEnabled);
                                    changed = true;
                                }

                                if (CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, pComponent, style::icon::Trashcan, fmt::sprintf("Remove %s component", classDesc->GetClassDisplayName())))
                                {
                                    componentInspectorMenu.removeComponent(pComponent);
                                    changed = true;
                                }

                                // Invisible selectable for interline
                                dragAndDropInterline(pComponent, DragAndDropType::AfterNode);

                                if (open)
                                {
                                    ImGui::Indent();
                                    displayArrayObject(pComponent, i, nullptr);
                                    ImGui::Unindent();
                                }
                            }
                        }
                        else
                        {
                            auto availableWidth = ImGui::GetContentRegionAvail().x + GetCursorPosX() - style.FramePadding.x;
                            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

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
                            int removeAt = -1;

                            const char * interfaceName = _prop->GetInterface();
                            const auto * factory = Kernel::getFactory();
                            const vector<IClassDesc *> elemClassDescs = factory->GetClassDescriptors();
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

                                    //ImGui::BeginDisabled(vec->size() <= 0);
                                    //if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _object, style::icon::Minus, fmt::sprintf("Remove %s", elemDesc->GetClassName()), 1))
                                    //    remove = true;
                                    //ImGui::EndDisabled();
                                }
                            }

                            if (open) //ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
                            {
                                for (uint i = 0; i < count; ++i)
                                {
                                    collapsingHeaderPos.y = ImGui::GetCursorPos().y;
                                    availableWidth = ImGui::GetContentRegionAvail().x + GetCursorPosX() - style.FramePadding.x;

                                    IObject * pObject = (*vec)[i];
                                    displayArrayObject(pObject, i, nullptr);

                                    auto y = GetCursorPosY();

                                    ImGui::PushID(i);

                                    if (ImGui::CollapsingHeaderIconButton(collapsingHeaderPos - ImVec2(0,4), availableWidth - 1, _object, style::icon::Trashcan, fmt::sprintf("Remove element %u", i)))
                                        removeAt = i;

                                    ImGui::PopID();

                                    SetCursorPosY(y);
                                }
                                //ImGui::TreePop();
                            }

                            if (add && nullptr != elemDesc)
                            {
                                auto * obj = factory->CreateObject(elemDesc->GetClassName(), fmt::sprintf("[%u]", vec->size()));
                                vec->push_back(obj);
                            }

                            if (removeAt >= 0)
                            {
                                auto * elem = (*vec)[removeAt];
                                VG_SAFE_RELEASE(elem);
                                vec->erase(vec->begin() + removeAt);
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

                case PropertyType::ObjectPtrDictionary:
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

                case PropertyType::Object:
                case PropertyType::ObjectPtr:
                {
                    bool ref = (type == PropertyType::ObjectPtr);
                    IObject * pObject = ref ? *_prop->GetPropertyObjectPtr(_object) : _prop->GetPropertyObject(_object);

                    string treeNodeName = displayName;

                    if (isEnumArray)
                    {
                        char label[1024];
                        sprintf_s(label, "%s (%u)", displayName, _prop->GetEnumCount());
                        if (ImGui::TreeNodeEx(label, enumArrayTreeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->GetEnumCount(); ++e)
                            {
                                pObject = ref ? *_prop->GetPropertyObjectPtr(_object, e) : _prop->GetPropertyObject(_object, e);

                                if (ImGui::TreeNodeEx(_prop->GetEnumName(e).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
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
                        if (nullptr != pObject)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Text, imGuiAdapter->GetTextColor());
                            auto treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen;

                            auto classDesc = pObject->GetClassDesc();
                            if (classDesc && asBool(ClassDescFlags::Component & classDesc->GetFlags()))
                                treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;

                            bool needTreeNode = strcmp(_prop->GetName(), "m_object") && ref && !flatten;
                            bool treenNodeOpen = !needTreeNode || ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags);

                            static int ObjectRightClicMenuIndex = -1;
                            bool needOpenPopup = false;

                            if (treenNodeOpen)
                            {
                                if (pObject)
                                    displayObject(pObject, _objectContext, &propContext);

                                if (needTreeNode)
                                    ImGui::TreePop();
                            }
                            ImGui::PopStyleColor();
                        }                        
                    }
                }
                break;

                case PropertyType::ResourceVector:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    const uint sizeOf = _prop->GetSizeOf();
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
                                displayResource(pResource, _prop, 0, propContext);
                                ImGui::Unindent();
                                ImGui::TreePop();
                            }
                        }
                        ImGui::Unindent();
                    }
                }
                break;

                case PropertyType::Resource:
                case PropertyType::ResourcePtr:
                {
                    const bool ref = type == PropertyType::ResourcePtr;

                    if (isEnumArray)
                    {
                        char label[1024];
                        sprintf_s(label, "%s (%u)", displayName, _prop->GetEnumCount());
                        if (ImGui::TreeNodeEx(label, enumArrayTreeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->GetEnumCount(); ++e)
                            {
                                auto pResource = ref ? *_prop->GetPropertyResourcePtr(_object, e) : _prop->GetPropertyResource(_object, e);

                                if (ImGui::TreeNodeEx(_prop->GetEnumName(e).c_str(), /*ImGuiTreeNodeFlags_OpenOnArrow |*/ ImGuiTreeNodeFlags_DefaultOpen))
                                {
                                    if (nullptr != pResource)
                                        changed |= displayResource(pResource, _prop, e, propContext);

                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        IResource * pResource = ref ? *propContext.m_originalProp->GetPropertyResourcePtr(propContext.m_originalObject) : propContext.m_originalProp->GetPropertyResource(propContext.m_originalObject);
                        if (nullptr != pResource)
                            changed |= displayResource(pResource, _prop, 0, propContext);
                    }
                }
                break;
            }
        }
        ImGui::PopStyleColor();

        if (optionalChanged)
        {
            if (propContext.m_prefab && propContext.m_optionalPropOverride != nullptr)
            {
                auto & children = propContext.m_prefab->GetChildren();
                for (uint i = 0; i < children.size(); ++i)
                    propContext.m_prefab->OverrideGameObjectProperties(children[i], propContext.m_optionalPropOverride);
            }
            else
            {
                _object->OnPropertyChanged(propContext.m_optionalObject, *propContext.m_optionalProp);
            }
        }

        if (changed)
        {
            if (propContext.m_prefab && propContext.m_propOverride != nullptr)
            {
                auto & children = propContext.m_prefab->GetChildren();
                for (uint i = 0; i < children.size(); ++i)
                    propContext.m_prefab->OverrideGameObjectProperties(children[i], propContext.m_propOverride);
            }
            else
            {
                _object->OnPropertyChanged(_object, *_prop);
            }
        }

        if (optional)
        {
            if (_prop->GetType() != PropertyType::LayoutElement)
            {
                ImGui::PopItemWidth();
                bool * b = propContext.m_optionalProp->GetPropertyBool(propContext.m_optionalObject);
                if (!*b)
                    ImGui::PopDisabledStyle();
            }
        }

        if (propContext.m_isPrefabInstance && propContext.m_canPrefabOverride && !propContext.m_readOnly)
        {
            const auto saveCurY = ImGui::GetCursorPosY();
            auto deltaY = max(saveCurY - cursorPosY, style::button::SizeSmall.y);

            ImGuiContext & g = *GImGui;
            auto * window = g.CurrentWindow;

            auto availableWidth2 = window->Size.x - window->WindowPadding.x - (window->ScrollbarY ? ImGui::GetStyle().ScrollbarSize : 0);
            availableWidth2 = max(availableWidth, availableWidth2);

            if (ImGui::BeginPopupContextItem(getObjectLabel("Menu", "Override", _prop).c_str()))
            {
                ImGui::BeginDisabled(!propContext.m_canPrefabOverride);

                bool overriden = propContext.m_isPrefabOverride;
                bool notOverriden = !overriden;

                ImGui::MenuItem("Prefab override", nullptr, false, false);
                ImGui::Separator();

                if (ImGui::MenuItem("Use original value", nullptr, &notOverriden))
                    propContext.m_prefab->ToggleOverride(propContext.m_originalObject, propContext.m_originalProp, false);

                if (ImGui::MenuItem("Use override value", nullptr, &overriden))
                    propContext.m_prefab->ToggleOverride(propContext.m_originalObject, propContext.m_originalProp, true);

                ImGui::EndDisabled();

                ImGui::EndPopup();
            }
        }

        return changed | optionalChanged;
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
    bool ImGuiWindow::displayResource(core::IResource * _resource, const core::IProperty * _prop, core::uint _index, PropertyContext & _propContext)
    {
        const char * className = _resource->GetClassName();
        const auto * factory = Kernel::getFactory();
        const auto * classDesc = factory->GetClassDescriptor(className);

        ImGui::PushID(_resource);
        ImGui::PushID(_prop);

        ObjectContext objectContext;

        bool changed = false;

        // Display all properties of the resource component
        {
            auto availableWidth = GetContentRegionAvail().x;
            ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);
            {
                const char * classDisplayName = classDesc->GetClassDisplayName();

                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);
                    if (!strcmp(prop->GetName(), "m_object"))
                        continue;

                    changed |= ImGuiWindow::displayProperty(_resource, prop);
                }
            }
            ImGui::PopItemWidth();
        }

        string resPath = _resource->GetResourcePath();

        if (_propContext.m_propOverride && _propContext.m_propOverride->IsEnable())
            resPath = ((DynamicPropertyResource *)_propContext.m_propOverride)->m_value;

        char buffer[1024];
        sprintf_s(buffer, resPath.c_str());
        string label =  (string)"###" + to_string(uint_ptr(_resource));

        const float buttonWidth = style::button::SizeSmall.x;

        auto storePath = [=](const char * _resPath)
        {
            string relativePath = io::getRelativePath((string)_resPath);
            if (_propContext.m_propOverride)
            {
                ((DynamicPropertyResource *)_propContext.m_propOverride)->m_value = relativePath;
                _propContext.m_propOverride->Enable(true);
            }
            _resource->SetResourcePath(relativePath);
        };

        const ImGuiStyle & style = ImGui::GetStyle();

        auto availableWidth = GetContentRegionAvail().x;
        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth - buttonWidth - style.ItemSpacing.x);

        ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();

        const bool fileDoesNotExist = buffer[0] != '\0' && !io::exists(buffer);

        if (ImGui::InputText(label.c_str(), buffer, countof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
            storePath(buffer);

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

        auto rm = getEngine()->GetResourceManager();

        ImGui::SameLine();
        auto x = ImGui::GetCursorPosX();
        ImGui::SetCursorPosX(x + buttonWidth + style.ItemSpacing.x);

        drawPropertyLabel(_propContext, _prop);

        auto x2 = ImGui::GetCursorPosX();
        ImGui::SameLine();

        ImGui::SetCursorPosX(x - style.ItemSpacing.x*0);        

        if (_propContext.m_readOnly)
            ImGui::BeginDisabled();

        if (ImGui::Button(getObjectLabel((string)style::icon::File, _propContext.m_originalProp).c_str(), style::button::SizeSmall))
        {
            //openExistingFile = true;
        }
        //ImGui::SetCursorPosX(x+24);
        
        if (ImGui::BeginPopupContextItem(getObjectLabel("PopContextMenu", _propContext.m_originalProp).c_str(), ImGuiPopupFlags_MouseButtonLeft))
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
                    string path = fmt::sprintf("%s/%s", io::getRootDirectory(), resPath);
                    BrowseToFile(path.c_str());
                }
            }
            ImGui::EndDisabled();

            ImGui::Separator();

            if (_resource->CanSaveFile())
            {
                ImGui::BeginDisabled(nullptr == _resource->GetObject());
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
                    rm->Reimport(_resource);
            }
            ImGui::EndDisabled();

            ImGui::EndPopup();
        }

        if (_propContext.m_readOnly)
            ImGui::EndDisabled();

        availableWidth = ImGui::GetContentRegionAvail().x + GetCursorPosX() - style.FramePadding.x;

        if (fileDoesNotExist)
            ImGui::CollapsingHeaderIconButton(collapsingHeaderPos, availableWidth, _resource, style::icon::Warning, "File does not exist on disk", _index, &getImGuiAdapter()->GetErrorColor());

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
                const string newFilePath = io::addExtensionIfNotPresent(ImGui::GetFileDialogSelectedFile(), _resource->GetExtensions());
                if (_resource->CreateFile(newFilePath))
                {
                    storePath(newFilePath.c_str());
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
                    storePath(newFilePath.c_str());
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
                const string newFilePath = io::addExtensionIfNotPresent(ImGui::GetFileDialogSelectedFile(), _resource->GetExtensions());
                if (_resource->SaveFile(newFilePath))
                {
                    _resource->SetResourcePath(newFilePath);
                    changed = true;
                }
            }

            ImGui::CloseFileDialog();
        }

        ImGui::Indent();
        auto customDisplayHandler = ImGuiObjectHandler::Find(_resource->GetClassName());
        if (nullptr != customDisplayHandler)
        {
            changed = customDisplayHandler->displayObject(_resource, objectContext, &_propContext);
        }
        else
        {
            // Default display
            IObject * resourceObject = _resource->GetObject();
            IResourceMeta * resourceMeta = nullptr;

            if (resourceObject)
            {
                ImGuiWindow::displayObject(resourceObject);
                if (IResourceMeta * resourceMeta = rm->GetOrCreateResourceMeta(_resource))
                {
                    const string metaLabel = fmt::sprintf("Metadata###%u", _resource->GetUID());
                    if (ImGui::TreeNodeEx(metaLabel.c_str(), ImGuiTreeNodeFlags_None))
                    {
                        changed |= ImGuiWindow::displayObject(resourceMeta);
                        ImGui::TreePop();
                    }
                }
            }
        }
        ImGui::Unindent();

        if (_resource->GetObject())
            ImGui::Spacing();

        ImGui::PopItemWidth();

        ImGui::PopID(); // _resource;
        ImGui::PopID(); // _prop;

        return changed;
    }

    //--------------------------------------------------------------------------------------
    core::string ImGuiWindow::getDefaultFolder(const core::string & _resourceTypeName)
    {
        const char * folder = nullptr;

        // TODO: Resource types should be able to register their default folder
        if (!strcmp(_resourceTypeName.c_str(), "MeshResource"))
            folder = "Meshes";
        else if (!strcmp(_resourceTypeName.c_str(), "TextureResource"))
            folder = "Textures";
        else if (!strcmp(_resourceTypeName.c_str(), "MaterialResource"))
            folder = "Materials";
        else if (!strcmp(_resourceTypeName.c_str(), "SoundResource"))
            folder = "Sounds";
        else if (!strcmp(_resourceTypeName.c_str(), "AnimationResource"))
            folder = "Animations";
        else if (!strcmp(_resourceTypeName.c_str(), "SceneResource"))
            folder = "Scenes";
        else if (!strcmp(_resourceTypeName.c_str(), "WorldResource"))
            folder = "Worlds";
        else if (!strcmp(_resourceTypeName.c_str(), "LensResource"))
            folder = "Lens";
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
    bool ImGuiWindow::editBool(core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext, const core::string & label, float availableWidth, bool singleLine, core::uint _index)
    {
        bool changed = false;

        bool * pBool = _prop->GetPropertyBool(_object, _index);
        bool temp = *pBool;

        bool edited;

        if (singleLine)
        {
            // Several checkboxes in the same line
            edited = ImGui::Checkbox(getPropertyLabel(label).c_str(), &temp);
        }
        else
        {
            // The only checkbox in the line => align right
            auto cursorPosX = GetCursorPosX();
            const ImGuiStyle & style = ImGui::GetStyle();
            ImGui::SetCursorPosX(availableWidth - style::label::PixelWidth - ImGui::GetFrameHeight() + ImGui::GetStyle().ItemSpacing.y + cursorPosX - style.FramePadding.x - style.ItemSpacing.x);
            ImGui::BeginDisabled(_propContext.m_readOnly);
            edited = ImGui::Checkbox(getPropertyLabel(label).c_str(), &temp);
            ImGui::EndDisabled();
        }

        EditingState editingState = undoRedoBeforeEdit<bool>(edited, _propContext, _object, _prop, &temp, pBool, InteractionType::Single);

        if (edited)
        {
            if (storeProperty<bool>(pBool, temp, _object, _prop, _propContext, editingState))
                changed = true;
        }
        drawPropertyLabel(_propContext, _prop, _index);
        return changed;
    }

    void QuaternionToEuler(const quaternion & q, float & pitch, float & yaw, float & roll)
    {
        // Convert quaternion to Euler angles (pitch, yaw, roll)
        float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
        float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
        roll = atan2(sinr_cosp, cosr_cosp);

        float sinp = 2.0f * (q.w * q.y - q.z * q.x);
        if (fabs(sinp) >= 1)
            pitch = copysign(PI / 2, sinp); // Use 90 degrees if out of range
        else
            pitch = asin(sinp);

        float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
        float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
        yaw = atan2(siny_cosp, cosy_cosp);
    }

    quaternion EulerToQuaternion(float pitch, float yaw, float roll)
    {
        float cy = cos(yaw * 0.5f);
        float sy = sin(yaw * 0.5f);
        float cr = cos(roll * 0.5f);
        float sr = sin(roll * 0.5f);
        float cp = cos(pitch * 0.5f);
        float sp = sin(pitch * 0.5f);

        quaternion q;
        q.w = cy * cr * cp + sy * sr * sp;
        q.x = cy * sr * cp - sy * cr * sp;
        q.y = cy * cr * sp + sy * sr * cp;
        q.z = sy * cr * cp - cy * sr * sp;

        return q;
    }

    float ClampEulerAngle(float angle) 
    {
        if (angle > 180.0f)
            return angle - 360.0f;
        if (angle < -180.0f) 
            return angle + 360.0f;
        return angle;
    }

    //--------------------------------------------------------------------------------------
    float4x4 load_float4x4(const float (&temp)[16])
    {
        return float4x4(temp[0],  temp[1],  temp[2],  temp[3],
                        temp[4],  temp[5],  temp[6],  temp[7],
                        temp[8],  temp[9],  temp[10], temp[11],
                        temp[12], temp[13], temp[14], temp[15]);
    }

    //--------------------------------------------------------------------------------------
    void store_float4x4(const float4x4 & _mat, float(&temp)[16])
    {
        for (uint i = 0; i < countof(temp); ++i)
            temp[i] = ((float*)&_mat)[i];
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::editFloat4x4(core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext)
    {
        bool changed = false;

        const auto displayName = _prop->GetDisplayName();
        float4x4 * pFloat4x4 = _prop->GetPropertyFloat4x4(_object);
        float * pFloat = (float *)pFloat4x4;

        float temp[16];
        store_float4x4(*pFloat4x4, temp);

        const bool flatten = asBool(PropertyFlags::Flatten & _prop->GetFlags());

        const string LabelI = flatten ? fmt::sprintf("%s.I", displayName) : "I";
        const string LabelJ = flatten ? fmt::sprintf("%s.J", displayName) : "J";
        const string LabelK = flatten ? fmt::sprintf("%s.K", displayName) : "K";
        const string LabelT = flatten ? fmt::sprintf("%s.T", displayName) : "T";

        if (flatten || ImGui::TreeNode(getObjectLabel(displayName, _propContext.m_originalProp).c_str()))
        {
            bool edited = false;
            EditingState editingState = EditingState::Unknown;

            if (_propContext.m_readOnly)
                ImGui::BeginDisabled();

            bool itemActive = false, itemAfterEdit = false;
            InteractionType interactionType = InteractionType::Continuous;

            //if (useTRS)
            {
                const bool defaultTransform = !strcmp(displayName, "Transform");

                const string TLabel = flatten && !defaultTransform ? fmt::sprintf("%s.T", displayName) : "Translation";
                const string RLabel = flatten && !defaultTransform ? fmt::sprintf("%s.R", displayName) : "Rotation";
                const string SLabel = flatten && !defaultTransform ? fmt::sprintf("%s.S", displayName) : "Scale";

                float translation[3];
                float rotation[3];
                float scale[3];
                ImGuizmo::DecomposeMatrixToComponents((float *)&temp[0], (float *)&translation, (float *)&rotation, (float *)&scale);

                float prevTranslation[3] = { translation[0], translation[1], translation[2] };
                float prevRot[3] = { rotation[0], rotation[1], rotation[2] };
                float prevScale[3] = { scale[0], scale[1], scale[2] };

                edited |= CustomDragFloat3(interactionType, getPropertyLabel("T").c_str(), (float *)&translation, getDragSpeedFloat(_object, _prop) * 90.0f/8.0f, -style::range::maxFloat, style::range::maxFloat, g_editFloatFormat) && !_propContext.m_readOnly;
                itemActive = ImGui::IsItemActive() || InteractionType::Single == interactionType;
                itemAfterEdit = ImGui::IsItemDeactivatedAfterEdit() || InteractionType::Single == interactionType;
                drawPropertyLabel(_propContext, TLabel.c_str(), "Represents the translation part of the matrix");

                edited |= CustomDragFloat3(interactionType, getPropertyLabel("R").c_str(), (float *)&rotation, getDragSpeedFloat(_object, _prop) * 90.0f / 8.0f, -style::range::maxFloat, style::range::maxFloat, g_editFloatFormat) && !_propContext.m_readOnly;
                itemActive |= ImGui::IsItemActive() || InteractionType::Single == interactionType;
                itemAfterEdit |= ImGui::IsItemDeactivatedAfterEdit() || InteractionType::Single == interactionType;
                drawPropertyLabel(_propContext, RLabel.c_str(), "Represents the rotation part of the matrix");

                edited |= CustomDragFloat3(interactionType, getPropertyLabel("S").c_str(), (float *)&scale, getDragSpeedFloat(_object, _prop) * 90.0f / 8.0f, 0.01f, style::range::maxFloat, g_editFloatFormat) && !_propContext.m_readOnly;
                itemActive |= ImGui::IsItemActive() || InteractionType::Single == interactionType;
                itemAfterEdit |= ImGui::IsItemDeactivatedAfterEdit() || InteractionType::Single == interactionType;
                drawPropertyLabel(_propContext, SLabel.c_str(), "Represents the scale part of the matrix");

                if (edited)
                {
                    // Do not recompose matrix from TRS, but apply matrix operations instead
                    //ImGuizmo::RecomposeMatrixFromComponents((float *)&translation, (float *)&rotation, (float *)&scale, (float *)&temp[0]);
                    float4x4 m = load_float4x4(temp);

                    if (prevTranslation[0] != translation[0])
                    {
                        float delta = translation[0] - prevTranslation[0];
                        m[3].x += delta;
                    }
                    else if (prevTranslation[1] != translation[1])
                    {
                        float delta = translation[1] - prevTranslation[1];
                        m[3].y += delta;
                    }
                    else if (prevTranslation[2] != translation[2])
                    {
                        float delta = translation[2] - prevTranslation[2];
                        m[3].z += delta;
                    }

                    if (prevRot[0] != rotation[0])
                    {
                        float delta = rotation[0] - prevRot[0];
                        float4x4 mRot = float4x4::rotation_x(delta * PI / 180.0f);
                        m = mul(mRot, m);
                        
                    } else if (prevRot[1] != rotation[1])
                    {
                        float delta = rotation[1] - prevRot[1];
                        float4x4 mRot = float4x4::rotation_y(delta * PI / 180.0f);
                        m = mul(mRot, m);
                    }
                    else if (prevRot[2] != rotation[2])
                    {
                        float delta = rotation[2] - prevRot[2];
                        float4x4 mRot = float4x4::rotation_z(delta * PI / 180.0f);
                        m = mul(mRot, m);
                    }

                    if (prevScale[0] != scale[0])
                    {
                        float ratio = scale[0] / prevScale[0];
                        m[0].x *= ratio;
                        m[1].x *= ratio;
                        m[2].x *= ratio;
                        //m[3].x *= ratio;
                    }
                    else if (prevScale[1] != scale[1])
                    {
                        float ratio = scale[1] / prevScale[1];
                        m[0].y *= ratio;
                        m[1].y *= ratio;
                        m[2].y *= ratio;
                        //m[3].y *= ratio;
                    }
                    else if (prevScale[2] != scale[2])
                    {
                        float ratio = scale[2] / prevScale[2];
                        m[0].z *= ratio;
                        m[1].z *= ratio;
                        m[2].z *= ratio;
                        //m[3].z *= ratio;
                    }

                    store_float4x4(m, temp);
                }
            }

            if (EditingState::Unknown == editingState)
                editingState = undoRedoBeforeEdit<float4x4>(edited, _propContext, _object, _prop, (float *)&temp[0], pFloat, interactionType, itemActive, itemAfterEdit);

            if (ImGui::TreeNode(getObjectLabel("(float4x4) " + (string)displayName, _propContext.m_originalProp).c_str()))
            {
                ImGui::Spacing();
            
                edited |= ImGui::CustomDragFloat4(interactionType, getPropertyLabel("I").c_str(), (float *)&temp[0], getDragSpeedFloat(_object, _prop), -style::range::maxFloat, style::range::maxFloat, g_editFloatFormat) && !_propContext.m_readOnly;
                if (EditingState::Unknown == editingState || InteractionType::Single == interactionType)
                    editingState = undoRedoBeforeEdit<float4x4>(edited, _propContext, _object, _prop, (float *)&temp[0], pFloat, interactionType);
                drawPropertyLabel(_propContext, LabelI.c_str(), "Represents the x-axis in the transformed space");
            
                edited |= ImGui::CustomDragFloat4(interactionType, getPropertyLabel("J").c_str(), (float *)&temp[4], getDragSpeedFloat(_object, _prop), -style::range::maxFloat, style::range::maxFloat, g_editFloatFormat) && !_propContext.m_readOnly;
                if (EditingState::Unknown == editingState || InteractionType::Single == interactionType)
                    editingState = undoRedoBeforeEdit<float4x4>(edited, _propContext, _object, _prop, (float *)&temp[0], pFloat, interactionType);
                drawPropertyLabel(_propContext, LabelJ.c_str(), "Represents the y-axis in the transformed space");
            
                edited |= ImGui::CustomDragFloat4(interactionType, getPropertyLabel("K").c_str(), (float *)&temp[8], getDragSpeedFloat(_object, _prop), -style::range::maxFloat, style::range::maxFloat, g_editFloatFormat) && !_propContext.m_readOnly;
                if (EditingState::Unknown == editingState || InteractionType::Single == interactionType)
                    editingState = undoRedoBeforeEdit<float4x4>(edited, _propContext, _object, _prop, (float *)&temp[0], pFloat, interactionType);
                drawPropertyLabel(_propContext, LabelK.c_str(), "Represents the z-axis in the transformed space");
            
                edited |= ImGui::CustomDragFloat4(interactionType, getPropertyLabel("T").c_str(), (float *)&temp[12], getDragSpeedFloat(_object, _prop), -style::range::maxFloat, style::range::maxFloat, g_editFloatFormat) && !_propContext.m_readOnly;
                if (EditingState::Unknown == editingState || InteractionType::Single == interactionType)
                    editingState = undoRedoBeforeEdit<float4x4>(edited, _propContext, _object, _prop, (float *)&temp[0], pFloat, interactionType);
                drawPropertyLabel(_propContext, LabelT.c_str(), "Represents the translation component"); 
            
                ImGui::TreePop();
            }
            
            if (_propContext.m_readOnly)
                ImGui::EndDisabled();

            if (edited)
            {
                if (storeProperty(pFloat4x4, float4x4(temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7], temp[8], temp[9], temp[10], temp[11], temp[12], temp[13], temp[14], temp[15]), _object, _prop, _propContext, editingState))
                    changed = true;
            }

            if (!flatten)
                ImGui::TreePop();
        }

        return changed;
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::editObjectHandle(core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext)
    {
        bool changed = false;

        ObjectHandle * pObjHandle = _prop->GetPropertyObjectHandle(_object);

        bool edited = false;

        UID temp = 0;
        IObject * obj = nullptr;

        if (pObjHandle)
        {
            temp = pObjHandle->getUID();
            obj = pObjHandle->getObject();
        }

        char buffer[1024];
        if (EditorOptions::get()->IsDebugInspector())
            sprintf_s(buffer, "%s (0x%08X)", obj ? obj->GetName().c_str() : "(null)", temp); 
        else
            sprintf_s(buffer, "%s", obj ? obj->GetName().c_str() : "");

        const float buttonWidth = style::button::SizeSmall.x;

        auto availableWidth = GetContentRegionAvail().x;
        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth - buttonWidth);
        const auto label = ImGui::getObjectLabel(_prop->GetDisplayName(), _propContext.m_originalProp);
        ImGui::InputText(getPropertyLabel(label).c_str(), buffer, countof(buffer), getImGuiInputTextFlags(_propContext, _prop) | ImGuiInputTextFlags_ReadOnly);

        bool openPopup = false;
        if (IsItemHovered() && IsMouseDoubleClicked(ImGuiMouseButton_Left))
            openPopup = true;

        ImGui::PopItemWidth();


        ImGui::PushID(_prop);
        ImGui::SameLine();
        ImGui::Button(style::icon::GameObject, style::button::SizeSmall);
        ImGuiObjectHandleMenu * pickObjectHandlemenu = Editor::get()->getMenu<ImGuiObjectHandleMenu>();
        if (pickObjectHandlemenu->SelectUID(&temp, _propContext.m_gameobject, openPopup))
            edited = true;
        ImGui::PopID();

        EditingState editingState = undoRedoBeforeEdit<UID>(edited, _propContext, _object, _prop, (UID *)&temp, (UID *)pObjHandle->getUIDPtr(), InteractionType::Single);

        drawPropertyLabel(_propContext, _prop);

        if (edited)
        {
            if (storeProperty((UID *)pObjHandle->getUIDPtr(), (UID)temp, _object, _prop, _propContext, editingState))
                changed = true;
        }

        return changed;
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::editBitMask(core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext)
    {
        bool changed = false;
        
        BitMask * pBitMask = _prop->GetPropertyBitMask(_object);

        string enumLabel = ImGui::getObjectLabel(_prop->GetDisplayName(), _prop);
        string preview = pBitMask->toString();

        if (_propContext.m_readOnly)
            ImGui::PushDisabledStyle(true);

        if (ImGui::BeginCombo(enumLabel.c_str(), preview.c_str(), ImGuiComboFlags_HeightLarge))
        {
            const auto bitCount = pBitMask->getBitCount();
            const auto names = pBitMask->getNames();

            for (uint i = 0; i < bitCount; ++i)
            {
                bool value = pBitMask->getBitValue(i);
                string bitName;

                if (i < names.size())
                {
                    if (names[i].empty())
                        continue;

                    bitName = fmt::sprintf("[%u] %s", i, names[i]);
                }
                else
                {
                    bitName = fmt::sprintf("Bit %u", i);
                }

                if (ImGui::Checkbox(bitName.c_str(), &value))
                {
                    if (!_propContext.m_readOnly)
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

        if (_propContext.m_readOnly)
            ImGui::PopDisabledStyle();

        return changed;
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::editString(core::IObject * _object, const core::IProperty * _prop, PropertyContext & _propContext, const core::string & _label, uint _index)
    {
        bool changed = false;
        bool selectPath = false;

        string * pString = _prop->GetPropertyString(_object, _index);
        char buffer[1024];

        const auto flags = _prop->GetFlags();
        const bool isFolder = asBool(PropertyFlags::IsFolder & flags);
        const bool isFile = asBool(PropertyFlags::IsFile & flags);
        const auto displayName = _prop->GetDisplayName();

        if (isFolder || isFile)
        {
            auto availableWidth = GetContentRegionAvail().x;
            ImGui::PushItemWidth(availableWidth - style::label::PixelWidth - style::button::SizeSmall.x);
            sprintf_s(buffer, pString->c_str());
            if (ImGui::InputText(getPropertyLabel(displayName).c_str(), buffer, countof(buffer), getImGuiInputTextFlags(_propContext, _prop)))
                *pString = buffer;
            ImGui::PopItemWidth();

            ImGui::SameLine();
            auto x = ImGui::GetCursorPosX();
            ImGui::SetCursorPosX(x + style::button::SizeSmall.x);
            drawPropertyLabel(_propContext, _prop);
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
                    const auto defaultFolder = _prop->GetDefaultFolder();

                    // TODO: other extensions deduced from default folder?
                    string ext = ".*";
                    if (strstr(defaultFolder, "World"))
                        ext = "World files (*.world){.world}";

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
            bool edited = false;

            strcpy_s(buffer, pString->c_str());

            if (ImGui::InputText(getPropertyLabel(_label, _index).c_str(), buffer, countof(buffer), getImGuiInputTextFlags(_propContext, _prop)))
                edited = true;

            string bufferString = (string)buffer;

            EditingState editingState = undoRedoBeforeEdit<string>(edited, _propContext, _object, _prop, &bufferString, pString, InteractionType::Single);

            drawPropertyLabel(_propContext, _prop, _index);

            if (edited)
            {
                if (storeProperty(pString, bufferString, _object, _prop, _propContext, editingState))
                    changed = true;
            }
        }

        return changed;
    }
}