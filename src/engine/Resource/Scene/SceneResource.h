#pragma once

#include "core/Resource/Resource.h"

namespace vg::engine
{
	class SceneResource : public core::Resource
	{
	public:
		VG_CLASS_DECL(SceneResource, core::Resource);

		SceneResource(const core::string & _name = "", IObject * _parent = nullptr);
		~SceneResource();

		const core::vector<core::string>    getExtensions			() const final;
		void                                onResourcePathChanged	(const core::string & _oldPath, const core::string & _newPath) final;

		bool                                cook					(const core::string & _file) const final override;
		core::IObject *						load					(const core::string & _path) final override;

		bool                                CreateFile				(const core::string & _path);
		bool                                SaveFile				(const core::string & _path) const override;

		bool                                CanCreateFile			() const final override { return true; };
		bool                                CanSaveFile				() const final override { return true; };

	private:
		static void * ResizeSceneResourceVector(core::IObject * _parent, core::uint _offset, core::uint _count, core::uint & _elementSize);
	};
}