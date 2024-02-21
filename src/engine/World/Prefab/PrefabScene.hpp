#include "engine/Precomp.h"
#include "PrefabScene.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_OBJECT_CLASS_EX(PrefabScene, "PrefabScene", IClassDesc::Flags::SceneNode);

    //--------------------------------------------------------------------------------------
    PrefabScene::PrefabScene(const string & _name, IObject * _parent) :
        super(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    PrefabScene::~PrefabScene()
    {

    }

    //--------------------------------------------------------------------------------------
    bool PrefabScene::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }
}