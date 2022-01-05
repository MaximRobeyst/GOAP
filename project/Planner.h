#pragma once
#include <vector>

class Character;
class Action;




/* The planner is the component of our AI that plans (wow who would've guessed)
 * it will make a plan with a list of actions and return the plan with the lowest cost
 * a planner can be initialized with a list of actions so that we dont have generate a graph
 * every time we want to plan
 */
class Planner
{
public:
	Planner(Character* pCharacter, std::pair<std::string, bool> goal);
	~Planner();

	std::vector<Action*> GetPlan();
	bool IsPlanValid(Action* pAction);
	
private:
	struct PlanNode
	{
		float runningCost{};
		Action* action{};
	};
	
};

