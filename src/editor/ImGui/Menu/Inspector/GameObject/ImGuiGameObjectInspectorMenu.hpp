#include "editor/ImGui/Menu/Inspector/GameObject/ImGuiGameObjectInspectorMenu.h"
#include "core/IComponent.h"
#include "core/Math/Math.h"

using namespace vg::core;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    ImGuiMenu::Status ImGuiGameObjectInspectorMenu::Display(IObject * _object)
    {
        auto status = Status::None;

        IGameObject * gameObject = dynamic_cast<IGameObject *>(_object);
        VG_ASSERT(nullptr != gameObject);

        bool openPopup = false;   
        bool addComponent = false;

        if (m_addComponent)
        {
            addComponent = true;
            m_addComponent = false;
        }

        if (addComponent)
        {
            m_selected = (uint)GameObjectInspectorMenuOption::AddComponent;
            m_popup = "Add Component";
            m_popupIcon = style::icon::Plus;
            openPopup = true;
            ImGui::OpenPopup(m_popup.c_str());
            ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        }

        if (openPopup)
        {
            ImGui::OpenPopup(fmt::sprintf("%s %s", style::icon::Plus, m_popup).c_str());
            openPopup = false;
        }

        switch ((GameObjectInspectorMenuOption)m_selected)
        {
            case GameObjectInspectorMenuOption::AddComponent:
            {
                if (ImGui::BeginPopupModal(fmt::sprintf("%s %s", style::icon::Plus, m_popup).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
                {
                    m_addComponentFilter.Draw("Filter");
                    ImGui::SameLine();

                    // List the available components
                    auto factory = Kernel::getFactory();
                    auto componentClassDescs = factory->GetClassDescriptors(ClassDescFlags::Component);

                    // make a list of categories
                    for (uint i = 0; i < componentClassDescs.size(); ++i)
                    {
                        const auto * classDesc = componentClassDescs[i];
                        const char * category = classDesc->GetCategory();
                        bool found = false;
                        for (uint j = 0; j < m_categories.size(); ++j)
                        {
                            if (!strcmp(m_categories[j].name.c_str(),category))
                            {
                                found = true;
                                break;
                            }
                        }
                        if (!found)
                        {
                            auto & newCat = m_categories.emplace_back();
                            newCat.name = category;
                        }
                    }

                    // sort categories in alphabetical order
                    sort(m_categories.begin(), m_categories.end(), [](CategoryInfo & a, CategoryInfo & b)
                    {
                        if (a.name.empty() == b.name.empty())
                            return a.name < b.name;
                        else if (a.name.empty())
                            return true;
                        else
                            return false;
                    }
                    );

                    // add to each category
                    for (uint i = 0; i < m_categories.size(); ++i)
                    {
                        CategoryInfo & cat = m_categories[i];
                        for (uint j = 0; j < componentClassDescs.size(); ++j)
                        {
                            auto * classDesc = componentClassDescs[j];
                            const char * category = classDesc->GetCategory();

                            if (!strcmp(category, cat.name.c_str()))
                            {
                                if (!vector_helper::exists(cat.classes, classDesc))
                                    cat.classes.push_back(classDesc);
                            }
                        }

                        // sort classes in alphabetical order
                        sort(cat.classes.begin(), cat.classes.end(), [](IClassDesc * _a, IClassDesc * _b)
                            {
                                string a = _a->GetClassName();
                                string b = _b->GetClassName();
                                if (a.empty() == b.empty())
                                    return a < b;
                                else if (a.empty())
                                    return true;
                                else
                                    return false;
                            }
                        );
                    }

                    string preview = "";
                    bool first = true;
                    for (int i = 0; i < m_categories.size(); i++)
                    {
                        auto & cat = m_categories[i];
                        if (cat.show)
                        {
                            if (!first)
                                preview += " | ";
                            preview += cat.name;
                            first = false;
                        }
                    }

                    if (ImGui::BeginCombo("Categories", preview.c_str()))
                    {
                        for (int i = 0; i < m_categories.size(); i++)
                        {
                            auto & cat = m_categories[i];
                            bool selected = cat.show;
                            if (ImGui::Selectable(cat.name.c_str(), &selected))
                            {
                                bool everythingIsSelected = true;
                                for (int j = 0; j < m_categories.size(); j++)
                                {
                                    auto & cat2 = m_categories[j];
                                    if (!cat2.show)
                                    {
                                        everythingIsSelected = false;
                                        break;
                                    }
                                }

                                cat.show = selected;

                                if (!ImGui::GetIO().KeyCtrl)
                                {
                                    if (everythingIsSelected)
                                        cat.show = true;
   
                                    // Unselect others unless ctrl is pressed
                                    for (int j = 0; j < m_categories.size(); j++)
                                    {
                                        if (j != i)
                                            m_categories[j].show = false;
                                    }
                                }
                            }

                            // Default select first visible, unselected if hidden
                            if (m_selectedClass == nullptr)
                            {
                                if (cat.show)
                                {
                                    if (cat.classes.size() > 0)
                                        m_selectedClass = cat.classes[0];
                                }
                            }
                            else if (!cat.show && vector_helper::exists(cat.classes, (IClassDesc *)m_selectedClass))
                            {
                                m_selectedClass = nullptr;
                            }
                        }

                        ImGui::EndCombo();
                    }

                    bool doCreate = false;

                    ImGui::BeginChild("TableContainer", ImVec2(0, 512), true);
                    {
                        if (ImGui::BeginTable("Components", 2, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY))
                        {
                            ImGui::TableSetupColumn("Name");
                            ImGui::TableSetupColumn("Category");
                            ImGui::TableSetupScrollFreeze(0, 1);
                            ImGui::TableHeadersRow();

                            // Sort our data if sort specs have been changed!
                            if (ImGuiTableSortSpecs * sort_specs = ImGui::TableGetSortSpecs())
                            {
                                if (sort_specs->Specs->ColumnIndex == 0)
                                {
                                    sort(componentClassDescs.begin(), componentClassDescs.end(), [](IClassDesc * a, IClassDesc * b)
                                    {
                                        return (string)a->GetClassDisplayName() < (string)b->GetClassDisplayName();
                                    }
                                    );
                                }
                                else if (sort_specs->Specs->ColumnIndex == 1)
                                {
                                    sort(componentClassDescs.begin(), componentClassDescs.end(), [](IClassDesc * a, IClassDesc * b)
                                    {
                                        return (string)a->GetCategory() < (string)b->GetCategory();
                                    }
                                    );
                                }
                            }

                            for (int n = 0; n < componentClassDescs.size(); n++)
                            {
                                const auto & classDesc = componentClassDescs[n];

                                if (!m_addComponentFilter.PassFilter(classDesc->GetClassDisplayName()) && !m_addComponentFilter.PassFilter(classDesc->GetCategory()))
                                    continue;

                                bool hideCategory = false;
                                for (int c = 0; c < m_categories.size(); ++c)
                                {
                                    if ((string)classDesc->GetCategory() == m_categories[c].name)
                                    {
                                        if (!m_categories[c].show)
                                        {
                                            hideCategory = true;
                                            break;
                                        }
                                    }
                                }

                                if (hideCategory)
                                    continue;

                                ImGui::TableNextRow();
                                ImGui::TableNextColumn();
                                const bool isSelected = m_selectedClass == classDesc;
                                ImGui::SetNextItemSelectionUserData(n);
                                string displayName = fmt::sprintf("%s %s", classDesc->GetIcon(), classDesc->GetClassDisplayName());
                                
                                if (ImGui::Selectable(displayName.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap | ImGuiSelectableFlags_DontClosePopups))
                                {
                                    m_selectedClass = classDesc;
                                }    

                                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                                    doCreate = true;

                                if (isSelected)
                                    ImGui::SetItemDefaultFocus();

                                if (ImGui::IsItemHovered())
                                {
                                    const char * description = classDesc->GetDescription();
                                    string desc = description && strlen(description) > 0 ? description : "<no description available>";
                                    ImGui::SetTooltip(desc.c_str());
                                }

                                ImGui::TableNextColumn();
                                ImGui::Text(classDesc->GetCategory());
                            }

                            ImGui::EndTable();
                        }
                    }
                    ImGui::EndChild();
                    ImGui::Spacing();

                    ImGui::BeginDisabled(m_selectedClass == nullptr);
                    {
                        if (ImGui::Button("Create", style::button::SizeMedium) || doCreate)
                        {
                            status = Status::Failure;

                            if (m_selectedClass)
                            {
                                IComponent * comp = (IComponent *)Kernel::getFactory()->CreateObject(m_selectedClass->GetClassName(), "New " + (string)m_selectedClass->GetClassName(), _object);
                                if (comp)
                                {
                                    comp->SetObjectFlags(ObjectFlags::Opened, true);
                                    comp->RegisterUID();
                                    gameObject->AddComponent(comp);
                                    comp->Release();
                                    status = Status::Success;
                                }
                            }

                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::EndDisabled();

                    ImGui::SameLine();

                    if (ImGui::Button("Cancel", style::button::SizeMedium))
                    {
                        status = Status::Canceled;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }
            break;
        }

        return status;
    }
}