#include "SceneResource.h"
#include "core/File/File.h"
#include "core/IScene.h"

using namespace vg::core;

namespace vg::engine
{
	VG_REGISTER_RESOURCE_CLASS(SceneResource, "Scene");

	//--------------------------------------------------------------------------------------
	void * SceneResource::ResizeSceneResourceVector(IObject * _parent, uint _offset, uint _count, uint & _elementSize)
	{
		auto vec = (core::vector<SceneResource> *)(uint_ptr(_parent) + _offset);
		vec->clear();
		vec->resize(_count);
		_elementSize = sizeof(SceneResource);
		return vec->data();
	}

	//--------------------------------------------------------------------------------------
	bool SceneResource::registerProperties(IClassDesc & _desc)
	{
		super::registerProperties(_desc);

		registerProperty(SceneResource, m_name, "Name");
		registerResizeVectorFunc(SceneResource, ResizeSceneResourceVector);

		return true;
	}

	//--------------------------------------------------------------------------------------
	SceneResource::SceneResource(const core::string & _name, IObject * _parent) :
		Resource(_name, _parent)
	{

	}

	//--------------------------------------------------------------------------------------
	SceneResource::~SceneResource()
	{
		ResourceManager::get()->unloadResource(this, GetResourcePath());
	}

	//--------------------------------------------------------------------------------------
	const core::vector<core::string> SceneResource::getExtensions() const
	{
		vector<string> ext;
		ext.push_back(".scene");
		return ext;
	}

	//--------------------------------------------------------------------------------------
	void SceneResource::onResourcePathChanged(const string & _oldPath, const string & _newPath)
	{
		if (_oldPath != _newPath)
			ResourceManager::get()->loadResourceAsync(this, _oldPath, _newPath);
	}

	//--------------------------------------------------------------------------------------
	bool SceneResource::cook(const string & _file) const
	{
		// Cooked file is same format as source file for now (TODO : serializeFrom/ToBinary)
		const string cookedPath = io::getCookedPath(_file);

		string data;
		if (io::readFile(_file, data))
		{
			if (io::writeFile(cookedPath, data))
			{
				return true;
			}
		}

		return false;
	}

	//--------------------------------------------------------------------------------------
	core::IObject * SceneResource::load(const string & _path)
	{
		IFactory * factory = Kernel::getFactory();
		IScene * scene = dynamic_cast<IScene*>(factory->createObject("Scene"));
		if (nullptr != scene)
		{
			scene->setParent(this);
			if (factory->loadFromXML(scene, _path))
				return scene;
		}

		return nullptr;
	}

	//--------------------------------------------------------------------------------------
	// a .scene file is an XML file that serializes a Scene Object
	//--------------------------------------------------------------------------------------
	bool SceneResource::CreateFile(const core::string & _path)
	{
		const auto * factory = Kernel::getFactory();

		IScene * scene = dynamic_cast<IScene*>(factory->createObject("Scene"));
		if (nullptr != scene)
		{
			factory->saveToXML(scene, _path);
			VG_SAFE_RELEASE(scene);
			return true;
		}

		return false;
	}

	//--------------------------------------------------------------------------------------
	bool SceneResource::SaveFile(const string & _path) const
	{
		IScene * scene = dynamic_cast<IScene*>(getObject());
		if (nullptr != scene)
		{
			const auto * factory = Kernel::getFactory();
			return factory->saveToXML(scene, _path);
		}
		return false;
	}
}