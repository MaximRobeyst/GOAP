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

void FiniteStateMachine::Update(float deltaTime, Character* pBlackboard)
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
	m_pCurrentState = newState;
}

ActionState::ActionState(Character* pCharacter)
	: m_pCharacter{pCharacter}
{
}

void ActionState::Update(float deltaTime, Character* pCharacter)
{
	if (m_pCurrentAction == nullptr || !m_pPlanner->IsPlanValid(m_pCurrentAction, m_pCharacter->GetConditions(), pCharacter))
		GenerateNewPlan(pCharacter);

	m_pCurrentAction->ExecuteAction(deltaTime, pCharacter);

	if (m_pCurrentAction->IsDone(pCharacter))
	{
		++m_ActionPointer;
		if (m_pCurrentPlan.size() >= static_cast<size_t>(m_ActionPointer))
		{
			m_ActionPointer = 0;
			GenerateNewPlan(pCharacter);
		}
		
		m_pCurrentAction = m_pCurrentPlan[m_ActionPointer];
	}
}

Action* ActionState::GetCurrentAction() const
{
	return m_pCurrentAction;
}

std::vector<Action*> ActionState::GetCurrentPlan() const
{
	return m_pCurrentPlan;
}

Planner* ActionState::GetPlanner() const
{
	return m_pPlanner;
}

void ActionState::GenerateNewPlan(Character* pCharacter)
{
	m_pCurrentPlan = m_pPlanner->GetPlan(m_pCharacter->GetActions(), m_pCharacter->GetConditions(), m_pCharacter->GetGoals(), pCharacter);
	if (m_pCurrentPlan.empty())
		return;
	
	m_pCurrentAction = m_pCurrentPlan[0];
	std::cout << "New Plan: ";
	for_each(m_pCurrentPlan.begin(), m_pCurrentPlan.end(), [](Action* action)
		{
			std::cout << action->GetName() << " -> ";
		});
	std::cout << std::endl;
}

void MoveState::Update(float deltaTime, Character* pCharacter)
{
	IExamInterface* pInterface = pCharacter->GetInterface();

	auto agentInfo = pCharacter->GetAgentInfo();
	auto steering = pCharacter->GetSteeringOutput();

	auto target = pCharacter->GetCurrentAction()->GetTarget(pCharacter);
	pCharacter->GetInterface()->Draw_Point(target, 4.f, Elite::Vector3{ 1.f, 1.f, 1.f }, 0.4f);

	steering.AutoOrient = true;
	steering.LinearVelocity = pInterface->NavMesh_GetClosestPathPoint(target) - agentInfo.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed;

	pCharacter->SetAgentInfo(agentInfo);
	pCharacter->SetSteeringOutput(steering);
}

bool ToMoveTransition::ToTransition(Character* pCharacter) const
{
	if (pCharacter->GetCurrentAction() == nullptr)
		return false;
	
	return pCharacter->GetCurrentAction()->RequiresInRange() && !pCharacter->GetCurrentAction()->IsInRange(pCharacter);

}

bool ToActionTransition::ToTransition(Character* pCharacter) const
{
	if (pCharacter->GetCurrentAction() == nullptr)
		return false;

	return pCharacter->GetCurrentAction()->IsInRange(pCharacter) || pCharacter->GetCurrentAction()->IsDone(pCharacter) ||
		!pCharacter->GetPlanner()->IsPlanValid(pCharacter->GetCurrentAction(), pCharacter->GetConditions(), pCharacter);
}
