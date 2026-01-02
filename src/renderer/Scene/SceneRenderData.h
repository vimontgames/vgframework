#pragma once

#include "renderer/IGraphicScene.h"

namespace vg::renderer
{
    class GraphicInstance;
    class MeshInstance;
    class ParticleSystemInstance;
    class LightInstance;
    class CameraInstance;

    class SceneRenderData : public IGraphicScene
    {
    public:
        SceneRenderData(const core::string & _name, core::IObject * _parent);
        ~SceneRenderData();

        bool RegisterGraphicInstance(IGraphicInstance * _graphicInstance) final override;
        bool UnregisterGraphicInstance(IGraphicInstance * _graphicInstance) final override;

        core::uint getTotalGraphicInstanceCount() const;

    //private:
        core::vector<CameraInstance *>          m_cameraInstances;
        core::vector<LightInstance *>           m_lightInstances;
        core::vector<ParticleSystemInstance *>  m_particleSystemInstances;
        core::vector<MeshInstance *>            m_skinnedMeshInstances;
        core::vector<MeshInstance *>            m_staticMeshInstances;
    };
}