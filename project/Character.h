#pragma once
#include "ConditionsAndActions.h"
class Planner;
class FiniteStateMachine;

class Character final
{
public:
	Character();
	~Character();

	void Update(float dt);

	void AddAction(Action* action);
	
private:
	void MakeFSM();
	
	FiniteStateMachine* m_pFSM;	// the finite state machine that will make the character move and perform actions its states will be (Plan, Move, Perform)
	Planner* m_pPlanner;		// Planner that will find plans for our character

	std::vector<Action*> m_pActions;
	std::vector<Action*> m_pCurrentPlan;
};

