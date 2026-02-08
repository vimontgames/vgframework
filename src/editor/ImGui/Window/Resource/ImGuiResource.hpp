#include "ImGuiResource.h"
#include "core/IResourceManager.h"

using namespace vg::core;
using namespace vg::engine;

namespace vg::editor
{
    //--------------------------------------------------------------------------------------
    string formatSizeInBytes(u64 _sizeInBytes)
    {
        const float KB = 1024.0f;
        const float MB = KB * 1024.0f;
        const float GB = MB * 1024.0f;

        if (_sizeInBytes < 1024)
            return fmt::sprintf("%lld B", _sizeInBytes);
        else if (_sizeInBytes < MB)
            return fmt::sprintf("%.1f kB", _sizeInBytes / KB);
        else if (_sizeInBytes < GB)
            return fmt::sprintf("%.2f MB", _sizeInBytes / MB);
        else
            return fmt::sprintf("%.3f GB", _sizeInBytes / GB);
    }

    //--------------------------------------------------------------------------------------
    string formatTimeInMilliseconds(float _timeInMilliseconds)
    {
        if (_timeInMilliseconds < 1000)
            return fmt::sprintf("%.0f ms", _timeInMilliseconds);

        float seconds = _timeInMilliseconds / 1000.0f;
        if (seconds < 60)
            return fmt::sprintf("%.1f s", seconds);

        float minutes = seconds / 60.0f;
        return fmt::sprintf("%.2f m", minutes);
    }

    vg_enum_class(vg::editor, Column, core::uint,
        Name = 0,
        Extension,
        Type,
        RawSize,
        CookedSize,
        CookingTime,
        LoadingTime,
        Folder
    );

    //--------------------------------------------------------------------------------------
    ImGuiResource::ImGuiResource() :
        ImGuiWindow(style::icon::Resource, "", "Resources", ImGuiWindow::StartVisible | ImGuiWindow::AddMenuEntry)
    {
    }

    //--------------------------------------------------------------------------------------
    void ImGuiResource::DrawGUI()
    {
        if (ImGui::IconBegin(style::icon::Resource, GetName().c_str(), &m_isVisible))
        {
            IResourceManager * rm = Editor::get()->getEngine()->GetResourceManager();

            const uint loadingCount = rm->GetLoadingInProgressCount();
            const bool enabled = (loadingCount == 0);

            if (enabled)
            {
                if (ImGui::TooltipButton(fmt::sprintf("%s Reload Resources", style::icon::Reload).c_str(), true, true, "Reload Resources (F7)", style::button::SizeLarge))
                    rm->UpdateResources();
            }
            else
            {
                ImGui::TooltipButton(fmt::sprintf("%s %u Loading ...", style::icon::Reload, loadingCount).c_str(), false, false, "Reload Resources (F7)", style::button::SizeLarge);
            }

            ImGui::Separator();

            rm->Lock();

            if (1)
            {
                const auto & resources = rm->GetAllResourceInfos();
                uint resIndex = 0;
                vector<const IResourceInfo *> resourceInfos(resources.size());
                for (auto & resInfoPair : resources)
                {
                    const IResourceInfo * resInfo = resInfoPair.second;
                    resourceInfos[resIndex++] = resInfo;
                }

                // table
                struct ColumnDesc
                {
                    const char * name;
                    ImGuiTableColumnFlags flags;
                    const char * tooltip;
                };

                const ColumnDesc columnDescs[] = 
                {
                    { "Name", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort, "Resource name"},
                    { "Extension", ImGuiTableColumnFlags_WidthFixed, "Extension type"},
                    { "Type", ImGuiTableColumnFlags_WidthFixed, "Resource type"},
                    { "Raw size", ImGuiTableColumnFlags_WidthFixed, "Size of the raw data file"},
                    { "Cooked size", ImGuiTableColumnFlags_WidthFixed, "Size of the cooked data file"},
                    { "Cook time", ImGuiTableColumnFlags_WidthFixed, "Time to cook the resource (ms)"},
                    { "Load time", ImGuiTableColumnFlags_WidthFixed, "Time to load the resource (ms)"},
                    { "Data folder", ImGuiTableColumnFlags_WidthStretch, "Where the file is located in the \"Data\" folder"}
                };
                VG_STATIC_ASSERT(countof(columnDescs) == enumCount<Column>(), "invalid size for columnDescs");

                const uint columnCount = (uint)countof(columnDescs);
                const ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable;
                
                ImGui::BeginChild("resources_table_child", ImVec2(0, 0));
                if (ImGui::BeginTable("resources_table", columnCount, ImGuiTableFlags_Sortable | ImGuiTableFlags_RowBg))
                {
                    ImGui::TableSetupScrollFreeze(0, 1); // freeze 1 top row

                    for (int column = 0; column < columnCount; column++)
                    {
                        const auto & desc = columnDescs[column];
                        ImGui::TableSetupColumn(desc.name, desc.flags);
                    }

                    TableNextRow(ImGuiTableRowFlags_Headers);
                    for (int column = 0; column < columnCount; column++)
                    {
                        const auto & desc = columnDescs[column];
                        ImGui::TableSetColumnIndex(column);
                        ImGui::TableHeader(desc.name);

                        if (ImGui::IsItemHovered())
                            ImGui::SetTooltip(desc.tooltip);
                        
                    }

                    ImGui::TableNextRow();

                    ImGuiTableSortSpecs * sortSpecs = ImGui::TableGetSortSpecs();
                    if (sortSpecs && sortSpecs->SpecsDirty && sortSpecs->SpecsCount > 0)
                    {
                        const ImGuiTableColumnSortSpecs & spec = sortSpecs->Specs[0];
                        const Column column = (Column)spec.ColumnIndex;
                        switch (column)
                        {
                            default:
                                VG_ASSERT_ENUM_NOT_IMPLEMENTED(column);
                                break;

                            case Column::Name:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b) 
                                { 
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetFilename() < b->GetFilename();
                                    else
                                        return a->GetFilename() > b->GetFilename();
                                }
                            );
                            break;

                            case Column::Extension:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b) 
                                { 
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetExtension() < b->GetExtension();
                                    else
                                        return a->GetExtension() > b->GetExtension();
                                }
                            );
                            break;

                            case Column::Type:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b) 
                                { 
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetResourceType() < b->GetResourceType();
                                    else
                                        return a->GetResourceType() > b->GetResourceType();
                                }
                            );
                            break;

