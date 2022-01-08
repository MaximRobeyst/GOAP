#pragma once
#include "ConditionsAndActions.h"
class Planner;
class FiniteStateMachine;
class Blackboard;

class Character final
{
public:
	Character(Blackboard* pBlackboard);
	~Character();

	void Update(float dt);

	void AddAction(Action* action);

	std::vector<Action*> GetPlan() const;
	
private:
	void MakeFSM();
	
	FiniteStateMachine* m_pFSM;	// the finite state machine that will make the character move and perform actions its states will be (Plan, Move, Perform)
	Planner* m_pPlanner;		// Planner that will find plans for our character

	std::vector<Action*> m_pActions;
	std::vector<Action*> m_pCurrentPlan;
	Action* m_pCurrentAction;
	Blackboard* m_pBlackboard{nullptr};

	std::map<std::string, bool> m_WorldConditions{};
	std::map<std::string, bool> m_Goals{};
};

