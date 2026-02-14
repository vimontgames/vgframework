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

    //--------------------------------------------------------------------------------------
    string formatTimestamp(io::FileAccessTime _timestamp)
    {
        if (_timestamp == 0)
            return "";

        FILETIME ft;
        ft.dwHighDateTime = (DWORD)(_timestamp >> 32);
        ft.dwLowDateTime = (DWORD)(_timestamp & 0xFFFFFFFFULL);

        SYSTEMTIME stUTC;
        if (!FileTimeToSystemTime(&ft, &stUTC)) return "<invalid>";

        SYSTEMTIME stLocal;
        if (!SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal)) return "<invalid>";

        const char * months[12] = { "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec" };
        return fmt::sprintf("%02d %s %02d %02d:%02d:%02d",
            stLocal.wDay,
            months[stLocal.wMonth - 1],
            stLocal.wYear % 100,
            stLocal.wHour,
            stLocal.wMinute,
            stLocal.wSecond);
    }

    vg_enum_class(vg::editor, Column, core::uint,
        Name,
        Extension,
        Type,
        LastCook,
        CookedSize,
        CookingTime,
        LoadingTime,
        LastModified,
        RawSize,
        Folder,
        Clients
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
                    const char * name = nullptr;
                    ImGuiTableColumnFlags flags = 0x0;
                    const char * tooltip = nullptr;
                };

                ColumnDesc columnDescs[enumCount<Column>()];

                columnDescs[asInteger(Column::Type)]        = { "Type",        ImGuiTableColumnFlags_WidthFixed,   "Resource type" };
                columnDescs[asInteger(Column::Name)]        = { "Name",        ImGuiTableColumnFlags_WidthStretch, "Resource name" };
                columnDescs[asInteger(Column::Extension)]   = { "Extension",   ImGuiTableColumnFlags_WidthFixed,   "Extension type" };
                columnDescs[asInteger(Column::Folder)]      = { "Path",        ImGuiTableColumnFlags_WidthStretch, "Where the file is located in the \"Data\" folder" };
                columnDescs[asInteger(Column::LoadingTime)] = { "Load time",   ImGuiTableColumnFlags_WidthFixed,   "Time to load the resource (ms)" };
                columnDescs[asInteger(Column::RawSize)]     = { "Raw size",    ImGuiTableColumnFlags_WidthFixed,   "Size of the raw data file" };
                columnDescs[asInteger(Column::LastModified)]= { "Modified",    ImGuiTableColumnFlags_WidthFixed,   "Last time the resource source was modified" };
                columnDescs[asInteger(Column::CookedSize)]  = { "Cooked size", ImGuiTableColumnFlags_WidthFixed,   "Size of the cooked data file" };
                columnDescs[asInteger(Column::CookingTime)] = { "Cook time",   ImGuiTableColumnFlags_WidthFixed,   "Time to cook the resource (ms)" };
                columnDescs[asInteger(Column::LastCook)]    = { "Cooked",      ImGuiTableColumnFlags_WidthFixed,   "Last time the resource was cooked" };
                columnDescs[asInteger(Column::Clients)]     = { "Clients",     ImGuiTableColumnFlags_WidthFixed,   "Resource Clients" };

                const uint columnCount = (uint)countof(columnDescs);
                const ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable;

                if (ImGui::BeginTable("resources_table", columnCount, flags))
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
                        VG_ASSERT(desc.name, "Column '%s' has no name", asCString((Column)column));
                        ImGui::TableHeader(desc.name);

                        if (desc.tooltip && ImGui::IsItemHovered())
                            ImGui::SetTooltip(desc.tooltip);
                        
                    }
                    ImGuiTableSortSpecs * sortSpecs = ImGui::TableGetSortSpecs();
                    if (sortSpecs && sortSpecs->SpecsDirty && sortSpecs->SpecsCount > 0)
                    {
                        VG_PROFILE_CPU("Sort");

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
                                        return a->GetCookingDuration() < b->GetCookingDuration();
                                    else
                                        return a->GetCookingDuration() > b->GetCookingDuration();
                                }
                            );
                            break;

                            case Column::LoadingTime:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b) 
                                { 
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetLoadingDuration() < b->GetLoadingDuration();
                                    else
                                        return a->GetLoadingDuration() > b->GetLoadingDuration();
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

                            case Column::LastCook:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b)
                                {
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetLastCookDate() < b->GetLastCookDate();
                                    else
                                        return a->GetLastCookDate() > b->GetLastCookDate();
                                }
                            );
                            break;     

                            case Column::LastModified:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b)
                                {
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetLastModifiedDate() < b->GetLastModifiedDate();
                                    else
                                        return a->GetLastModifiedDate() > b->GetLastModifiedDate();
                                }
                            );
                            break;   

                            case Column::Clients:
                            std::sort(resourceInfos.begin(), resourceInfos.end(), [=](const IResourceInfo * a, const IResourceInfo * b)
                                {
                                    if (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending)
                                        return a->GetClientCount() < b->GetClientCount();
                                    else
                                        return a->GetClientCount() > b->GetClientCount();
                                }
                            );
                            break;   
                        }
                    }

                    // Draw lines
                    {
                        VG_PROFILE_CPU("Draw");

                        ImGuiListClipper clipper;
                        clipper.Begin((int)resourceInfos.size());
                        while (clipper.Step())
                        {
                            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
                            {
                                ImGui::TableNextRow(); // MUST be inside the inner loop
                                const IResourceInfo * resInfo = resourceInfos[row];
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
                                                ImGui::Text("%s", formatSizeInBytes(fileSize).c_str());
                                            else
                                                ImGui::Text("N/A");
                                        }
                                        break;
                                
                                        case Column::CookedSize:
                                        {
                                            const auto fileSize = resInfo->GetCookedFileSize();
                                            if (fileSize > 0)
                                                ImGui::Text("%s", formatSizeInBytes(fileSize).c_str());
                                            else
                                                ImGui::Text("N/A");
                                        }
                                        break;
                                
                                        case Column::CookingTime:
                                        {
                                            const float cookingTime = resInfo->GetCookingDuration();
                                            if (cookingTime > 0.0f)
                                                ImGui::Text("%s", formatTimeInMilliseconds(resInfo->GetCookingDuration()).c_str());
                                            else
                                                ImGui::Text("N/A");
                                        }
                                        break;
                                
                                        case Column::LoadingTime:
                                            ImGui::Text("%s", formatTimeInMilliseconds(resInfo->GetLoadingDuration()).c_str());
                                            break;
                                
                                
                                        case Column::Folder:
                                            ImGui::Text(resInfo->GetFolder().c_str());
                                            break;

                                        case Column::LastCook:
                                            ImGui::Text(formatTimestamp(resInfo->GetLastCookDate()).c_str());
                                            break;

                                        case Column::LastModified:
                                            ImGui::Text(formatTimestamp(resInfo->GetLastModifiedDate()).c_str());
                                            break;

                                        case Column::Clients:
                                            ImGui::Text("%u", resInfo->GetClientCount());
                                            break;
                                    }
                                }
                            }
                        }
                    }
                    ImGui::EndTable();
                }
            }

            rm->Unlock();
        }  
        ImGui::End();          
    }
}