#include "core/Curve/FloatNCurve.h"
#include "core/Math/Math.h"
#include "editor/Editor_Consts.h"
#include "editor/ImGui/Extensions/ImGuizmo/ImGuizmoAdapter.h"

using namespace vg::core;

namespace vg::editor
{
    class FloatCurveEdit final : public ImCurveEdit::Delegate
    {
    public:
        FloatCurveEdit(FloatNCurve * _curve) :
            m_curve(_curve)
        {

        }

        size_t GetCurveCount() final override
        {
            return m_curve ? m_curve->getCurveCount() : 0;
        }

        bool IsVisible(size_t _curveIndex) final override
        {
            return m_curve->IsVisible((uint)_curveIndex);
        }

        size_t GetPointCount(size_t _curveIndex) final override
        {
            return m_curve->getCurveData((uint)_curveIndex).m_points.size();
        }

        uint32_t GetCurveColor(size_t _curveIndex) final override
        {
            if (m_curve->GetCurveCount() == 1)
            {
                return 0xFFCFCFCF;
            }
            else
            {
                switch (_curveIndex)
                {
                    case 0:
                        return 0xFF0000FF;

                    case 1:
                        return 0xFF00FF00;

                    case 2:
                        return 0xFFFF0000;

                    default:
                        return 0xFFCFCFCF;
                }
            }
        }

        ImVec2 * GetPoints(size_t _curveIndex) final override
        {
            return (ImVec2*)m_curve->getCurveData((uint)_curveIndex).m_points.data();
        }

        ImCurveEdit::CurveType GetCurveType(size_t _curveIndex) const final override
        { 
            const auto type = m_curve->getInterpolationType();
            switch (type)
            {
                default:
                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(type);

                case CurveInterpolationType::Constant:
                    return ImCurveEdit::CurveNone;

                case CurveInterpolationType::Linear:
                    return ImCurveEdit::CurveLinear;

                case CurveInterpolationType::Smooth:
                    return ImCurveEdit::CurveSmooth;
            }
        }

        int EditPoint(size_t _curveIndex, int _pointIndex, ImVec2 _value) final override
        {
            auto & points = m_curve->getCurveData((uint)_curveIndex).m_points;

            if (points.size() < _pointIndex)
                points.resize(_pointIndex + 1);

            points[_pointIndex] = { _value.x, _value.y };

            if (GetCurveType(_curveIndex) == ImCurveEdit::CurveType::CurveNone)
            {
                for (size_t i = 0; i < points.size(); i++)
                {
                    if (i != _pointIndex)
                        points[i].value = _value.y;
                }
            }

            const float yMin = m_curve->getVerticalRange().x;
            const float yMax = m_curve->getVerticalRange().y;
            for (size_t i = 0; i < points.size(); i++)
            {
                points[i].value = clamp(points[i].value, yMin, yMax);
            }

            SortValues(_curveIndex);
            for (size_t i = 0; i < points.size(); i++)
            {
                if (points[i].t == _value.x)
                    return (int)i;
            }
            return _pointIndex;
        }

        void AddPoint(size_t _curveIndex, ImVec2 _value) final override
        {
            auto & points = m_curve->getCurveData((uint)_curveIndex).m_points;

            points.resize(points.size() + 1);
            points[points.size()-1] = { _value.x, _value.y };

            SortValues(_curveIndex);
        }

        void RemovePoint(size_t _curveIndex, int _pointIndex) final override
        {
            auto & points = m_curve->getCurveData((uint)_curveIndex).m_points;

            if (points.size() > 2)
                points.erase(points.begin() + _pointIndex);

            SortValues(_curveIndex);
        }

        ImVec2 GetLowerBounds() const final override
        {
            return ImVec2(m_curve->getHorizontalRange().x, m_curve->getVerticalRange().x);
        }

        ImVec2 GetUpperBounds() const final override
        {
            return ImVec2(m_curve->getHorizontalRange().y, m_curve->getVerticalRange().y);
        }

