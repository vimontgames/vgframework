#include "engine/Precomp.h"
#include "MeshResource.h"
#include "engine/Engine.h"
#include "graphics/renderer/IRenderer.h"
#include "graphics/renderer/IMeshModel.h"

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
        {
            auto * engine = Engine::get();
            auto * renderer = engine->getRenderer();

            VG_SAFE_RELEASE(m_meshModel);

            m_meshModel = renderer->createMeshModel(_newPath);

            _owner->onResourceLoaded(this);
        }
    }
}