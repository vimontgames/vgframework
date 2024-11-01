#pragma once

#include "engine/Resource/BaseScene/BaseSceneResource.h"

namespace vg::engine
{
    class PrefabResource : public BaseSceneResource
    {
    public:
        VG_CLASS_DECL(PrefabResource, BaseSceneResource);

        PrefabResource(const core::string& _name = "", IObject* _parent = nullptr);
        ~PrefabResource();

        const core::vector<core::string>    GetExtensions               () const final;
        core::IObject*                      Load                        (const core::string & _path) final override;
        bool                                CreateFile                  (const core::string & _path, core::IObject * _data = nullptr) override;
        bool                                SaveFile                    (const core::string & _path) const override;
    };
}