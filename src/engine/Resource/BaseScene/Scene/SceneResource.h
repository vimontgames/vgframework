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

		const core::vector<core::string>    getExtensions				() const final;
		core::IObject *						load						(const core::string & _path) final override;
		bool                                CreateFile					(const core::string & _path);
		bool                                SaveFile					(const core::string & _path) const override;

	private:
		static void *						ResizeSceneResourceVector	(core::IObject * _parent, core::uint _offset, core::uint _count, core::uint & _elementSize);
	};
}