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

        const core::vector<core::string>    getExtensions               () const final;
        core::IObject*                      load                        (const core::string & _path) final override;
        bool                                CreateFile                  (const core::string & _path);
        bool                                SaveFile                    (const core::string & _path) const override;

    private:
        static void *                       ResizePrefabResourceVector  (core::IObject* _parent, core::uint _offset, core::uint _count, core::uint & _elementSize);
    };
}