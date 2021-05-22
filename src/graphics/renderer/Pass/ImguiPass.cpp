#include "graphics/renderer/Precomp.h"
#include "ImguiPass.h"
#include "graphics/driver/Device/Device.h"
#include "graphics/renderer/Renderer.h"
#include "graphics/renderer/imgui/imguiAdapter.h"
#include "graphics/renderer/Options/DisplayOptions.h"
#include "engine/IEngine.h"
#include "core/Plugin/Plugin.h"
#include "core/IObjectFactory.h"
#include "core/IResource.h"
#include "core/File/File.h"
#include "ImGui-Addons/FileBrowser/ImGuiFileBrowser.cpp"

#include "graphics/renderer/Geometry/Batch.h"

using namespace vg::core;
using namespace vg::graphics::driver;
using namespace vg::graphics::renderer;

namespace vg::core
{
    template <size_t S> struct AnonymousStruct
    {
        char pad[S];
    };
}

namespace ImGui
{
    //--------------------------------------------------------------------------------------
    void underLine(ImColor _color)
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
    void textURL(const char * _text, const char * _url = nullptr)
    {
        if (nullptr == _url)
            _url = _text;

        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
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

namespace vg::graphics::renderer
{
    //--------------------------------------------------------------------------------------
    void ImguiPass::setup(double _dt)
    {
        writeRenderTarget(0, "Backbuffer");

        ImGuiViewport * viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Appearing, ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(viewport->WorkSize, ImGuiCond_None);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None | ImGuiDockNodeFlags_PassthruCentralNode;

        bool showUI = true;

        ImGui::Begin("DockSpace", &showUI, window_flags);
        {
            ImGui::PopStyleVar(3);

            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Plugins"))
                {
                    if (ImGui::MenuItem("Engine"))
                        m_isEngineWindowVisible = true;

                    if (ImGui::MenuItem("Renderer"))
                        m_isRendererWindowVisible = true;

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("View"))
                {
                    if (ImGui::MenuItem("Performance"))
                        m_isPerfWindowVisible = true;

                    if (ImGui::MenuItem("Scene"))
                        m_isSceneWindowVisible = true; 

                    if (ImGui::MenuItem("Selection"))
                        m_isCurrentSelectionWindowVisible = true;

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Options"))
                {
                    if (ImGui::MenuItem("Display"))
                        m_isDisplayOptionsWindowsVisible = true;

                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::MenuItem("About VG Framework"))
                        m_isAboutWindowVisible = true;

                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            if (m_isAboutWindowVisible)
            {
                if (ImGui::Begin("About", &m_isAboutWindowVisible))
                {
                    float columnWidth[3] = { 256, 128, 512 };

                    ImGui::Text("VG Framework 0.1");
                    ImGui::Text("");

                    ImGui::Columns(3, "author", false);

                    ImGui::SetColumnWidth(0, columnWidth[0]);
                    ImGui::SetColumnWidth(1, columnWidth[1]);
                    ImGui::SetColumnWidth(2, columnWidth[2]);

                    ImGui::Text("Website");
                    ImGui::Text("Twitter");
                    ImGui::Text("");

                    ImGui::NextColumn();
                    ImGui::NextColumn();

                    ImGui::textURL("github.com/vimontgames/vgframework", "https://github.com/vimontgames/vgframework");
                    ImGui::textURL("Benoit Vimont", "https://twitter.com/benoitvimont");
                    ImGui::Text("");

                    ImGui::Separator();

                    ImGui::Columns(1);
                    ImGui::Text("SDKs:");
                    ImGui::Text("");

                    ImGui::Columns(3, "SDK", false);

                    ImGui::SetColumnWidth(0, columnWidth[0]);
                    ImGui::SetColumnWidth(1, columnWidth[1]);
                    ImGui::SetColumnWidth(2, columnWidth[2]);

                    // name                        
                    ImGui::Text("FBX SDK");
                    ImGui::Text("Vulkan SDK");
                    ImGui::Text("Windows 10 SDK");

                    ImGui::Text("");
                    ImGui::NextColumn();

                    // version
                    ImGui::Text("2020.0.1");
                    ImGui::Text("1.2.148.0");
                    ImGui::Text("10.0.17763.0");

                    ImGui::Text("");
                    ImGui::NextColumn();

                    // url
                    ImGui::textURL("www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0", "https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0");
                    ImGui::textURL("vulkan.lunarg.com/sdk/home", "https://vulkan.lunarg.com/sdk/home");
                    ImGui::textURL("developer.microsoft.com/fr-fr/windows/downloads/sdk-archive", "https://developer.microsoft.com/fr-fr/windows/downloads/sdk-archive/");
                    ImGui::Text("");

                    ImGui::Separator();

                    ImGui::Columns(1);
                    ImGui::Text("3rd-party libs:");
                    ImGui::Text("");

                    ImGui::Columns(3, "3rdparties", false);

                    ImGui::SetColumnWidth(0, columnWidth[0]);
                    ImGui::SetColumnWidth(1, columnWidth[1]);
                    ImGui::SetColumnWidth(2, columnWidth[2]);

                    struct ThirdPartyInfo
                    {
                        const char * name;
                        const char * version;
                        const char * url;
                    };

                    ThirdPartyInfo thirdParties[] =
                    {
                        { "D3D12MemoryAllocator",   "",                 "https://github.com/GPUOpen-LibrariesAndSDKs/D3D12MemoryAllocator" },
                        { "Dear ImGui",             "d5a4d53",          "https://github.com/ocornut/imgui" },
                        { "Dirent",                 "833b692",          "https://github.com/tronkko/dirent" },
                        { "hlslpp",                 "",                 "https://github.com/redorav/hlslpp" },
                        { "ImGui-Addons",           "ea0af59",          "https://github.com/gallickgunner/ImGui-Addons" },
                        { "magic_enum",             "",                 "https://github.com/Neargye/magic_enum" },
                        { "optick",                 "",                 "https://github.com/bombomby/optick" },
                        { "px_sched",               "",                 "https://github.com/pplux/px" },
                        { "stb_image",              "",                 "https://github.com/nothings/stb" },
                        { "VulkanMemoryAllocator",  "",                 "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator" },
                        { "WinPixEventRuntime",     "1.0.200127001",    "https://www.nuget.org/packages/WinPixEventRuntime" },
                    };


                    for (uint i = 0; i < countof(thirdParties); ++i)
                        ImGui::Text(thirdParties[i].name);

                    ImGui::NextColumn();

                    for (uint i = 0; i < countof(thirdParties); ++i)
                        ImGui::Text(thirdParties[i].version);

                    ImGui::NextColumn();

                    for (uint i = 0; i < countof(thirdParties); ++i)
                        ImGui::textURL(thirdParties[i].url + strlen("https://"), thirdParties[i].url);

                    ImGui::End();
                }
            }

            static const uint smoothDtTime = 500; // 0.5s

            m_accum += _dt;
            m_frame++;

            if (m_accum > (double)smoothDtTime)
            {
                m_dt = (float)(m_accum / (float)m_frame);
                m_fps = (float)1000.0f / m_dt;
                m_accum = 0.0;
                m_frame = 0;
            }
        }
        ImGui::End();

        if (m_isPerfWindowVisible)
            displayPerformanceWindow(_dt);

        if (m_isDisplayOptionsWindowsVisible)
            displayOptionsWindow();

        if (m_isEngineWindowVisible)
            displayEngineWindow();

        if (m_isRendererWindowVisible)
            displayRendererWindow();

        if (m_isSceneWindowVisible)
            displaySceneWindow();

        if (m_isCurrentSelectionWindowVisible)
            displayCurrentSelectionWindow();

        static bool showDemo = false;
        if (showDemo)
            ImGui::ShowDemoWindow(&showDemo);
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayPerformanceWindow(double _dt)
    {
        if (ImGui::Begin("Performance", &m_isPerfWindowVisible))
        {
            if (ImGui::CollapsingHeader("Target", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border

                ImGui::Text("Platform:");
                ImGui::Text("Configuration:");
                ImGui::Text("Graphics API:");
                ImGui::NextColumn();
                ImGui::Text(Plugin::getPlatform().c_str());
                ImGui::Text(Plugin::getConfiguration().c_str());
                ImGui::Text(asString(Device::get()->getDeviceParams().api).c_str());
                ImGui::Columns(1);
            }

            if (ImGui::CollapsingHeader("Performance", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
                ImGui::Text("FPS: ");
                ImGui::Text("Frame: ");
                ImGui::NextColumn();
                ImGui::Text("%.0f img/sec", m_fps);
                ImGui::Text("%.4f ms", m_dt);

                ImGui::Columns(1);
                ImGui::Text("Press 'F1' to start/stop profiler");

                if (VG_PROFILE_CAPTURE_IN_PROGRESS())
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Capture in progress ... %u", m_captureFrameCounter);
                    m_captureFrameCounter++;
                }
                else if (0 != m_captureFrameCounter)
                {
                    m_captureFrameCounter = 0;
                }
            }

            if (ImGui::CollapsingHeader("Shaders", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Press 'F6' to hot reload shaders");
            }
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayOptionsWindow()
    {
        core::IObject * displayOptions = DisplayOptions::get();
        if (displayOptions)
        {
            if (ImGui::Begin("Display Options", &m_isDisplayOptionsWindowsVisible))
                displayObject(displayOptions);
            ImGui::End();
        }
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayEngineWindow()
    {
        if (ImGui::Begin("Engine", &m_isEngineWindowVisible))
        {
            const auto * factory = Kernel::getObjectFactory();
            IObject * engine = factory->getSingleton("Engine");
            if (engine)
            {
                const char * className = engine->getClassName();
                const auto * classDesc = factory->getClassDescriptor(className);
                const char * classDisplayName = classDesc->getClassDisplayName();

                displayObject(engine);
            }
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayRendererWindow()
    {
        if (ImGui::Begin("Renderer", &m_isRendererWindowVisible))
        {
            const auto * factory = Kernel::getObjectFactory();
            IObject * renderer = factory->getSingleton("Renderer");
            if (renderer)
                displayObject(renderer);
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displaySceneWindow()
    {
        if (ImGui::Begin("Scene", &m_isSceneWindowVisible))
        {
            const auto * factory = Kernel::getObjectFactory();
            engine::IEngine * engine = (engine::IEngine *)factory->getSingleton("Engine");
            core::IObject * scene = (core::IObject *)engine->getScene();

            if (scene)
                displayObject(scene, UIMode::Scene);
        }
        ImGui::End();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayCurrentSelectionWindow()
    {
        if (ImGui::Begin("Selection", &m_isCurrentSelectionWindowVisible))
        {
            if (m_selected)
                displayObject(m_selected, UIMode::ObjectPointer);
        }
        ImGui::End();
    }   

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayArrayObject(IObject * _object, core::uint _index, u32 _color, const char * _name, UIMode _mode)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, _color);

        string treeNodeName;
        if (_object)
            treeNodeName = _object->getName();
        else
            treeNodeName = "[" + to_string(_index) + "]";

        auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

        if (nullptr != _object && asBool(_object->getClassDesc()->getFlags() & (IObjectDescriptor::Flags::Component)))
        {
            treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;

            if (UIMode::Scene == _mode)
                treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if (treeNodeName.length() == 0)
            treeNodeName = "[" + to_string(_index) + "]";

        if (ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
        {
            updateSelection(_object, _mode);

            if (_object)
                displayObject(_object, _mode);
            else
                ImGui::TextDisabled("null");

            ImGui::TreePop();
        }
        else
            updateSelection(_object, _mode);

        ImGui::PopStyleColor();
    }

    //--------------------------------------------------------------------------------------
    string getFixedSizeString(const string & _string, uint _size)
    {
        const auto spaces = _size - _string.length();
        VG_ASSERT(spaces >= 0 && spaces <= _size);
        string result = _string;
               result.append(spaces, ' ');
        return result;
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::displayObject(core::IObject * _object, UIMode _mode)
    {
        static imgui_addons::ImGuiFileBrowser file_dialog;

        const char * className = _object->getClassName();

        const auto * factory = Kernel::getObjectFactory();
        const auto * classDesc = factory->getClassDescriptor(className);

        if (!classDesc)
            return;

        const u32 objectColor = ImGui::GetColorU32(ImGuiCol_Text);
        const u32 containerColor = objectColor & 0x7FFFFFFF;

        const char * classDisplayName = classDesc->getClassDisplayName();

        ImGui::PushItemWidth(196);

        IPropertyDescriptor::Type previous = IPropertyDescriptor::Type::Undefined;

        //if (UIMode::Scene != _mode)
        //    ImGui::TextDisabled(className);

        for (uint i = 0; i < classDesc->getPropertyCount(); ++i)
        {
            const auto & prop = classDesc->getPropertyByIndex(i);

            const auto type = prop->getType();
            const auto name = prop->getName();
            const auto displayName = prop->getDisplayName();
            const auto offset = prop->getOffset();
            const auto flags = prop->getFlags();

            if (UIMode::Scene == _mode)
            {
                switch (type)
                {
                    case IPropertyDescriptor::Type::ObjectPointer:
                    case IPropertyDescriptor::Type::ObjectPointerVector:
                        break;

                    default:
                        continue;
                }
            }

            if (asBool(IPropertyDescriptor::Flags::Hidden & flags))
                continue;

            if (asBool(IPropertyDescriptor::Flags::SameLine & flags))
                ImGui::SameLine();

            ImGuiInputTextFlags imguiInputTextflags = 0;

            bool changed = false;

            if (asBool(IPropertyDescriptor::Flags::ReadOnly & flags))
                imguiInputTextflags = ImGuiInputTextFlags_ReadOnly;

            const uint boolWidth = 12;
            const uint enumWidth = 12;
            
            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);
                    break;

                case IPropertyDescriptor::Type::Bool:
                {
                    bool * pBool = (bool*)(uint_ptr(_object) + offset);
					changed |= ImGui::Checkbox(getFixedSizeString(displayName, boolWidth).c_str(), pBool);
                };
                break;

				case IPropertyDescriptor::Type::Enum:
				{
					u32 * pEnum = (u32*)(uint_ptr(_object) + offset);
					changed |= ImGui::RadioButton(getFixedSizeString(displayName, enumWidth).c_str(), (int*)pEnum, prop->getValue());
				};
				break;

                case IPropertyDescriptor::Type::Uint32:
                {
                    i32 * pU32 = (i32*)(uint_ptr(_object) + offset);

                    if (asBool(IPropertyDescriptor::Flags::HasRange & flags))
                        changed |= ImGui::SliderInt(displayName, pU32, (int)prop->getRange().x, (int)prop->getRange().y, "%d", imguiInputTextflags);
                    else
                        changed |= ImGui::InputInt(displayName, pU32, 1, 16, imguiInputTextflags);
                };
                break;

                case IPropertyDescriptor::Type::Float:
                {
                    float * pFloat = (float*)(uint_ptr(_object) + offset);

                    if (asBool(IPropertyDescriptor::Flags::HasRange & flags))
                        changed |= ImGui::SliderFloat(displayName, pFloat, prop->getRange().x, prop->getRange().y);
                    else
                        changed |= ImGui::InputFloat(displayName, pFloat);
                };
                break;

                case IPropertyDescriptor::Type::Matrix44:
                {
                    float * pFloat4x4 = (float*)(uint_ptr(_object) + offset);

                    changed |= ImGui::InputFloat4(((string)displayName + ".I").c_str(), pFloat4x4 + 0 , "%.2f", imguiInputTextflags);
                    changed |= ImGui::InputFloat4(((string)displayName + ".J").c_str(), pFloat4x4 + 4 , "%.2f", imguiInputTextflags);
                    changed |= ImGui::InputFloat4(((string)displayName + ".K").c_str(), pFloat4x4 + 8 , "%.2f", imguiInputTextflags);
                    changed |= ImGui::InputFloat4(((string)displayName + ".T").c_str(), pFloat4x4 + 12, "%.2f", imguiInputTextflags);

                }
                break;

                case IPropertyDescriptor::Type::Float4:
                {
                    float * pFloat4 = (float*)(uint_ptr(_object) + offset);

                    if (asBool(IPropertyDescriptor::Flags::Color & flags))
                        changed |= ImGui::ColorEdit4(displayName, pFloat4);
                    else
                        changed |= ImGui::SliderFloat4(displayName, pFloat4, 0.0f, 1.0f);
                };
                break;

                case IPropertyDescriptor::Type::String:
                {
                    bool selectPath = false;

                    string * pString = (string*)(uint_ptr(_object) + offset);

                    char buffer[1024];
                    sprintf_s(buffer, pString->c_str());
                    changed |= ImGui::InputText(displayName, buffer, countof(buffer), imguiInputTextflags);

                    if (changed)
                    {
                        *pString = buffer;

                        if (asBool(IPropertyDescriptor::Flags::IsFolder & flags))
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

                        if (asBool(IPropertyDescriptor::Flags::IsFolder & flags))
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

                case IPropertyDescriptor::Type::Function:
                {
                    IPropertyDescriptor::Func pFunc = (IPropertyDescriptor::Func)offset;

                    if (ImGui::Button(displayName))
                        pFunc(_object);
                }
                break;

                case IPropertyDescriptor::Type::ObjectVector:
                {
                    const uint sizeOf = prop->getValue();
                    byte * data = nullptr;

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
                    }

                    string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

                    ImGui::PushStyleColor(ImGuiCol_Text, containerColor);

                    auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

                    if (UIMode::Scene == _mode)
                        treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

                    if (count > 0 && ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
                    {
                        for (uint i = 0; i < count; ++i)
                        {
                            IObject * pObject = (IObject * )(data + sizeOf * i);
                            displayArrayObject(pObject, i, objectColor, displayName, _mode);
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopStyleColor();
                }
                break;

                case IPropertyDescriptor::Type::ObjectPointerVector:
                {
                    vector<IObject*> * vec = (vector<IObject*>*)(uint_ptr(_object) + offset);
                    const uint count = (uint)vec->size();

                    string treeNodeName = (string)displayName + " (" + to_string(count) + ")";

                    ImGui::PushStyleColor(ImGuiCol_Text, containerColor);

					auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

					if (UIMode::Scene == _mode)
						treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

                    if (count > 0 && ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags))
                    {
                        for (uint i = 0; i < count; ++i)
                        {
                            IObject * pObject = (*vec)[i];
                            displayArrayObject(pObject, i, objectColor, displayName, _mode);
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopStyleColor();
                }
                break;

                case IPropertyDescriptor::Type::ObjectPointer:
                {
                    IObject * pObject = *(IObject**)(uint_ptr(_object) + offset);

                    string treeNodeName;
                        
                    if (pObject)
                        treeNodeName = pObject->getName();
                    else
                        treeNodeName = displayName;

                    // Only entities & components in scene treeview
                    if (UIMode::Scene == _mode)
                        if (nullptr != pObject && !asBool(pObject->getClassDesc()->getFlags() & (IObjectDescriptor::Flags::Component | IObjectDescriptor::Flags::Entity | IObjectDescriptor::Flags::SceneNode)))
                            continue;

                    ImGui::PushStyleColor(ImGuiCol_Text, objectColor);

					auto treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;

					if (UIMode::Scene == _mode)
						treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

					if (nullptr != pObject && asBool(pObject->getClassDesc()->getFlags() & (IObjectDescriptor::Flags::Component)))
						treeNodeFlags |= ImGuiTreeNodeFlags_Leaf;

                    bool treenNodeOpen = /*UIMode::Object != _mode &&*/ ImGui::TreeNodeEx(treeNodeName.c_str(), treeNodeFlags);

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
                        updateSelection(_object, _mode);

                        if (pObject)
                            displayObject(pObject, _mode);
                        else
                            ImGui::TextDisabled("null");

                        ImGui::TreePop();
                    }
                    else
                    {
                        updateSelection(pObject, _mode);
                    }

                    ImGui::PopStyleColor();
                }
                break;

                case IPropertyDescriptor::Type::Resource:
                {
                    IResource * pResource = (IResource*)(uint_ptr(_object) + offset);
                    IObject * pObject = pResource->getObject();

                    bool open = false;// , save = false;

                    //if (ImGui::CollapsingHeader(displayName, ImGuiTreeNodeFlags_None))
                    {
                        char buffer[1024];
                        sprintf_s(buffer, pResource->getPath().c_str());
                        if (ImGui::InputText(displayName, buffer, countof(buffer), imguiInputTextflags))
                            pResource->setPath(buffer);

                        ImGui::SameLine();
                        if (ImGui::Button("File"))
                            open = true;

                        //ImGui::SameLine();
                        //if (ImGui::Button("Save"))
                        //    save = true;

                        if (pObject)
                            displayObject(pObject);
                    }

                    if (open)
                    {
                        ImGui::OpenPopup("Open file");
                        open = false;
                    }
                    //else if (save)
                    //{
                    //    ImGui::OpenPopup("Save file");
                    //    save = false;
                    //}

                    // build extension list
                    string ext = "";
                    bool first = true;
                    const auto & extensions = pResource->getExtensions();
                    for (uint e = 0; e < extensions.size(); ++e)
                    {
                        if (!first)
                            ext += ";";
                        ext += extensions[e];
                        first = false;
                    }
                    if (ext == "")
                        ext = "*.*";

                    if (file_dialog.showFileDialog("Open file", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ext.c_str()))
                    {
                        const string newFilePath = file_dialog.selected_path;
                        if (pResource->getPath() != newFilePath)
                        {
                            pResource->setPath(newFilePath);
                            changed = true;
                        }
                    }

                    //if (file_dialog.showFileDialog("Save file", imgui_addons::ImGuiFileBrowser::DialogMode::SAVE, ImVec2(700, 310), ".ini"))
                    //    pResource->setPath(file_dialog.selected_path);
                }
                break;
            }

            if (changed)
                _object->onPropertyChanged(*prop);

            previous = type;
        }

        ImGui::PopItemWidth();
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::setCurrentSelection(core::IObject * _object)
    {
        m_selected = _object;
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::updateSelection(core::IObject * _object, UIMode _mode)
    {
        if (_mode == UIMode::Scene)
        {
            //if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                //if (ImGui::IsItemHovered())
            if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
                    setCurrentSelection(_object);
        }
    }

    //--------------------------------------------------------------------------------------
    void ImguiPass::draw(driver::CommandList * _cmdList) const
    {
        Renderer::get()->getImGuiAdapter()->render(_cmdList);
    }
}