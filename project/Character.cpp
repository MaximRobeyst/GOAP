#include "stdafx.h"
#include "Character.h"
#include "Planner.h"
#include "Blackboard.h"

Character::Character(Blackboard* pBlackboard)
	: m_pPlanner{new Planner{}}
	, m_pBlackboard(pBlackboard)
{
	// Set Actions
	m_pActions.push_back(new GetAxe(
		std::map<std::string, bool>{ {"HasAxe", false}},
		std::map<std::string, bool>{ {"HasAxe", true}},
		2
	));

	m_pActions.push_back(new ChopLogs(
		std::map<std::string, bool>{ {"HasAxe", true}},
		std::map<std::string, bool>{ {"HasFireWood", true}},
		4
	));
	
	m_pActions.push_back(new CollectBraches(	
		std::map<std::string, bool>{},
		std::map<std::string, bool>{ {"HasFireWood", true}},
		8
	));

	// Set World Conditions
	m_WorldConditions["HasAxe"] = false;
	m_WorldConditions["HasFireWood"] = false;
	
	// Set Character goals
	m_Goals["HasFireWood"] = true;

	
	MakeFSM();
}

Character::~Character()
{
	for (Action* action : m_pActions)
		delete action;
	
	delete m_pPlanner;
}

void Character::Update(float dt)
{
	if (m_pCurrentAction == nullptr || m_pPlanner->IsPlanValid(m_pCurrentAction, m_WorldConditions))
	{
		m_pCurrentPlan = m_pPlanner->GetPlan(m_pActions, m_WorldConditions, m_Goals);
		m_pCurrentAction = m_pCurrentPlan[0];
	}
	

}

std::vector<Action*> Character::GetPlan() const
{
	return m_pCurrentPlan;
}

void Character::MakeFSM()
{
	
}
