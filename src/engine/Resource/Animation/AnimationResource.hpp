#include "AnimationResource.h"
#include "renderer/IAnimation.h"

using namespace vg::core;
using namespace vg::renderer;

namespace vg::engine
{
    VG_AUTO_REGISTER_CLASS(AnimationResource);

    //--------------------------------------------------------------------------------------
    bool AnimationResource::registerClass(IFactory & _factory)
    {
        if (core::IClassDesc * desc = _factory.registerClassHelper(AnimationResource, "Animation", IClassDesc::Flags::Resource))
            registerProperties(*desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    void * ResizeAnimationResourceVector(IObject * _parent, uint _offset, uint _count, uint & _elementSize)
    {
        auto vec = (core::vector<AnimationResource> *)(uint_ptr(_parent) + _offset);
        vec->clear();
        vec->resize(_count);
        _elementSize = sizeof(AnimationResource);
        return vec->data();
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        _desc.registerResizeVectorFunc(AnimationResource, ResizeAnimationResourceVector);

        return true;
    }

    //--------------------------------------------------------------------------------------
    AnimationResource::AnimationResource(const core::string & _name, core::IObject * _parent) :
        Resource(_name, _parent)
    {

    }

    //--------------------------------------------------------------------------------------
    AnimationResource::~AnimationResource()
    {
        ResourceManager::get()->unloadResource(this, GetResourcePath());
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> AnimationResource::getExtensions() const
    {
        vector<string> ext;
        ext.push_back(".fbx");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void AnimationResource::onResourcePathChanged(const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _oldPath, _newPath);
    }

    //--------------------------------------------------------------------------------------
    bool AnimationResource::cook(const string & _file) const
    {
        return Engine::get()->GetRenderer()->cookAnimation(_file);
    }

    //--------------------------------------------------------------------------------------
    core::IObject * AnimationResource::load(const string & _file)
    {
        IAnimation * anim = Engine::get()->GetRenderer()->loadAnimation(_file);
        VG_ASSERT(nullptr != anim);
        return anim;
    }
}