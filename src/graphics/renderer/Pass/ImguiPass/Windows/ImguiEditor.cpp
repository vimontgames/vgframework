#include "graphics/renderer/Precomp.h"
#include "core/IObject.h"
#include "core/Math/Math.h"
#include "core/File/File.h"
#include "core/IFactory.h"
#include "core/IResource.h"
#include "core/IUniverse.h"
#include "core/IScene.h"
#include "core/ISector.h"
#include "core/IEntity.h"
#include "core/IComponent.h"
#include "imgui/imgui.h"
#include "ImguiEditor.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/Imgui/imguiAdapter.h"

#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.cpp"

// TODO: move to core
namespace vg::core
{
    template <size_t S> struct AnonymousStruct
    {
        char pad[S];
    };
}

using namespace vg::core;

namespace vg::graphics::renderer
{
    static const uint labelWidth = 14;
    static imgui_addons::ImGuiFileBrowser file_dialog;
    core::vector<core::IObject*> ImguiEditor::s_selection;

    //--------------------------------------------------------------------------------------
    ImguiEditor::ImguiEditor(const string & _name, Flags _flags) :
        m_name(_name),
        m_flags(_flags),
        m_isVisible(asBool(_flags & Flags::StartVisible))
    {

    }

    //--------------------------------------------------------------------------------------
    const vg::engine::IEngine * ImguiEditor::getEngine()
    {
        const auto * factory = Kernel::getFactory();
        return (const vg::engine::IEngine *) factory->getSingleton("Engine");
    }

    //--------------------------------------------------------------------------------------
    bool ImguiEditor::isVisible() const
    {
        return m_isVisible;
    }
    
    //--------------------------------------------------------------------------------------
    void ImguiEditor::setVisible(bool _visible)
    {
        m_isVisible = _visible;
    }

    //--------------------------------------------------------------------------------------
    const string & ImguiEditor::getName() const
    {
        return m_name;
    }

    //--------------------------------------------------------------------------------------
    ImguiEditor::Flags ImguiEditor::getFlags() const
    {
        return m_flags;
    }

    //--------------------------------------------------------------------------------------
    string ImguiEditor::getPropertyLabel(const IProperty * _prop)
    {
        string name = _prop->getDisplayName();
        string className = _prop->getClassName();

        if (name.length() < labelWidth)
        {
            const auto spaces = labelWidth - name.length();
            name.append(spaces, ' ');
        }

        name += "##" + className;
        return name;
    }

