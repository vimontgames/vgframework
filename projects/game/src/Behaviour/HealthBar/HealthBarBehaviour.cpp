#include "Precomp.h"
#include "HealthBarBehaviour.h"
#include "engine/IUIImageComponent.h"
#include "engine/IUITextComponent.h"

using namespace vg::core;
using namespace vg::engine;

VG_REGISTER_COMPONENT_CLASS(HealthBarBehaviour, "HealthBar", "Game", "A health bar with player HP", vg::editor::style::icon::Script, 0);

//--------------------------------------------------------------------------------------
HealthBarBehaviour::HealthBarBehaviour(const string & _name, IObject * _parent) :
    super(_name, _parent)
{
    EnableUpdateFlags(UpdateFlags::Update);
}

//--------------------------------------------------------------------------------------
HealthBarBehaviour::~HealthBarBehaviour()
{

}

//--------------------------------------------------------------------------------------
bool HealthBarBehaviour::registerProperties(IClassDesc & _desc)
{
    super::registerProperties(_desc);

    registerPropertyEx(HealthBarBehaviour, m_width, "Width", vg::core::PropertyFlags::ReadOnly);
    registerPropertyEx(HealthBarBehaviour, m_currentHP, "HP", vg::core::PropertyFlags::ReadOnly);
    registerPropertyEx(HealthBarBehaviour, m_maxHP, "Max", vg::core::PropertyFlags::ReadOnly);

    return true;
}

//--------------------------------------------------------------------------------------
void HealthBarBehaviour::HealthBarBehaviour::OnPlay()
{

}

//--------------------------------------------------------------------------------------
void HealthBarBehaviour::HealthBarBehaviour::OnStop()
{

}

//--------------------------------------------------------------------------------------
void HealthBarBehaviour::Update(const Context & _context)
{

}

//--------------------------------------------------------------------------------------
void HealthBarBehaviour::init(float _maxHP)
{
    m_maxHP = _maxHP;
    m_currentHP = m_maxHP;

    if (auto * lifeGO = GetGameObject()->GetChildGameObject("Life"))
    {
        if (auto * lifeImage = lifeGO->GetComponentT<IUIImageComponent>())
            m_width = (float)lifeImage->GetSize().x;
    }

    setHP(m_currentHP);
}

//--------------------------------------------------------------------------------------
void HealthBarBehaviour::setHP(float _currentHP)
{
    m_currentHP = _currentHP;

    if (auto * backgroundGO = GetGameObject()->GetChildGameObject("Background"))
    {
        if (auto * backgroundImage = backgroundGO->GetComponentT<IUIImageComponent>())
            backgroundImage->SetSize(uint2((1.0f-((float)m_currentHP / (float)m_maxHP)) * (float)m_width, backgroundImage->GetSize().y)); 
    }

    if (auto * lifeGO = GetGameObject()->GetChildGameObject("Life"))
    {
        if (auto * lifeImage = lifeGO->GetComponentT<IUIImageComponent>())
            lifeImage->SetSize(uint2(((float)m_currentHP / (float)m_maxHP) * (float)m_width, lifeImage->GetSize().y));

        if (auto * lifeText = lifeGO->GetComponentT<IUITextComponent>())
            lifeText->SetText(vg::core::fmt::sprintf("%.0f HP", m_currentHP));
    }

    if (m_currentHP <= 0)
        GetGameObject()->setInstanceFlags(InstanceFlags::Enabled, false);
}