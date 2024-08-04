#pragma once

#include "engine/Resource/Material/MaterialResourceData.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiMaterialModelTypeHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object, ObjectContext & _objectContext) final
        {
            if (auto * matModel = dynamic_cast<engine::MaterialModelType *>(_object))
            {
                auto models = getMaterialModels();

                IFactory * factory = Kernel::getFactory();
                const auto * prop = _object->GetClassDesc()->GetPropertyByName("m_shader");
                VG_ASSERT(prop);
                if (nullptr == prop)
                    return;

                if (ImGui::BeginCombo(ImGuiWindow::getPropertyLabel("Shader").c_str(), matModel->m_shader.c_str(), ImGuiComboFlags_HeightLarge))
                {
                    for (uint i = 0; i < models.size(); ++i)
                    {
                        auto model = models[i];
                        if (ImGui::Selectable(model->GetClassDisplayName()))
                        {
                            matModel->m_shader = model->GetClassDisplayName();
                            _object->OnPropertyChanged(_object, *prop);
                        }
                    }
                    ImGui::EndCombo();
                }

                PropertyContext propContext(_object, prop);
                ImGuiWindow::drawPropertyLabel(propContext, prop);
            }
        }

        //--------------------------------------------------------------------------------------
        // Get available MaterialModel from factory
        //--------------------------------------------------------------------------------------
        const vector<IClassDesc *> getMaterialModels() const
        {
            vector<IClassDesc *> matModel;
            
            IFactory * factory = Kernel::getFactory();
            const auto models = factory->getClassDescriptors(IClassDesc::Flags::Model);
            for (uint i = 0; i < models.size(); ++i)
            {
                auto * desc = models[i];
                if (strstr(desc->GetClassName(), "MaterialModel"))
                    matModel.push_back(desc);
            }

            return matModel;
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(MaterialModelType, ImGuiMaterialModelTypeHandler);
}



