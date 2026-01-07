#include "MergeStaticCollidersComponent.h"
#include "engine/World/World.h"

using namespace vg::core;

namespace vg::engine
{
    //--------------------------------------------------------------------------------------
    // When the 'IsMergeStaticBodiesEnabled' option is enabled in physics options, component
    // is added to scene root and will hold all the static bodies to optimize physics when
    // engine start, and the original individual physics components are disabled.
    // TODO: This component should be hidden to user.
    //--------------------------------------------------------------------------------------

    VG_REGISTER_COMPONENT_CLASS(MergeStaticCollidersComponent, "Merge Static Colliders", "Physics", "Merge all static colliders in a single physics body.", editor::style::icon::PhysicsMergeStatic, -1);

    //--------------------------------------------------------------------------------------
    bool MergeStaticCollidersComponent::registerProperties(core::IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        return true;
    }

    //--------------------------------------------------------------------------------------
    MergeStaticCollidersComponent::MergeStaticCollidersComponent(const core::string & _name, IObject * _parent) :
        super(_name, _parent)
    {
        EnableUpdateFlags(UpdateFlags::Update, false);
        EnableUpdateFlags(UpdateFlags::ToolUpdate, true);
    }

    //--------------------------------------------------------------------------------------
    MergeStaticCollidersComponent::~MergeStaticCollidersComponent()
    {
        
    }

    //--------------------------------------------------------------------------------------
    void MergeStaticCollidersComponent::ToolUpdate(const Context & _context)
    {
        const auto * physicsOptions = Engine::get()->GetPhysics()->GetOptions();

        if (physicsOptions->IsAnyBodyVisible())
        {
            if (IWorld * iWorld = _context.m_world)
            {
                if (const World * world = VG_SAFE_STATIC_CAST(World, iWorld))
                {
                    const auto & mergedBodies = world->GetMergedStaticBodyComponents();

                    for (uint i = 0; i < mergedBodies.size(); ++i)
                    {
                        const auto & body = mergedBodies[i];
                        const IGameObject * go = body->GetGameObject();

                        if (!body->CanStaticMerge())
                            continue;

                        const auto shapes = go->GetComponentsT<PhysicsShapeComponent>();
                        for (uint j = 0; j < shapes.size(); ++j)
                        {
                            const PhysicsShapeComponent * shape = shapes[j];
                            if (shape && shape->isEnabled())
                            {
                                const auto * physicsShape = shape->getPhysicsShape();
                                if (physicsShape && physicsOptions->IsBodyVisible(physicsShape->GetShapeType(), body->GetMotionType()))
                                    physicsShape->Draw(_context.m_world, go->getGlobalMatrix());
                            }
                        }
                    }
                }
            }            
        }
    }
}