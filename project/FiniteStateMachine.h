#pragma once
#include <map>
#include <vector>

class FSMState
{
public:
	FSMState() = default;
	virtual ~FSMState() = default;

	virtual void Update(float deltaTime) {};
};

class FSMTransition
{
public:
	FSMTransition() = default;
	virtual ~FSMTransition() = default;

	virtual bool ToTransition() const = 0;
};

class FiniteStateMachine final
{
public:
	FiniteStateMachine(FSMState* startstate);
	~FiniteStateMachine();

	void AddTransition(FSMState* startState, FSMState* toState, FSMTransition* transition);
	void Update(float deltaTime);
private:
	void ChangeState(FSMState* newState);

	typedef std::pair<FSMTransition*, FSMState*> TransitionStatePair;	// 
	typedef std::vector<TransitionStatePair> Transitions;

	std::map<FSMState*, Transitions> m_Transitions;
	FSMState* m_pCurrentState;
};

