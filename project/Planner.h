#pragma once
#include <map>
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
private:
	struct PlanNode
	{
		PlanNode() = default;
		PlanNode(std::shared_ptr<PlanNode> pParent, float runningCost, std::map<std::string, bool> state, Action* action);

		bool operator< (const PlanNode& plannode);
		
		std::shared_ptr<PlanNode> pParent{};
		float runningCost{};
		std::map<std::string, bool> state;
		Action* action{};
	};
	
public:
	Planner(Character* pCharacter, std::pair<std::string, bool> goal);
	~Planner();

	std::vector<Action*> GetPlan(const std::vector<Action*>& availableActions, const std::map<std::string, bool>& worldConditions, const std::map<std::string, bool>& goal);
	bool IsPlanValid(Action* pAction);
	
private:
	bool BuildGraph(std::shared_ptr<PlanNode> startNode, std::vector<std::shared_ptr<PlanNode>>& leaves, const std::vector<Action*>& usableActions ,const map<std::string, bool>& goal);	// all possible plans the character can generate
	bool ConditionMatch(const std::map<std::string, bool>& conditions, const std::map<std::string, bool>& preconditions);
	
};

