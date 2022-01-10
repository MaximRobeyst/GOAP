#include "stdafx.h"
#include "FiniteStateMachine.h"


#include "Character.h"
#include <IExamInterface.h>

FiniteStateMachine::FiniteStateMachine(FSMState* startstate)
{
	ChangeState(startstate);
}

FiniteStateMachine::~FiniteStateMachine()
{
}

void FiniteStateMachine::AddTransition(FSMState* startState, FSMState* toState, FSMTransition* transition)
{
	auto it = m_Transitions.find(startState);
	if (it == m_Transitions.end())
		m_Transitions[startState] = Transitions();

	m_Transitions[startState].push_back(std::make_pair(transition, toState));
}

void FiniteStateMachine::Update(float deltaTime, Blackboard* pBlackboard)
{
	if (m_pCurrentState == nullptr)
		return;

	for (size_t i = 0; i < m_Transitions[m_pCurrentState].size(); ++i)
	{
		if (m_Transitions[m_pCurrentState][i].first->ToTransition(pBlackboard))
		{
			ChangeState(m_Transitions[m_pCurrentState][i].second);
		}
	}


	if (m_pCurrentState != nullptr)
		m_pCurrentState->Update(deltaTime, pBlackboard);
}

void FiniteStateMachine::ChangeState(FSMState* newState)
{
	if (newState != m_pCurrentState)
		newState->OnEnter();
	
	m_pCurrentState = newState;
}

ActionState::ActionState(Character* pCharacter)
	: m_pCharacter{pCharacter}
{
}

void ActionState::Update(float deltaTime, Blackboard* pBlackboard)
{
	if (m_pCurrentAction == nullptr || !m_pPlanner->IsPlanValid(m_pCurrentAction, m_pCharacter->GetConditions(), pBlackboard))
	{
		m_pCurrentPlan = m_pPlanner->GetPlan(m_pCharacter->GetActions(), m_pCharacter->GetConditions(), m_pCharacter->GetGoals(), pBlackboard);
		m_pCurrentAction = m_pCurrentPlan[0];
		std::cout << "New Plan: ";
		for_each(m_pCurrentPlan.begin(), m_pCurrentPlan.end(), [](Action* action)
			{
				std::cout << action->GetName() << " -> ";
			});
		std::cout << std::endl;
	}

	m_pCurrentAction->ExecuteAction(pBlackboard);
}

Action* ActionState::GetCurrentAction()
{
	return m_pCurrentAction;
}

void MoveState::OnEnter()
{
	std::cout << "Entered move state" << std::endl;
}

void MoveState::Update(float deltaTime, Blackboard* pBlackboard)
{
	HouseInfo houseInfo;
	if (!pBlackboard->GetData("ClosestHouse", houseInfo))
		return;

	Character* pCharacter;
	if (!pBlackboard->GetData("Character", pCharacter))
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface))
		return;

	auto agentInfo = pCharacter->GetAgentInfo();

	agentInfo.LinearVelocity = pInterface->NavMesh_GetClosestPathPoint(houseInfo.Center) - agentInfo.Position;
	agentInfo.LinearVelocity.Normalize();
	agentInfo.LinearVelocity *= agentInfo.MaxLinearSpeed;


	pCharacter->SetAgentInfo(agentInfo);
	
}

bool ToMoveTransition::ToTransition(Blackboard* pBlackboard) const
{
	Character* pCharacter;
	pBlackboard->GetData("Character", pCharacter);

	if (pCharacter->GetCurrentAction() == nullptr)
		return false;

	return pCharacter->GetCurrentAction()->RequiresInRange() && !pCharacter->GetCurrentAction()->IsInRange(pBlackboard);

}

bool ToActionTransition::ToTransition(Blackboard* pBlackboard) const
{
	Character* pCharacter;
	pBlackboard->GetData("Character", pCharacter);

	if (pCharacter->GetCurrentAction() == nullptr)
		return false;

	return pCharacter->GetCurrentAction()->IsInRange(pBlackboard);
}
