#include "engine/Precomp.h"
#include "SnapComponent.h"
#include "editor/Editor_Consts.h"
#include "core/IGameObject.h"

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
            if (m_snapTranslation)
            {
                float4x4 matrix = _gameObject->GetLocalMatrix();
                const float eps = 0.001f;
                const float3 ratioT = matrix[3].xyz / abs(m_translation.xyz);
                const float3 nearestMultiple = round(ratioT);

                if (any( abs(nearestMultiple-ratioT) > eps ))
                {
                    float3 snapT = nearestMultiple * m_translation;

                    VG_WARNING("[Snap] Translation of Prefab \"%s\" (%.3f, %.3f, %.3f) has been snapped to (%.3f, %.3f, %.3f) because of snap component forces snap to (%.3f, %.3f, %.3f)", 
                        _gameObject->GetName().c_str(), 
                        (float)matrix[3].x, (float)matrix[3].y, (float)matrix[3].z,
                        (float)snapT.x, (float)snapT.y, (float)snapT.z,
                        (float)m_translation.x, (float)m_translation.y, (float)m_translation.z
                    );

                    matrix[3].xyz = snapT;
                    _gameObject->SetLocalMatrix(matrix);
                }
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