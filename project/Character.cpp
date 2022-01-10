#include "stdafx.h"
#include "Character.h"

#include <Exam_HelperStructs.h>

#include "Planner.h"
#include "Blackboard.h"
#include "FiniteStateMachine.h"

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
	m_pFSM->Update(dt, m_pBlackboard);
}

std::vector<Action*> Character::GetPlan() const
{
	return m_pCurrentPlan;
}

std::vector<Action*> Character::GetActions() const
{
	return m_pActions;
}

Action* Character::GetCurrentAction() const
{
	return m_pActionState->GetCurrentAction();
}

void Character::ChangeCharacterState(std::string string, bool b)
{
#ifdef _DEBUG
	if (m_WorldConditions[string] != b)
		std::cout << string << " changed to " << (b ? "true" : "false") << std::endl;
#endif
	
	m_WorldConditions[string] = b;
}

std::map<std::string, bool> Character::GetConditions() const
{
	return m_WorldConditions;
}

std::map<std::string, bool> Character::GetGoals() const
{
	return m_Goals;
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
	m_pActionState = new ActionState(this);
	m_pMoveState = new MoveState();
	ToMoveTransition* pMoveTransition = new ToMoveTransition();
	ToActionTransition* pActionTransition = new ToActionTransition();

	m_pFSM = new FiniteStateMachine(m_pActionState);
	
	m_pFSM->AddTransition(m_pActionState, m_pMoveState, pMoveTransition);
	m_pFSM->AddTransition(m_pMoveState, m_pActionState, pActionTransition);
}
