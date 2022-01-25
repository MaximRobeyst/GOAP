#pragma once
#include <map>
#include <string>
#include <vector>

class Blackboard;
class Character;
class Action;




/* The planner is the component of our AI that plans (wow who would've guessed)
 * it will make a plan with a list of actions and return the plan with the lowest cost
 * a planner can be initialized with a list of actions so that we dont have generate a graph
 * every time we want to plan
 */

 //TODO: very important there is a lot of memory allocation going on right now make sure you fix this 
class Planner final
{
public:
	Planner() = default;

	struct PlanNode
	{
		PlanNode() = default;
		PlanNode(PlanNode* pParent, float runningCost, std::map<std::string, bool> state, Action* action);
		~PlanNode();

		bool operator< (const PlanNode& plannode) const;

		PlanNode* pParent;
		float runningCost{};
		std::map<std::string, bool> state;
		Action* action{};
	};

	std::vector<Action*> GetPlan(const std::vector<Action*>& availableActions, const std::map<std::string, bool>& worldConditions, const std::map<std::string, bool>& goal, Character* pCharacter);
	bool IsPlanValid(Action* action, const std::map<std::string, bool>& worldConditions, Character* pCharacter);

private:
	bool BuildGraph(PlanNode* startNode, std::vector<PlanNode*>& leaves, const std::vector<Action*>& usableActions, const std::map<std::string, bool>& goal);	// all possible plans the character can generate
	bool ConditionMatch(const std::map<std::string, bool>& conditions, const std::map<std::string, bool>& preconditions);
	std::map<std::string, bool> PopulateState(std::map<std::string, bool> currentState, std::map<std::string, bool> stateChange);

};