        ImVec2 & GetMin() final override
        {
            return (ImVec2 &)m_curve->getDisplayMin();
        }

        ImVec2 & GetMax()  final override
        {
            return (ImVec2 & )m_curve->getDisplayMax();
        }

    private:
        void SortValues(size_t _curveIndex)
        {
            auto & points = m_curve->getCurveData((uint)_curveIndex).m_points;
            sort(points.begin(), points.end(), [](FloatCurveData::FloatCurvePoint a, FloatCurveData::FloatCurvePoint b) { return a.t < b.t; });
        }

    private:
        FloatNCurve * m_curve = nullptr;
    };

    class ImGuiCurveObjectHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        bool displayObject(IObject * _object, ObjectContext & _objectContext, const PropertyContext * _propContext) final override
        {
            bool changed = false;

            auto * curve = dynamic_cast<core::Curve *>(_object);

            const auto * factory = Kernel::getFactory();
            const auto * classDesc = factory->GetClassDescriptor(curve->GetClassName());

            ImVec2 collapsingHeaderPos = ImGui::GetCursorPos();
            string curveLabel = fmt::sprintf("%s %s", editor::style::icon::Curve, _propContext->m_originalProp->GetDisplayName());
            auto * imGuiAdapter = ImGuiWindow::getImGuiAdapter();

            const bool useCollapsingHeader = !asBool(PropertyFlags::Flatten & _propContext->m_originalProp->GetFlags());

