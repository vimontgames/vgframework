#include "engine/Precomp.h"
#include "SnapComponent.h"
#include "editor/Editor_Consts.h"
#include "core/IGameObject.h"
#include "core/Math/Math.h"

using namespace vg::core;

namespace vg::engine
{
    VG_REGISTER_COMPONENT_CLASS(SnapComponent, "Snap", "Editor", "Override Snap Settings", editor::style::icon::Snap, 0);

    //--------------------------------------------------------------------------------------
    bool SnapComponent::registerProperties(IClassDesc & _desc)
    {
        super::registerProperties(_desc);

        registerPropertyCallbackEx(SnapComponent, snapObject, "Snap", PropertyFlags::SingleLine);
        setPropertyDescription(SnapComponent, snapObject, "Apply snaping constraints to selected object");

        registerOptionalProperty(SnapComponent, m_snapTranslation, m_translation, "Translate");
        registerOptionalProperty(SnapComponent, m_snapRotation, m_rotation, "Rotate");
        registerOptionalProperty(SnapComponent, m_snapScale, m_scale, "Scale");

        return true;
    }

    //--------------------------------------------------------------------------------------
    SnapComponent::SnapComponent(const core::string & _name, core::IObject * _parent) :
        super(_name, _parent)
    {
        EnableUpdateFlags(UpdateFlags::Update, false);
    }

    //--------------------------------------------------------------------------------------
    SnapComponent::~SnapComponent()
    {

    }

    //--------------------------------------------------------------------------------------
    void SnapComponent::snap()
    {
        snapObjectRecur(GetGameObject());
    }

    //--------------------------------------------------------------------------------------
    void SnapComponent::snapObjectRecur(IGameObject * _gameObject)
    {
        if (_gameObject->IsPrefab())
        {
            bool dirty = false;

            float3 T, R, S;
            Float4x4ToTRS(_gameObject->GetLocalMatrix(), T, R, S);

            const float eps = 0.0001f;

            if (m_snapTranslation)
            {
                const float3 ratioT = T / abs(m_translation.xyz);
                const float3 nearestIntT = round(ratioT);

                if (any( abs(nearestIntT-ratioT) > eps ))
                {
                    float3 snapT = nearestIntT * m_translation;

                    VG_WARNING("[Snap] Translation of Prefab \"%s\" (UID: 0x%08X) changed from (%.3f, %.3f, %.3f) to (%.3f, %.3f, %.3f) because of translation snap value of (%.3f, %.3f, %.3f)", 
                        _gameObject->GetName().c_str(), 
                        _gameObject->GetUID(),
                        (float)T.x, (float)T.y, (float)T.z,
                        (float)snapT.x, (float)snapT.y, (float)snapT.z,
                        (float)m_translation.x, (float)m_translation.y, (float)m_translation.z
                    );

                    T = snapT;
                    dirty = true;
                }
            }

            if (m_snapRotation)
            {
                const float3 ratioR = R / abs(m_rotation);
                const float3 nearestIntR = round(ratioR);

                if (any(abs(nearestIntR - ratioR) > eps))
                {
                    float3 snapR = nearestIntR * m_rotation;

                    VG_WARNING("[Snap] Rotation of Prefab \"%s\" (UID: 0x%08X) changed from (%.3f, %.3f, %.3f) to (%.3f, %.3f, %.3f) because of translation snap value of (%.3f, %.3f, %.3f)",
                        _gameObject->GetName().c_str(),
                        _gameObject->GetUID(),
                        (float)R.x, (float)R.y, (float)R.z,
                        (float)snapR.x, (float)snapR.y, (float)snapR.z,
                        (float)m_rotation, (float)m_rotation, (float)m_rotation
                    );

                    R = snapR;
                    dirty = true;
                }
            }

            if (dirty)
            {
                float4x4 matrix = TRSToFloat4x4(T, R, S);
                _gameObject->SetLocalMatrix(matrix);
            }
        }

        if (IGameObject * parent = dynamic_cast<IGameObject *>(_gameObject->getParent()))
            snapObjectRecur(parent);
    }

    //--------------------------------------------------------------------------------------
    bool SnapComponent::snapObject(IObject * _object)
    {
        auto * snap = dynamic_cast<SnapComponent *>(_object);

        if (snap)
            snap->snapObjectRecur(snap->GetGameObject());

        return true;
    }

    //--------------------------------------------------------------------------------------
    bool SnapComponent::TryGetSnapTranslate(core::float3 & _translate) const
    {
        if (m_snapTranslation)
        {
            _translate = m_translation;
            return true;
        }

        return false;
    }
    //--------------------------------------------------------------------------------------
    bool SnapComponent::TryGetSnapRotate(float & _rotate) const
    {
        if (m_snapRotation)
        {
            _rotate = m_rotation;
            return true;
        }

        return false;
    }

    //--------------------------------------------------------------------------------------
    bool SnapComponent::TryGetSnapScale(float & _scale) const 
    {
        if (m_snapScale)
        {
            _scale = m_scale;
            return true;
        }

        return false;
    }
}