#include "engine/Precomp.h"
#include "Scene.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS_EX(Scene, "Scene", ClassDescFlags::SceneNode);

    //--------------------------------------------------------------------------------------
    Scene::Scene(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    Scene::~Scene()
    {
        // TODO: unselect all scene objects otherwise we might crash at exit?
    }

    //--------------------------------------------------------------------------------------
    bool Scene::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }
}