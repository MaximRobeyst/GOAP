#include "stdafx.h"
#include "FiniteStateMachine.h"

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

void FiniteStateMachine::Update(float deltaTime)
{
	if (m_pCurrentState == nullptr)
		return;

	for (size_t i = 0; i < m_Transitions[m_pCurrentState].size(); ++i)
	{
		if (m_Transitions[m_pCurrentState][i].first->ToTransition())
		{
			ChangeState(m_Transitions[m_pCurrentState][i].second);
		}
	}


	if (m_pCurrentState != nullptr)
		m_pCurrentState->Update(deltaTime);
}

void FiniteStateMachine::ChangeState(FSMState* newState)
{
	m_pCurrentState = newState;
}
