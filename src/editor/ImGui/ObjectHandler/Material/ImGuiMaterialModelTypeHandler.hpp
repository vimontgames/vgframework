#pragma once

#include "engine/Resource/Material/MaterialResourceData.h"

using namespace vg::core;

namespace vg::editor
{
    class ImGuiMaterialModelTypeHandler : public ImGuiObjectHandler
    {
    public:
        //--------------------------------------------------------------------------------------
        void displayObject(IObject * _object) final
        {
            if (auto * matModel = dynamic_cast<engine::MaterialModelType *>(_object))
            {
                auto models = getMaterialModels();

                if (ImGui::BeginCombo("Shader", matModel->m_materialModelName.c_str(), ImGuiComboFlags_HeightLarge))
                {
                    for (uint i = 0; i < models.size(); ++i)
                    {
                        auto model = models[i];
                        if (ImGui::Selectable(model->getClassDisplayName()))
                            matModel->m_materialModelName = model->getClassDisplayName();
                    }
                    ImGui::EndCombo();
                }
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
                if (strstr(desc->getClassName(), "MaterialModel"))
                    matModel.push_back(desc);
            }

            return matModel;
        }
    };

    VG_AUTO_REGISTER_IMGUI_OBJECT_HANDLER(MaterialModelType, ImGuiMaterialModelTypeHandler);
}



