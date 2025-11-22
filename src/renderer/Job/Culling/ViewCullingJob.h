#pragma once

#include "core/Scheduler/Job.h"
#include "GraphicInstanceList.h"
#include "core/Container/AtomicVector.h"
#include "renderer/ILightInstance.h"
#include "renderer/Instance/GraphicInstance.h"
#include "renderer/Instance/Camera/CameraInstance.h"

namespace vg::core
{
    class GameObject;
}

namespace vg::renderer
{
    class View;
    class IGraphicInstance;
    class MeshInstance;
    class CameraInstance;
    class ParticleSystemInstance;
    class Frustum;

    vg_enum_class(vg::renderer, GraphicInstanceListType, core::u8,
        All = 0,
        Opaque,
        AlphaTest,
        Transparent,
        Particle,
        Decal,
        Outline
    );

    struct CullingOptions
    {
        View * m_view = nullptr;
        bool m_raytracing = false;
    };
 
    struct ViewCullingJobOutput
    {
        ~ViewCullingJobOutput() { clear(); }
        void clear();

        void add(GraphicInstanceListType _type, const GraphicInstance * _instance)
        {
            m_instancesLists[core::asInteger(_type)].m_instances.push_back(const_cast<GraphicInstance*>(_instance));
        }

        void add(LightType _type, const LightInstance * _light)
        {
            m_lightsLists[core::asInteger(_type)].m_instances.push_back(const_cast<LightInstance*>(_light));
        }

        void add(const CameraInstance * _cameraInstance)
        {
            m_cameras.push_back(const_cast<CameraInstance *>(_cameraInstance));
        }

        void add(View * _view)
        {
            m_additionalViews.push_back(_view);
        }

        const GraphicInstanceList & get(GraphicInstanceListType _type) const
        {
            return m_instancesLists[core::asInteger(_type)];
        }

        const LightInstanceList & get(LightType _type) const
        {
            return m_lightsLists[core::asInteger(_type)];
        }

        GraphicInstanceList             m_instancesLists[core::enumCount<GraphicInstanceListType>()];
        LightInstanceList               m_lightsLists[core::enumCount<LightType>()];
        core::vector<View *>            m_additionalViews;
        core::vector<CameraInstance *>  m_cameras;
    };

    struct SharedCullingJobOutput
    {
        SharedCullingJobOutput() :
            m_instances(16384),
            m_skins(4096),
            m_particleSystems(4096)
        {

        }

        void clear()
        {
            m_skins.clear();
            m_instances.clear();
            m_particleSystems.clear();
        }

        // Add Instance/SkinMesh/ParticleSystem only once using an atomic flag check and return 'true' if just added

        bool addInstance(GraphicInstance * _graphicInstance)
        {
            if (_graphicInstance->setAtomicFlags(GraphicInstance::AtomicFlags::InstanceList))
            {
                m_instances.push_back_atomic(const_cast<GraphicInstance *>(_graphicInstance));
                return true;
            }
            return false;
        }

        bool addSkinMesh(MeshInstance * _skinMeshInstance)
        {
            if (((GraphicInstance*)_skinMeshInstance)->setAtomicFlags(GraphicInstance::AtomicFlags::SkinList))
            {
                m_skins.push_back_atomic(const_cast<MeshInstance *>(_skinMeshInstance));
                return true;
            }
            return false;
        }

        bool addParticleSystem(ParticleSystemInstance * _particleSystemInstance)
        {
            if (((GraphicInstance *)_particleSystemInstance)->setAtomicFlags(GraphicInstance::AtomicFlags::ParticleList))
            {
                m_particleSystems.push_back_atomic(const_cast<ParticleSystemInstance *>(_particleSystemInstance));
                return true;
            }
            return false;
        }

        core::atomicvector<GraphicInstance *>           m_instances;        // all mesh instances visible in at least one view (used to fill instance data)
        core::atomicvector<MeshInstance *>              m_skins;            // all skinned mesh instances visible at least one view (used to fill skinning data)
        core::atomicvector<ParticleSystemInstance *>    m_particleSystems;  // all simulated particle systems (used to fill particle data)
    };

    struct CullingResult
    {
        ViewCullingJobOutput *      m_output        = nullptr;
        SharedCullingJobOutput *    m_sharedOutput  = nullptr;        
    };

    class ViewCullingJob : public core::Job
    {
    public:
        const char * GetClassName() const final { return "ViewCullingJob"; }

        ViewCullingJob(const core::string & _name, core::IObject * _parent, ViewCullingJobOutput * const _output, SharedCullingJobOutput * const _sharedOutput);
        void Run() override;

    protected:
        void cullGameObjectRecur(const CullingOptions & _cullingOptions, const core::GameObject * _gameobject);

    private:
        VG_INLINE void add(GraphicInstanceListType _type, const IGraphicInstance * _instance);

    private:
        CullingResult m_result;
    };
}