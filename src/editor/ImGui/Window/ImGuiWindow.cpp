#include "editor/Precomp.h"
#include "core/IObject.h"
#include "core/Math/Math.h"
#include "core/File/File.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IUniverse.h"
#include "core/IScene.h"
#include "core/IGameObject.h"
#include "core/IComponent.h"
#include "core/ISelection.h"

#include "renderer/IRenderer.h"

#include "engine/IEngine.h"
#include "engine/IResourceManager.h"

#include "editor/ImGui/ImGui.h"
#include "editor/Editor_Consts.h"
#include "editor/imgui/Extensions/imGuiExtensions.h"

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
#include "editor/ImGui/Window/DisplayOptions/ImGuiDisplayOptions.hpp"
#include "editor/ImGui/Window/About/ImGuiAbout.hpp"
#include "editor/ImGui/Window/Input/ImGuiInput.hpp"
#include "editor/ImGui/Window/Inspector/ImGuiInspector.hpp"
#include "editor/ImGui/Window/Shader/ImGuiShader.hpp"
#include "editor/ImGui/Window/Scene/ImGuiScene.hpp"
#include "editor/ImGui/Window/Resource/ImGuiResource.hpp"
#include "editor/ImGui/Window/View/ImGuiView.hpp"
#include "editor/ImGui/Window/View/EditorView/ImGuiEditorView.hpp"
#include "editor/ImGui/Window/View/GameView/ImGuiGameView.hpp"
#include "editor/ImGui/Window/Console/ImGuiConsole.hpp"

