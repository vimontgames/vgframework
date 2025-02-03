#include "ParticleSystemInstance.h"

#if !VG_ENABLE_INLINE
#include "ParticleSystemInstance.inl"
#endif

using namespace vg::core;
using namespace vg::gfx;

namespace vg::renderer
{
    VG_REGISTER_OBJECT_CLASS_EX(ParticleSystemInstance, "ParticleSystem Instance", core::ClassDescFlags::Instance);

    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    ParticleSystemInstance::ParticleSystemInstance(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        //auto * rtManager = RayTracingManager::get(false);
        //if (rtManager)
        //    rtManager->addParticleSystemInstance(this);
    }

    //--------------------------------------------------------------------------------------
    ParticleSystemInstance::~ParticleSystemInstance()
    {
        //VG_SAFE_RELEASE(m_instanceBLAS);
        //auto * rtManager = RayTracingManager::get(false);
        //if (rtManager)
        //    rtManager->removeParticleSystemInstance(this);
    }

    //--------------------------------------------------------------------------------------
    void ParticleSystemInstance::SetModel(core::Lod _lod, core::IModel * _model)
    {
        super::SetModel(_lod, (Model *)_model);
    }

    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::TryGetAABB(core::AABB & _aabb) const
    {
        // TODO
        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::Cull(CullingResult * _cullingResult, View * _view) const
    {
        bool visible = true; // TODO: use AABB vs. Frustum culling

        if (visible)
        {
            _cullingResult->m_output->add(GraphicInstanceListType::Transparent, this);
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool ParticleSystemInstance::OnUpdateRayTracing(gfx::CommandList * _cmdList, View * _view, core::uint _index)
    {
        return false;
    }

    //--------------------------------------------------------------------------------------
    void ParticleSystemInstance::Draw(const RenderContext & _renderContext, gfx::CommandList * _cmdList) const
    {

    }
}
