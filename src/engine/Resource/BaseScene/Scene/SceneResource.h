#pragma once

#include "engine/Resource/BaseScene/BaseSceneResource.h"

namespace vg::engine
{
	class SceneResource : public BaseSceneResource
	{
	public:
		VG_CLASS_DECL(SceneResource, BaseSceneResource);

		SceneResource(const core::string & _name = "", IObject * _parent = nullptr);
		~SceneResource();

		const core::vector<core::string>    GetExtensions           () const final;
		core::LoadStatus                    Load                    (const core::string & _path, core::IObject *& _object) final override;
		bool                                CreateFile              (const core::string & _path, core::IObject * _data = nullptr) final override;
		bool                                SaveFile                (const core::string & _path) const override;
	};
}