#include "editor/ImGui/ObjectHandler/ImGuiObjectHandler.h"
#include "editor/ImGui/Menu/Inspector/Component/ImGuiComponentInspectorMenu.h"

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    imgui_addons::ImGuiFileBrowser & ImGuiWindow::getFileBrowser()
    {
        static imgui_addons::ImGuiFileBrowser g_imguiFileBrower;
        return g_imguiFileBrower;
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
        m_icon(_icon),
        m_path(_path),
        m_name(_name),
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
    string ImGuiWindow::getPropertyLabel(const IProperty * _prop)
    {
        string name = _prop->getDisplayName();
        string className = _prop->getClassName();
        const auto width = style::label::TextLength;

        if (name.length() < width)
        {
            const auto spaces = width - name.length();
            name.append(spaces, ' ');
        }

        name += "##" + className;
        return name;
    }

    //--------------------------------------------------------------------------------------
    string ImGuiWindow::getButtonLabel(string _baseName, IObject * _object)
    {
        return _baseName + "##" + _object->getClassName() + "##" + to_string((u64)&_object); // TODO: Object GUID?
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool ImGuiWindow::displayEnum(core::IObject * _object, const core::IProperty * _prop)
    {
        const auto displayName = _prop->getDisplayName();
        const auto offset = _prop->getOffset();
        const auto flags = _prop->getFlags();

        const bool readonly = asBool(IProperty::Flags::ReadOnly & flags);

        T * pEnum = (T*)(uint_ptr(_object) + offset);
        int enumVal = (int)*pEnum;

        string preview = "<Invalid>";
        for (uint e = 0; e < _prop->getEnumCount(); ++e)
        {
            if (enumVal == _prop->getEnumValue(e))
            {
                preview = _prop->getEnumName(e);
                break;
            }
        }

        bool changed = false;
        if (ImGui::BeginCombo(getPropertyLabel(_prop).c_str(), preview.c_str(), ImGuiComboFlags_HeightLarge))
        {
            for (uint e = 0; e < _prop->getEnumCount(); ++e)
            {
                if (ImGui::Selectable(_prop->getEnumName(e)))
                {
                    if (!readonly)
                    {
                        *pEnum = e;
                        changed = true;
                    }
                }
            }
            ImGui::EndCombo();
        }
        
        return changed;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool ImGuiWindow::displayEnumFlags(core::IObject * _object, const core::IProperty * _prop)
    {
        const auto displayName = _prop->getDisplayName();
        const auto offset = _prop->getOffset();
        const auto flags = _prop->getFlags();

        const bool readonly = asBool(IProperty::Flags::ReadOnly & flags);

        T * pEnum = (T*)(uint_ptr(_object) + offset);
        int enumVal = (int)*pEnum;

        bool changed = false;
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

        if (ImGui::BeginCombo(getPropertyLabel(_prop).c_str(), preview.c_str(), ImGuiComboFlags_None))
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
                    changed = true;
                }
            }
            ImGui::EndCombo();

            if (changed)
                *pEnum = enumVal;
        }

        return changed;
    }

    //--------------------------------------------------------------------------------------
    void ImGuiWindow::displayObject(core::IObject * _object)
    {
        const char * className = _object->getClassName();

        const auto * factory = Kernel::getFactory();
        const auto * classDesc = factory->getClassDescriptor(className);

        if (!classDesc)
            return;

        auto availableWidth = GetContentRegionAvail().x;
        ImGui::PushItemWidth(availableWidth - style::label::PixelWidth);

        // TODO: Custom Object edit
        if (!ImGuiObjectHandler::display(_object))
        {
            const char * classDisplayName = classDesc->GetClassDisplayName();
        
            for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
            {
                const IProperty * prop = classDesc->GetPropertyByIndex(i);
                ImGuiWindow::displayProperty(_object, prop);
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
            treeNodeName = _object->getName();
        else
            treeNodeName = "[" + to_string(_index) + "]";

        auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

        const bool isComponent = nullptr != _object && _object->getClassDesc() && asBool(IClassDesc::Flags::Component & _object->getClassDesc()->GetFlags());
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
    void ImGuiWindow::displayProperty(core::IObject * _object, const IProperty * _prop)
    {
        VG_ASSERT(nullptr != _prop);
        const auto * factory = Kernel::getFactory();

        // TODO: Custom property edit
        //if (ImGuiPropertyHandler::display(_prop, _object))
        //    return; 

        const auto type = _prop->getType();
        const auto name = _prop->getName();
        const auto displayName = _prop->getDisplayName();
        const auto offset = _prop->getOffset();
        const auto flags = _prop->getFlags();

        if (asBool(IProperty::Flags::Hidden & flags))
            return;

        if (asBool(IProperty::Flags::SameLine & flags))
            ImGui::SameLine();

        bool changed = false;

        const bool readOnly = asBool(IProperty::Flags::ReadOnly & flags);

        ImGuiInputTextFlags imguiInputTextflags = ImGuiInputTextFlags_EnterReturnsTrue;
        if (readOnly)
            imguiInputTextflags = ImGuiInputTextFlags_ReadOnly;

        const bool flatten = asBool(IProperty::Flags::Flatten & flags);
        const bool isEnumArray = asBool(IProperty::Flags::EnumArray & flags);

        //ImGui::BeginDisabled(readOnly);
        {
            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case IProperty::Type::Bool:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    bool * pBool = _prop->GetPropertyBool(_object);
                    changed |= ImGui::Checkbox(getPropertyLabel(_prop).c_str(), pBool);
                };
                break;

                case IProperty::Type::EnumU8:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnum<u8>(_object, _prop);
                    break;

                case IProperty::Type::EnumU16:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnum<u16>(_object, _prop);
                    break;

                case IProperty::Type::EnumU32:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnum<u32>(_object, _prop);
                    break;

                case IProperty::Type::EnumFlagsU8:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnumFlags<u8>(_object, _prop);
                    break;

                case IProperty::Type::EnumFlagsU16:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnumFlags<u16>(_object, _prop);
                    break;

                case IProperty::Type::EnumFlagsU32:
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayEnumFlags<u32>(_object, _prop);
                    break;

                case IProperty::Type::Uint8:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    i8 * pU8 = (i8 *)(uint_ptr(_object) + offset);

                    i32 temp = (u8)*pU8;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        changed |= ImGui::SliderInt(displayName, &temp, max((int)0, (int)_prop->getRange().x), min((int)255, (int)_prop->getRange().y), "%d", imguiInputTextflags);
                    else
                        changed |= ImGui::InputInt(displayName, &temp, 1, 16, imguiInputTextflags);

                    if (changed)
                        *pU8 = (u8)temp;
                };
                break;

                case IProperty::Type::Uint16:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    i16 * pU16 = (i16 *)(uint_ptr(_object) + offset);

                    i32 temp = (u16)*pU16;

                    if (asBool(IProperty::Flags::HasRange & flags))
                        changed |= ImGui::SliderInt(displayName, &temp, max((int)0, (int)_prop->getRange().x), min((int)65535, (int)_prop->getRange().y), "%d", imguiInputTextflags);
                    else
                        changed |= ImGui::InputInt(displayName, &temp, 1, 16, imguiInputTextflags);

                    if (changed)
                        *pU16 = (u16)temp;
                };
                break;

                case IProperty::Type::Uint32:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    i32 * pU32 = (i32 *)(uint_ptr(_object) + offset);

                    if (asBool(IProperty::Flags::HasRange & flags))
                        changed |= ImGui::SliderInt(displayName, pU32, max(0, (int)_prop->getRange().x), (int)_prop->getRange().y, "%d", imguiInputTextflags);
                    else
                        changed |= ImGui::InputInt(displayName, pU32, 1, 16, imguiInputTextflags);
                };
                break;

                case IProperty::Type::Float:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    float * pFloat = _prop->GetPropertyFloat(_object);

                    if (asBool(IProperty::Flags::HasRange & flags))
                        changed |= ImGui::SliderFloat(displayName, pFloat, _prop->getRange().x, _prop->getRange().y);
                    else
                        changed |= ImGui::InputFloat(displayName, pFloat);
                };
                break;

                case IProperty::Type::Float2:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    float * pFloat2 = (float *)_prop->GetPropertyFloat2(_object);

                    changed |= ImGui::InputFloat2(displayName, pFloat2);
                };
                break;

                case IProperty::Type::Float3:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    float * pFloat4 = (float *)_prop->GetPropertyFloat3(_object);

                    if (asBool(IProperty::Flags::Color & flags))
                        changed |= ImGui::ColorEdit3(displayName, pFloat4);
                    else
                        changed |= ImGui::InputFloat3(displayName, pFloat4);
                };
                break;

                case IProperty::Type::Float4:
                {
                    float * pFloat4 = (float *)_prop->GetPropertyFloat4(_object);

                    if (isEnumArray)
                    {
                        char label[1024];
                        sprintf_s(label, "%s (%u)", displayName, _prop->getEnumCount());
                        if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_OpenOnArrow))
                        {
                            for (uint e = 0; e < _prop->getEnumCount(); ++e)
                            {
                                if (asBool(IProperty::Flags::Color & flags))
                                    changed |= ImGui::ColorEdit4(_prop->getEnumName(e), pFloat4 + e * 4);
                                else
                                    changed |= ImGui::InputFloat4(_prop->getEnumName(e), pFloat4 + e * 4);
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        if (asBool(IProperty::Flags::Color & flags))
                            changed |= ImGui::ColorEdit4(displayName, pFloat4);
                        else
                            changed |= ImGui::InputFloat4(displayName, pFloat4);
                    }
                };
                break;

                case IProperty::Type::Float4x4:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());
                    changed |= displayFloat4x4(displayName, _prop->GetPropertyFloat4x4(_object));
                }
                break;

                case IProperty::Type::String:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    bool selectPath = false;

                    string * pString = _prop->GetPropertyString(_object);

                    char buffer[1024];
                    sprintf_s(buffer, pString->c_str());
                    changed |= ImGui::InputText(getButtonLabel(displayName, _object).c_str(), buffer, countof(buffer), imguiInputTextflags);

                    if (changed)
                    {
                        *pString = buffer;

                        if (asBool(IProperty::Flags::IsFolder & flags))
                        {
                            ImGui::SameLine();
                            if (ImGui::Button("Path"))
                                selectPath = true;
                        }

                        if (selectPath)
                        {
                            ImGui::OpenPopup("Select folder");
                            selectPath = false;
                        }

                        if (asBool(IProperty::Flags::IsFolder & flags))
                        {
                            auto & fileBrowser = getFileBrowser();
                            if (fileBrowser.showFileDialog("Select folder", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT, style::dialog::Size))
                            {
                                const string newFolder = io::getRelativePath(fileBrowser.selected_path);

                                if (newFolder != *pString)
                                {
                                    *pString = newFolder;
                                    changed = true;
                                }
                            }
                        }

                        //ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
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

                    string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

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

                case IProperty::Type::ObjectRefVector:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    auto * vec = _prop->GetPropertyObjectRefVector(_object);
                    const uint count = vec->count();

                    string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

                    auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

                    if (count > 0)
                    {
                        if (!strcmp(displayName, "Components"))
                        {
                            for (uint i = 0; i < count; ++i)
                            {
                                IComponent * pComponent = (IComponent *)(*vec)[i];

                                string componentShortName = pComponent->getClassName();

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
                                const bool open = ImGui::CollapsingHeader(componentShortName.c_str(), nullptr, ImGuiTreeNodeFlags_None);

                                ImGuiComponentInspectorMenu componentInspectorMenu;
                                componentInspectorMenu.Display(pComponent);

                                if (open)
                                    displayArrayObject(pComponent, i, nullptr);
                            }
                        }
                        else
                        {
                            if (ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
                            {
                                for (uint i = 0; i < count; ++i)
                                {
                                    IObject * pObject = (*vec)[i];
                                    displayArrayObject(pObject, i, nullptr);
                                }
                                ImGui::TreePop();
                            }
                        }
                    }
                }
                break;

                case IProperty::Type::ObjectRefDictionary:
                {
                    VG_ASSERT(!isEnumArray, "Display of EnumArray property not implemented for type '%s'", asString(type).c_str());

                    dictionary<IObject *> * dic = _prop->GetPropertyObjectRefDictionary(_object);
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
                case IProperty::Type::ObjectRef:
                {
                    bool ref = (type == IProperty::Type::ObjectRef);
                    IObject * pObject = ref ? _prop->GetPropertyObjectRef(_object) : _prop->GetPropertyObject(_object);

                    string treeNodeName;
                    auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

                    //if (pObject)
                    //    treeNodeName = pObject->getName();
                    //else
                    treeNodeName = displayName;

                    if (isEnumArray)
                    {
                        char label[1024];
                        sprintf_s(label, "%s (%u)", displayName, _prop->getEnumCount());
                        if (ImGui::TreeNodeEx(label, treeNodeFlags))
                        {
                            for (uint e = 0; e < _prop->getEnumCount(); ++e)
                            {
                                pObject = ref ? _prop->GetPropertyObjectRef(_object, e) : _prop->GetPropertyObject(_object, e);

                                if (ImGui::TreeNodeEx(_prop->getEnumName(e), treeNodeFlags | ImGuiTreeNodeFlags_DefaultOpen))
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

                        if (nullptr != pObject)
                        {
                            auto classDesc = pObject->getClassDesc();
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
                                displayResource(pResource, _prop);
                                ImGui::Unindent();
                                ImGui::TreePop();
                            }
                        }
                        ImGui::Unindent();
                    }
                }
                break;

                case IProperty::Type::Resource:
                case IProperty::Type::ResourceRef:
                {
                    const bool ref = type == IProperty::Type::ResourceRef;

                    if (isEnumArray)
                    {
                        char label[1024];
                        sprintf_s(label, "%s (%u)", displayName, _prop->getEnumCount());
                        if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_OpenOnArrow))
                        {
                            for (uint e = 0; e < _prop->getEnumCount(); ++e)
                            {
                                auto pResource = ref ? _prop->GetPropertyResourceRef(_object, e) : _prop->GetPropertyResource(_object, e);

                                if (ImGui::TreeNodeEx(_prop->getEnumName(e), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen))
                                {
                                    if (nullptr != pResource)
                                    {
                                        changed |= displayResource(pResource, _prop, e);
                                    }

                                    ImGui::TreePop();
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    else
                    {
                        IResource * pResource = ref ? _prop->GetPropertyResourceRef(_object) : _prop->GetPropertyResource(_object);
                        changed |= displayResource(pResource, _prop);
                    }
                }
                break;
            }
        }
        //ImGui::EndDisabled();

        if (changed)
            _object->onPropertyChanged(_object, *_prop);
    }

    void * allocate(size_t size)
    {
        return operator new(size);
    };

    //--------------------------------------------------------------------------------------
    // Display Resource Object
    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::displayResource(core::IResource * _resource, const core::IProperty * _prop, core::uint _index)
    {
        bool changed = false;

        char buffer[1024];
        sprintf_s(buffer, _resource->GetResourcePath().c_str());
        string label = (string)"###" + to_string(uint_ptr(_resource));

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
        ImGui::Text(_prop->getDisplayName());
        auto x2 = ImGui::GetCursorPosX();
        ImGui::SameLine();
        string buttonLabel = (string)style::icon::File;// +(string)" " + (string)_prop->getDisplayName();
     
        ImGui::SetCursorPosX(x-4);
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

            if (_resource->CanSaveFile())
            {

                ImGui::BeginDisabled(nullptr == _resource->getObject());
                {
                    if (ImGui::MenuItem(saveFileButtonName.c_str()))
                    {
                        if (_resource->SaveFile(_resource->GetResourcePath()))
                            changed = true;
                    }

                    if (ImGui::MenuItem(saveAsFileButtonName.c_str()))
                        saveAsFile = true;
                }
                ImGui::EndDisabled();

                ImGui::Separator();
            }

            ImGui::BeginDisabled(_resource->GetResourcePath().empty());
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

        //ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 4);

        if (createNewFile)
            ImGui::OpenPopup(newFileButtonName.c_str());
        else if (openExistingFile)
            ImGui::OpenPopup(openFileButtonName.c_str());
        else if (saveAsFile)
            ImGui::OpenPopup(saveAsFileButtonName.c_str());

        // build extension list
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

        auto & fileBrowser = getFileBrowser();

        // Create new file
        if (fileBrowser.showFileDialog(newFileButtonName.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, style::dialog::Size, ext.c_str()))
        {
            const string newFilePath = fileBrowser.selected_path;
            if (_resource->CreateFile(newFilePath))
            {
                _resource->SetResourcePath(newFilePath);
                changed = true;
            }
        }

        // Open existing file
        if (fileBrowser.showFileDialog(openFileButtonName.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, style::dialog::Size, ext.c_str()))
        {
            const string newFilePath = fileBrowser.selected_path;
            if (_resource->GetResourcePath() != newFilePath)
            {
                _resource->SetResourcePath(newFilePath);
                changed = true;
            }
        }

        // Save existing file
        if (fileBrowser.showFileDialog(saveAsFileButtonName.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, style::dialog::Size, ext.c_str()))
        {
            const string newFilePath = fileBrowser.selected_path;
            if (_resource->SaveFile(newFilePath))
            {
                _resource->SetResourcePath(newFilePath);
                changed = true;
            }
        }

        displayObject(_resource);

        ImGui::PopID();

        return changed;
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
        if (isItemClicked())
        {
            if (ImGui::IsKeyDown(ImGuiMod_Ctrl))
            {
                if (selection->IsSelectedObject(_object))
                    selection->Remove(_object);
                else
                    selection->Add(_object);
            }
            else
            {
                selection->Clear();
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

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_PlotHistogramHovered]);
        ImGui::Text(_text);
        ImGui::PopStyleColor();

        if (ImGui::IsItemHovered())
        {
            underLine(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);

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
            underLine(ImGui::GetStyle().Colors[ImGuiCol_Button]);
        }
    }

    //--------------------------------------------------------------------------------------
    bool ImGuiWindow::displayFloat4x4(const core::string & _label, core::float4x4 * _pFloat4x4)
    {
        bool changed = false;

        changed |= ImGui::InputFloat4(((string)_label + ".I").c_str(), (float*)_pFloat4x4 + 0, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
        changed |= ImGui::InputFloat4(((string)_label + ".J").c_str(), (float*)_pFloat4x4 + 4, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
        changed |= ImGui::InputFloat4(((string)_label + ".K").c_str(), (float*)_pFloat4x4 + 8, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);
        changed |= ImGui::InputFloat4(((string)_label + ".T").c_str(), (float*)_pFloat4x4 + 12, "%.2f", ImGuiInputTextFlags_EnterReturnsTrue);

        return changed;
    }

}