            bool open = !useCollapsingHeader || ImGui::CollapsingHeader(ImGui::getObjectLabel(curveLabel, curve).c_str(),  ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

            if (open)
            {
                ImGui::Indent();

                auto availableWidth = ImGui::GetContentRegionAvail().x;

                for (uint i = 0; i < classDesc->GetPropertyCount(); ++i)
                {
                    const IProperty * prop = classDesc->GetPropertyByIndex(i);

                    if (strstr(prop->GetName(), "m_data"))
                    {
                        if (!strcmp(prop->GetClassName(), "FloatNCurve"))
                        {
                            ImGui::PushID(curve);

                            FloatCurveEdit floatCurveEdit((FloatNCurve*)curve);
                            
                            if (ImGui::Button(ImGui::getObjectLabel(editor::style::icon::ResetRange, curve).c_str(), style::button::SizeSmall))
                                curve->ResetBounds();

                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("Reset to show full curve range");

                            ImGui::SameLine();

                            if (ImGui::Button(ImGui::getObjectLabel(editor::style::icon::FitVertical, curve).c_str(), style::button::SizeSmall))
                                curve->FitVerticalBounds();

                            if (ImGui::IsItemHovered())
                                ImGui::SetTooltip("Fit curve vertically");

                            ImGui::SameLine();

                            ImGui::Button(ImGui::getObjectLabel(editor::style::icon::About, curve).c_str(), style::button::SizeSmall);

                            if (ImGui::IsItemHovered())
                            {
                                ImGui::BeginTooltip();

                                if (ImGui::BeginTable("Table1", 2))
                                {
                                    ImGui::TableSetupColumn("Header1");
                                    ImGui::TableSetupColumn("Header2");

                                    ImGui::TableNextRow();

                                    ImGui::TableSetColumnIndex(0);
                                    ImGui::Text("Pan");
                                    ImGui::Text("Zoom");
                                    ImGui::Text("Vertical Zoom");
                                    ImGui::Text("Horizontal Zoom");
                                    ImGui::Text("Add point");
                                    ImGui::Text("Remove point");

                                    ImGui::TableSetColumnIndex(1);
                                    imGuiAdapter->PushFontStyle(renderer::FontStyle::Italic);
                                    ImGui::Text("Middle mouse button");
                                    ImGui::Text("Ctrl + Mouse wheel");
                                    ImGui::Text("Ctrl + Mouse wheel + Shift");
                                    ImGui::Text("Ctrl + Mouse wheel + Alt");
                                    ImGui::Text("Double-click left mouse button");
                                    ImGui::Text("Alt + Left mouse button");
                                    imGuiAdapter->PopFontStyle();
                                    ImGui::EndTable();
                                }

                                ImGui::EndTooltip();
                            }

                            ImGui::SameLine();

                            FloatNCurve * floatCurve = (FloatNCurve *)curve;

                            float available = ImGui::GetContentRegionAvail().x;

                            const char * curveShortNames[4];
                            const char * curveLongNames[4];

                            switch (curve->GetCurveValueType())
                            {
                                default:
                                    VG_ASSERT_ENUM_NOT_IMPLEMENTED(curve->GetCurveValueType());

                                case CurveValueType::None:
                                {
                                    curveShortNames[0] = "X";
                                    curveShortNames[1] = "Y";
                                    curveShortNames[2] = "Z";
                                    curveShortNames[3] = "W";

                                    curveLongNames[0] = "X";
                                    curveLongNames[1] = "Y";
                                    curveLongNames[2] = "Z";
                                    curveLongNames[3] = "W";
                                }
                                break;

                                case CurveValueType::Color:
                                {
                                    curveShortNames[0] = "R";
                                    curveShortNames[1] = "G";
                                    curveShortNames[2] = "B";
                                    curveShortNames[3] = "A";

                                    curveLongNames[0] = "Red";
                                    curveLongNames[1] = "Green";
                                    curveLongNames[2] = "Blue";
                                    curveLongNames[3] = "Alpha";
                                }
                                break;
                            }

                            float cbWidth = style::button::SizeSmall.x + ImGui::GetStyle().FramePadding.x; // ImGui::CalcTextSize("R").x + ImGui::GetStyle().FramePadding.x * 2;
                            const auto posX = ImGui::GetCursorPosX();
                            const auto maxCurves = curve->CanChangeCurveCount() ? 4 : curve->GetCurveCount();
                            for (uint c = 0; c < maxCurves; ++c)
                            {
                                bool clickable = c < floatCurve->getCurveCount();
                                bool enabled = clickable && floatCurve->IsVisible(c);

                                bool disabled = c >= floatCurve->getCurveCount();

                                if (disabled && !curve->CanChangeCurveCount())
                                    continue;

                                ImGui::SameLine();

                                ImGui::SetCursorPosX(posX + available - (maxCurves - c) * cbWidth);

                                ImGui::BeginDisabled(disabled);
                                {
                                    if (ImGui::TooltipButton(ImGui::getObjectLabel(curveShortNames[c], curve).c_str(), enabled, clickable, fmt::sprintf("%s %s curve", enabled? "Hide" : "Show", curveLongNames[c]), style::button::SizeSmall))
                                    {
                                        floatCurve->SetVisible(c, !floatCurve->IsVisible(c));
                                        changed = true;
                                    }
                                }
                                ImGui::EndDisabled();
                            }

                            ImRect rc;
                            rc.Min = ImGui::GetCursorPos();
                            rc.Max = rc.Min + ImVec2(availableWidth, availableWidth);

                            ImRect clip;
                            clip.Min = ImGui::GetCursorScreenPos();
                            clip.Max = clip.Min + ImVec2(availableWidth, availableWidth);

                            changed = 0 != ImCurveEdit::Edit(floatCurveEdit, rc.Max - rc.Min, (uint)(uint_ptr)curve, &clip);

                            ImGui::PopID();
                        }
                        else
                        {
                            ImGui::Text(fmt::sprintf("%s type not implemented !", prop->GetClassName()).c_str());
                        }
                    }
                    else
                    {
                        changed |= ImGuiWindow::displayProperty(curve, prop);
                    }
                }
                ImGui::Unindent();
            }


            return changed;
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(FloatNCurve, ImGuiCurveObjectHandler);
    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Float1Curve, ImGuiCurveObjectHandler);
    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Float2Curve, ImGuiCurveObjectHandler);
    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Float3Curve, ImGuiCurveObjectHandler);
    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(Float4Curve, ImGuiCurveObjectHandler);
}