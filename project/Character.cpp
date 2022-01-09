#include "stdafx.h"
#include "Character.h"

#include <Exam_HelperStructs.h>

#include "Planner.h"
#include "Blackboard.h"

Character::Character(Blackboard* pBlackboard)
	: m_pPlanner{new Planner{}}
	, m_pBlackboard(pBlackboard)
{
	m_pActions.push_back(new FleeFromEnemy());
	m_pActions.push_back(new ShootAtEnemy());
	m_pActions.push_back(new SearchForItems());
	m_pActions.push_back(new EnterHouse());

	
	// Set World Conditions
	m_WorldConditions["EnemiesInFov"] = false;
	m_WorldConditions["HouseInFov"] = false;
	m_WorldConditions["HasWeapon"] = false;
	m_WorldConditions["Survive"] = true;
	
	// Set Character goals
	m_Goals["Survive"] = true;

	
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
	if (m_pCurrentAction == nullptr || !m_pPlanner->IsPlanValid(m_pCurrentAction, m_WorldConditions, m_pBlackboard))
	{
		m_pCurrentPlan = m_pPlanner->GetPlan(m_pActions, m_WorldConditions, m_Goals, m_pBlackboard);
		m_pCurrentAction = m_pCurrentPlan[0];
		std::cout << "New Plan: ";
		for_each(m_pCurrentPlan.begin(), m_pCurrentPlan.end(), [](Action* action)
			{
				std::cout << typeid(action).name() << " -> ";
			});
		std::cout << std::endl;
	}

	m_pCurrentAction->ExecuteAction(m_pBlackboard);
	

}

std::vector<Action*> Character::GetPlan() const
{
	return m_pCurrentPlan;
}

void Character::ChangeCharacterState(std::string string, bool b)
{
#ifdef _DEBUG
	if (m_WorldConditions[string] != b)
		std::cout << string << " changed to " << (b ? "true" : "false") << std::endl;
#endif
	
	m_WorldConditions[string] = b;
}

void Character::SetAgentInfo(AgentInfo agentInfo)
{
	m_AgentInfo = agentInfo;
}

AgentInfo Character::GetAgentInfo() const
{
	return m_AgentInfo;
}

void Character::MakeFSM()
{
	
}
