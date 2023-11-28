#pragma once

#include "renderer/IGraphicInstance.h"

namespace vg::renderer
{
    class MaterialModel;

    class GraphicInstance : public IGraphicInstance
    {
    public:
        VG_CLASS_DECL(GraphicInstance, IGraphicInstance);

        GraphicInstance(const core::string & _name, core::IObject * _parent);
        ~GraphicInstance();

        virtual bool                    SetMaterial     (core::uint _index, IMaterialModel * _materialModel) override;
        virtual IMaterialModel *        GetMaterial     (core::uint _index) const override;

        // internal
        bool                            setMaterial     (core::uint _index, MaterialModel * _materialModel);
        MaterialModel *                 getMaterial     (core::uint _index) const;

        void                            ClearPickingID  () override;
        void                            SetPickingID    (PickingID _id) override;
        PickingID                       GetPickingID    () const override;

    private:
        PickingID                       m_pickingID;
        core::vector<MaterialModel *>   m_materials;
    };
}