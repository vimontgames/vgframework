#include "engine/Precomp.h"
#include "MeshResource.h"
#include "engine/Engine.h"
#include "graphics/renderer/IRenderer.h"
#include "graphics/renderer/IMeshModel.h"
#include "engine/resource/ResourceManager.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    MeshResource::MeshResource(IObject * _owner) : 
        Resource(_owner)
    {

    }

    //--------------------------------------------------------------------------------------
    MeshResource::~MeshResource()
    {
        VG_SAFE_RELEASE(m_meshModel);
    }

    //--------------------------------------------------------------------------------------
    const core::vector<core::string> MeshResource::getExtensions() const
    {
        vector<string> ext;
                       ext.push_back(".fbx");
        return ext;
    }

    //--------------------------------------------------------------------------------------
    void MeshResource::onPathChanged(IObject * _owner, const string & _oldPath, const string & _newPath)
    {
        if (_oldPath != _newPath)
            ResourceManager::get()->loadResourceAsync(this, _newPath, _owner);
    }

    //--------------------------------------------------------------------------------------
    bool MeshResource::loadResource(const core::string & _path, core::IObject * _owner)
    {
        auto * engine = Engine::get();
        auto * renderer = engine->getRenderer();

        VG_SAFE_RELEASE(m_meshModel);

        m_meshModel = renderer->createMeshModel(_path);

        return nullptr != m_meshModel;
    }
}