#pragma once
#include <map>
#include <vector>

#include "Blackboard.h"
#include "ConditionsAndActions.h"
#include "Planner.h"

class Character;

class FSMState
{
public:
	FSMState() = default;
	virtual ~FSMState() = default;

	virtual void OnEnter() {};
	virtual void Update(float deltaTime, Character* pCharacter) {};
};

class FSMTransition
{
public:
	FSMTransition() = default;
	virtual ~FSMTransition() = default;

	virtual bool ToTransition(Character* pCharacter) const = 0;
};

class FiniteStateMachine final
{
public:
	FiniteStateMachine(FSMState* startstate);
	~FiniteStateMachine();

	void AddTransition(FSMState* startState, FSMState* toState, FSMTransition* transition);
	void Update(float deltaTime, Character* pBlackboard);
private:
	void ChangeState(FSMState* newState);

	typedef std::pair<FSMTransition*, FSMState*> TransitionStatePair;	// 
	typedef std::vector<TransitionStatePair> Transitions;

	std::map<FSMState*, Transitions> m_Transitions;
	FSMState* m_pCurrentState;
};

class ActionState : public FSMState
{
public:
	ActionState(Character* pCharacter);

	void OnEnter() override;
	void Update(float deltaTime, Character* pCharacter) override;
	
	Action* GetCurrentAction();
private:
	Character* m_pCharacter;
	Planner* m_pPlanner;

	std::vector<Action*> m_pCurrentPlan;
	Action* m_pCurrentAction;
};

class MoveState : public FSMState
{
public:
	void OnEnter() override;
	void Update(float deltaTime, Character* pCharacter) override;
};

class ToMoveTransition : public FSMTransition
{
public:
	bool ToTransition(Character* pCharacter) const override;
};

class ToActionTransition : public FSMTransition
{
public:
	bool ToTransition(Character* pCharacter) const override;
};