                            case Column::RawSize:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b) 
                                { 
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetRawFileSize() < b->GetRawFileSize();
                                    else
                                        return a->GetRawFileSize() > b->GetRawFileSize();
                                }
                            );
                            break;

                            case Column::CookedSize:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b) 
                                { 
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetCookedFileSize() < b->GetCookedFileSize();
                                    else
                                        return a->GetCookedFileSize() > b->GetCookedFileSize();
                                }
                            );
                            break;

                            case Column::CookingTime:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b) 
                                { 
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetCookingTime() < b->GetCookingTime();
                                    else
                                        return a->GetCookingTime() > b->GetCookingTime();
                                }
                            );
                            break;

                            case Column::LoadingTime:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b) 
                                { 
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetLoadingTime() < b->GetLoadingTime();
                                    else
                                        return a->GetLoadingTime() > b->GetLoadingTime();
                                }
                            );
                            break;

                            case Column::Folder:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b)
                                {
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetFolder() < b->GetFolder();
                                    else
                                        return a->GetFolder() > b->GetFolder();
                                }
                            );
                            break;
                        }
                    }

                    for (const IResourceInfo * resInfo : resourceInfos)
                    {
                        for (uint i = 0; i < enumCount<Column>(); ++i)
                        {
                            ImGui::TableSetColumnIndex(i);
                            const Column column = (Column)i;
                            switch (column)
                            {
                                default:
                                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(column);
                                    break;

                                case Column::Name:
                                    ImGui::Text(resInfo->GetFilename().c_str());
                                    break;

                                case Column::Extension:
                                    ImGui::Text(".%s", resInfo->GetExtension().c_str());
                                    break;

                                case Column::Type:
                                {
                                    string resType = resInfo->GetResourceType();
                                    const string resSuffix = "Resource";
                                    auto lastResString = resType.find_last_of(resSuffix);
                                    if (-1 != lastResString)
                                        resType = resType.substr(0, resType.length() - resSuffix.length());
                                    ImGui::Text(resType.c_str());
                                }
                                break;

                                case Column::RawSize:
                                {
                                    const auto fileSize = resInfo->GetRawFileSize();
                                    if (fileSize > 0)
                                        ImGui::Text("%s", formatSizeInBytes(fileSize));
                                    else
                                        ImGui::Text("N/A");
                                }
                                break;

                                case Column::CookedSize:
                                {
                                    const auto fileSize = resInfo->GetCookedFileSize();
                                    if (fileSize > 0)
                                        ImGui::Text("%s", formatSizeInBytes(fileSize));
                                    else
                                        ImGui::Text("N/A");
                                }
                                break;
              
                                case Column::CookingTime:
                                {
                                    const float cookingTime = resInfo->GetCookingTime();
                                    if (cookingTime > 0.0f)
                                        ImGui::Text("%s", formatTimeInMilliseconds(resInfo->GetCookingTime()));
                                    else
                                        ImGui::Text("N/A");
                                }
                                break;

                                case Column::LoadingTime:
                                    ImGui::Text("%s", formatTimeInMilliseconds(resInfo->GetLoadingTime()));
                                    break;


                                case Column::Folder:
                                    ImGui::Text(resInfo->GetFolder().c_str());
                                    break;
                            }
                        }
                        ImGui::TableNextRow();
                    }
                    ImGui::EndTable();
                }
                ImGui::EndChild();
            }

            rm->Unlock();
        }  
        ImGui::End();          
    }
}