    //--------------------------------------------------------------------------------------
    string ImguiEditor::getButtonLabel(string _baseName, IObject * _object)
    {
        return _baseName + "##" + _object->getClassName() + "##" + _object->getName();
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool ImguiEditor::displayEnum(core::IObject * _object, const core::IProperty * _prop)
    {
        const auto displayName = _prop->getDisplayName();
        const auto offset = _prop->getOffset();
        const auto flags = _prop->getFlags();

        const bool readonly = asBool(IProperty::Flags::ReadOnly & flags);

        T * pEnum = (T*)(uint_ptr(_object) + offset);
        int enumVal = (int)*pEnum;

        char temp[2048] = { '\0' };
        char * p = temp;
        for (uint e = 0; e < _prop->getEnumCount(); ++e)
        {
            const char * name = _prop->getEnumName(e);
            for (int c = 0; c < strlen(name); c++)
                *p++ = name[c];
            *p++ = '\0';
        }
        *p++ = '\0';

        bool changed = ImGui::Combo(getPropertyLabel(_prop).c_str(), &enumVal, temp) && !readonly;
        if (changed)
            *pEnum = enumVal;
        return changed;
    }

    //--------------------------------------------------------------------------------------
    template <typename T> bool ImguiEditor::displayEnumFlags(core::IObject * _object, const core::IProperty * _prop)
    {
        const auto displayName = _prop->getDisplayName();
        const auto offset = _prop->getOffset();
        const auto flags = _prop->getFlags();

        const bool readonly = asBool(IProperty::Flags::ReadOnly & flags);

        T * pEnum = (T*)(uint_ptr(_object) + offset);
        int enumVal = (int)*pEnum;

        bool changed = false;
        string preview;

        bool first = true;
        for (uint e = 0; e < _prop->getEnumCount(); ++e)
        {
            if (enumVal & (1 << e))
            {
                if (first)
                    first = false;
                else
                    preview += "|";

                preview += _prop->getEnumName(e);
            }
        }
        if (first)
            preview = "<None>";

        if (ImGui::BeginCombo(getPropertyLabel(_prop).c_str(), preview.c_str(), ImGuiComboFlags_None))
        {
            for (uint e = 0; e < _prop->getEnumCount(); ++e)
            {
                bool value = ((enumVal >> e) & 1) ? true : false;
                const char * name = _prop->getEnumName(e);
                if (ImGui::Checkbox(name, &value))
                {
                    if (value)
                        enumVal |= 1 << e;
                    else
                        enumVal &= ~(1 << e);

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
    void ImguiEditor::displayObject(core::IObject * _object)
    {
        const char * className = _object->getClassName();

        const auto * factory = Kernel::getFactory();
        const auto * classDesc = factory->getClassDescriptor(className);

        if (!classDesc)
            return;

        const char * classDisplayName = classDesc->getClassDisplayName();

        ImGui::PushItemWidth(196);

        // Sort top-level properties to display components at the end
        const IProperty * components = nullptr;
        const char * curClassName = nullptr;
        bool visible = false;

        for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
        {
            const IProperty * prop = classDesc->getPropertyByIndex(i);

            //if (!strcmp(prop->getName(), "m_object"))
            //{
            //    IObject * pObject = *(IObject**)(uint_ptr(_object) + prop->getOffset());
            //    if (pObject == nullptr)
            //        continue;
            //}

            if (strcmp(prop->getName(), "m_components"))
            {
                if (curClassName != prop->getClassName())
                {
                    curClassName = prop->getClassName();
                    visible = true; // ImGui::CollapsingHeader(curClassName, nullptr, ImGuiTreeNodeFlags_DefaultOpen);
                }

                if (visible)
                    displayProperty(prop, _object);
            }
            else
                components = prop;
        }

        if (nullptr != components)
        {
            if (ImGui::CollapsingHeader("Components", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();
                displayProperty(components, _object);

                //ImGui::Spacing();
                //if (ImGui::Button("Add Component"))
                //{
                //    // TODO
                //}
                ImGui::Unindent();
            }
        }

        if (!strcmp(_object->getClassName(), "Texture"))
        {
            // Texture preview (WIP)
            auto * tex = (driver::Texture*)_object;
            auto imGuiAdapter = Renderer::get()->getImGuiAdapter();
            ImTextureID texID = imGuiAdapter->getImguiTextureID(tex);
            {
                ImGui::Image(texID, ImVec2(128, 128));
            }
            imGuiAdapter->releaseImguiTextureID(texID);
        }

        ImGui::PopItemWidth();
    }

    //--------------------------------------------------------------------------------------
    void ImguiEditor::displayArrayObject(IObject * _object, core::uint _index, const char * _name)
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

        const bool isComponent = nullptr != _object && _object->getClassDesc() && asBool(_object->getClassDesc()->getFlags() & (IClassDesc::Flags::Component));
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
                else
                    ImGui::TextDisabled("<Null>");

                ImGui::TreePop();
            }
            else
                updateSelection(_object);
        }

        ImGui::PopStyleColor();
    }

    //--------------------------------------------------------------------------------------
    void ImguiEditor::displayProperty(const IProperty * _prop, core::IObject * _object)
    {
        VG_ASSERT(nullptr != _prop);

        const auto type = _prop->getType();
        const auto name = _prop->getName();
        const auto displayName = _prop->getDisplayName();
        const auto offset = _prop->getOffset();
        const auto flags = _prop->getFlags();

        if (asBool(IProperty::Flags::Hidden & flags))
            return;

        if (asBool(IProperty::Flags::SameLine & flags))
            ImGui::SameLine();

        ImGuiInputTextFlags imguiInputTextflags = 0;

        bool changed = false;

        if (asBool(IProperty::Flags::ReadOnly & flags))
            imguiInputTextflags = ImGuiInputTextFlags_ReadOnly;

        switch (type)
        {
        default:
            VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
            break;

        case IProperty::Type::Bool:
        {
            bool * pBool = (bool*)(uint_ptr(_object) + offset);
            changed |= ImGui::Checkbox(getPropertyLabel(_prop).c_str(), pBool);
        };
        break;

        case IProperty::Type::EnumU8:
            changed |= displayEnum<u8>(_object, _prop);
            break;

        case IProperty::Type::EnumU16:
            changed |= displayEnum<u16>(_object, _prop);
            break;

        case IProperty::Type::EnumU32:
            changed |= displayEnum<u32>(_object, _prop);
            break;

        case IProperty::Type::EnumFlagsU8:
            changed |= displayEnumFlags<u8>(_object, _prop);
            break;

        case IProperty::Type::EnumFlagsU16:
            changed |= displayEnumFlags<u16>(_object, _prop);
            break;

        case IProperty::Type::EnumFlagsU32:
            changed |= displayEnumFlags<u32>(_object, _prop);
            break;

        case IProperty::Type::Uint32:
        {
            i32 * pU32 = (i32*)(uint_ptr(_object) + offset);

            if (asBool(IProperty::Flags::HasRange & flags))
                changed |= ImGui::SliderInt(displayName, pU32, max(0, (int)_prop->getRange().x), (int)_prop->getRange().y, "%d", imguiInputTextflags);
            else
                changed |= ImGui::InputInt(displayName, pU32, 1, 16, imguiInputTextflags);
        };
        break;

        case IProperty::Type::Uint16:
        {
            i16 * pU16 = (i16*)(uint_ptr(_object) + offset);

            i32 temp = *pU16;

            if (asBool(IProperty::Flags::HasRange & flags))
                changed |= ImGui::SliderInt(displayName, &temp, max((int)0, (int)_prop->getRange().x), min((int)65535, (int)_prop->getRange().y), "%d", imguiInputTextflags);
            else
                changed |= ImGui::InputInt(displayName, &temp, 1, 16, imguiInputTextflags);

            if (changed)
                *pU16 = temp;
        };
        break;

        case IProperty::Type::Float:
        {
            float * pFloat = (float*)(uint_ptr(_object) + offset);

            if (asBool(IProperty::Flags::HasRange & flags))
                changed |= ImGui::SliderFloat(displayName, pFloat, _prop->getRange().x, _prop->getRange().y);
            else
                changed |= ImGui::InputFloat(displayName, pFloat);
        };
        break;

        case IProperty::Type::Matrix44:
        {
            float * pFloat4x4 = (float*)(uint_ptr(_object) + offset);

            changed |= ImGui::InputFloat4(((string)displayName + ".I").c_str(), pFloat4x4 + 0, "%.2f", imguiInputTextflags);
            changed |= ImGui::InputFloat4(((string)displayName + ".J").c_str(), pFloat4x4 + 4, "%.2f", imguiInputTextflags);
            changed |= ImGui::InputFloat4(((string)displayName + ".K").c_str(), pFloat4x4 + 8, "%.2f", imguiInputTextflags);
            changed |= ImGui::InputFloat4(((string)displayName + ".T").c_str(), pFloat4x4 + 12, "%.2f", imguiInputTextflags);

        }
        break;

        case IProperty::Type::Float4:
        {
            float * pFloat4 = (float*)(uint_ptr(_object) + offset);

            if (asBool(IProperty::Flags::Color & flags))
                changed |= ImGui::ColorEdit4(displayName, pFloat4);
            else
                changed |= ImGui::SliderFloat4(displayName, pFloat4, 0.0f, 1.0f);
        };
        break;

        case IProperty::Type::String:
        {
            bool selectPath = false;

            string * pString = (string*)(uint_ptr(_object) + offset);

            char buffer[1024];
            sprintf_s(buffer, pString->c_str());
            changed |= ImGui::InputText(displayName, buffer, countof(buffer), imguiInputTextflags);

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
                    if (file_dialog.showFileDialog("Select folder", imgui_addons::ImGuiFileBrowser::DialogMode::SELECT, ImVec2(700, 310)))
                    {
                        const string newFolder = io::getRelativePath(file_dialog.selected_path);

                        if (newFolder != *pString)
                        {
                            *pString = newFolder;
                            changed = true;
                        }
                    }
                }
            }
        }
        break;

        case IProperty::Type::Function:
        {
            IProperty::Func pFunc = (IProperty::Func)offset;

            if (ImGui::Button(displayName))
                pFunc(_object);
        }
        break;

        case IProperty::Type::ObjectVector:
        {
            const uint sizeOf = _prop->getValue();
            byte * data = nullptr;

            // Not great, not terrible
            size_t count = 0;
            switch (sizeOf)
            {
            default:
                VG_ASSERT(false, "case \'%u\': is not implemented", sizeOf);
                break;

            case 64:
                count = (size_t)((vector<core::AnonymousStruct<64>>*)(uint_ptr(_object) + offset))->size();
                data = (byte*)((vector<core::AnonymousStruct<64>>*)(uint_ptr(_object) + offset))->data();
                break;

            case 72:
                count = (size_t)((vector<core::AnonymousStruct<72>>*)(uint_ptr(_object) + offset))->size();
                data = (byte*)((vector<core::AnonymousStruct<72>>*)(uint_ptr(_object) + offset))->data();
                break;

            case 128:
                count = (size_t)((vector<core::AnonymousStruct<128>>*)(uint_ptr(_object) + offset))->size();
                data = (byte*)((vector<core::AnonymousStruct<128>>*)(uint_ptr(_object) + offset))->data();
                break;

            case 160:
                count = (size_t)((vector<core::AnonymousStruct<160>>*)(uint_ptr(_object) + offset))->size();
                data = (byte*)((vector<core::AnonymousStruct<160>>*)(uint_ptr(_object) + offset))->data();
                break;
            }

            string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));

            auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

            if (count > 0 && ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
            {
                for (uint i = 0; i < count; ++i)
                {
                    IObject * pObject = (IObject *)(data + sizeOf * i);
                    displayArrayObject(pObject, i, nullptr);
                }

                ImGui::TreePop();
            }

            ImGui::PopStyleColor();
        }
        break;

        case IProperty::Type::ObjectPointerVector:
        {
            vector<IObject*> * vec = (vector<IObject*>*)(uint_ptr(_object) + offset);
            const uint count = (uint)vec->size();

            string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

            auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

            if (count > 0)
            {
                if (!strcmp(displayName, "Components"))
                {
                    for (uint i = 0; i < count; ++i)
                    {
                        IComponent * pComponent = (IComponent*)(*vec)[i];

                        // Display one component
                        string componentShortName = pComponent->getClassName();
                        auto nPos = componentShortName.find("Component");
                        if (-1 != nPos)
                            componentShortName.erase(nPos);

                        if (ImGui::CollapsingHeader(componentShortName.c_str(), nullptr, ImGuiTreeNodeFlags_None))
                        {
                            ImGui::Indent();
                            displayArrayObject(pComponent, i, nullptr);
                            ImGui::Unindent();
                        }
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

        case IProperty::Type::ObjectPointerDictionary:
        {
            dictionary<IObject*> * dic = (dictionary<IObject*>*)(uint_ptr(_object) + offset);
            const uint count = (uint)dic->size();

            string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));

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

            ImGui::PopStyleColor();
        }
        break;

        case IProperty::Type::ObjectPointer:
        {
            IObject * pObject = *(IObject**)(uint_ptr(_object) + offset);

            string treeNodeName;

            if (pObject)
                treeNodeName = pObject->getName();
            else
                treeNodeName = displayName;

            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_Text));

            auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

            if (nullptr != pObject && pObject->getClassDesc() && asBool(pObject->getClassDesc()->getFlags() & (IClassDesc::Flags::Component)))
                treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;

            bool needTreeNode = strcmp(_prop->getName(),"m_object");
            bool treenNodeOpen = !needTreeNode || ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags);

            static int ObjectRightClicMenuIndex = -1;
            bool needOpenPopup = false;

            if (ImGui::BeginPopupContextItem())
            {
                ImGui::PushID("ObjectRightClicMenu");

                //if (ImGui::MenuItem("Cut", "Ctrl-X"))
                //    ObjectRightClicMenuIndex = 0;
                //
                //if (ImGui::MenuItem("Copy", "Ctrl-C"))
                //    ObjectRightClicMenuIndex = 1;
                //
                //if (ImGui::MenuItem("Paste", "Ctrl-V"))
                //    ObjectRightClicMenuIndex = 2;

                if (ImGui::MenuItem("Rename", "Ctrl-R"))
                {
                    needOpenPopup = true;
                    ObjectRightClicMenuIndex = 3;
                }

                ImGui::PopID();

                ImGui::EndPopup();
            }

            if (ObjectRightClicMenuIndex == 3)
            {
                static bool isRenamePopopOpened = true;

                if (needOpenPopup)
                    ImGui::OpenPopup("Rename");

                if (ImGui::BeginPopupModal("Rename", &isRenamePopopOpened, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    static char nameTmp[1024] = { '\0' };

                    if (nameTmp[0] == '\0')
                        strcpy(nameTmp, pObject->getName().c_str());

                    changed |= ImGui::InputText("", nameTmp, countof(nameTmp), ImGuiInputTextFlags_AutoSelectAll);

                    string newName = nameTmp;

                    if (ImGui::Button("Rename", ImVec2(80, 0)))
                    {
                        pObject->setName(newName);
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        ObjectRightClicMenuIndex = -1;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", ImVec2(80, 0)))
                    {
                        ImGui::CloseCurrentPopup();
                        nameTmp[0] = '\0';
                        ObjectRightClicMenuIndex = -1;
                    }

                    ImGui::EndPopup();
                }
            }

            if (treenNodeOpen)
            {
                updateSelection(_object);

                if (pObject)
                    displayObject(pObject);
                else
                    ImGui::TextDisabled("null");

                if (needTreeNode)
                    ImGui::TreePop();
            }
            else
            {
                updateSelection(pObject);
            }

            ImGui::PopStyleColor();
        }
        break;

        case IProperty::Type::ResourceVector:
        {
            const uint sizeOf = _prop->getValue();
            byte * data = nullptr;

            // Not great, not terrible
            size_t count = 0;
            switch (sizeOf)
            {
            default:
                VG_ASSERT(false, "case \'%u\': is not implemented", sizeOf);
                break;

            case 64:
                count = (size_t)((vector<core::AnonymousStruct<64>>*)(uint_ptr(_object) + offset))->size();
                data = (byte*)((vector<core::AnonymousStruct<64>>*)(uint_ptr(_object) + offset))->data();
                break;

            case 72:
                count = (size_t)((vector<core::AnonymousStruct<72>>*)(uint_ptr(_object) + offset))->size();
                data = (byte*)((vector<core::AnonymousStruct<72>>*)(uint_ptr(_object) + offset))->data();
                break;

            case 128:
                count = (size_t)((vector<core::AnonymousStruct<128>>*)(uint_ptr(_object) + offset))->size();
                data = (byte*)((vector<core::AnonymousStruct<128>>*)(uint_ptr(_object) + offset))->data();
                break;

            case 160:
                count = (size_t)((vector<core::AnonymousStruct<160>>*)(uint_ptr(_object) + offset))->size();
                data = (byte*)((vector<core::AnonymousStruct<160>>*)(uint_ptr(_object) + offset))->data();
                break;
            }

            string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));

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
                        displayResource(pResource);
                        ImGui::Unindent();
                        ImGui::TreePop();
                    }

                    //if (pResource->getObject())
                    //    displayObject(pResource->getObject());
                }
                ImGui::Unindent();
            }

            ImGui::PopStyleColor();
        }
            break;

        case IProperty::Type::Resource:
        {
            IResource * pResource = (IResource*)(uint_ptr(_object) + offset);
            changed |= displayResource(pResource);
        }
        break;
        }

        if (changed)
            _object->onPropertyChanged(*_prop);
    }

    static core::IResource * editedResource = nullptr;

    //--------------------------------------------------------------------------------------
    bool ImguiEditor::displayResource(core::IResource * _resource)
    {
        bool changed = false;
        IObject * pObject = _resource->getObject();

        bool open = false;// , save = false;

        char buffer[1024];
        sprintf_s(buffer, _resource->getPath().c_str());
        if (ImGui::InputText("##File", buffer, countof(buffer), (ImGuiInputTextFlags)0))
            _resource->setPath(buffer);

        string openFileButtonName = getButtonLabel("File", _resource);
        string openFileDialogName = getButtonLabel("Open file", _resource);

        ImGui::SameLine();
        if (ImGui::Button(openFileButtonName.c_str()))
            open = true;        

        if (open)
        {
            ImGui::OpenPopup(openFileDialogName.c_str());
            open = false;
            editedResource = _resource;
        }

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

        if (file_dialog.showFileDialog(openFileDialogName.c_str(), imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ext.c_str()))
        {
            const string newFilePath = file_dialog.selected_path;
            if (_resource->getPath() != newFilePath)
            {
                _resource->setPath(newFilePath);
                changed = true;
            }
        }

        displayObject(_resource);

        return changed;
    }

    //--------------------------------------------------------------------------------------
    bool ImguiEditor::isItemClicked()
    {
        if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
            return true;
        else
            return false;
    }

    //--------------------------------------------------------------------------------------
    bool ImguiEditor::updateSelection(core::IObject * _object)
    {
        if (isItemClicked())
        {
            setSelectedObject(_object);
            return true;
        }
        return false;
    }

    //--------------------------------------------------------------------------------------
    IObject * ImguiEditor::getSelectedObject()
    {
        if (s_selection.count() > 0)
            return s_selection[0];
        else
            return nullptr;
    }

    //--------------------------------------------------------------------------------------
    void ImguiEditor::setSelectedObject(core::IObject * _object)
    {
        s_selection.clear();
        s_selection.push_back(_object);
    }

    //--------------------------------------------------------------------------------------
    vector<IObject*> & ImguiEditor::getSelectedObjects()
    {
        return s_selection;
    }

    //--------------------------------------------------------------------------------------
    void ImguiEditor::setSelectedObjects(core::vector<core::IObject*> & _objects)
    {
        s_selection = _objects;
    }

    //--------------------------------------------------------------------------------------
    void ImguiEditor::underLine(const ImColor & _color)
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
    void ImguiEditor::textURL(const char * _text, const char * _url)
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

}