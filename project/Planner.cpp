#include "stdafx.h"
#include "Planner.h"
#include "ConditionsAndActions.h"

Planner::PlanNode::PlanNode(std::shared_ptr<PlanNode> pParent, float runningCost, std::map<std::string, bool> state,
	Action* action)
		: pParent(pParent), runningCost{runningCost}, state{state}, action{action}
{
}

bool Planner::PlanNode::operator<(const PlanNode& plannode)
{
	return runningCost < plannode.runningCost;
}

std::vector<Action*> Planner::GetPlan(const std::vector<Action*>& availableActions, const std::map<std::string, bool>& worldConditions, const std::map<std::string, bool>& goal)
{
	// Check what precedural preconditions
	std::vector<Action*> usableActions{};
	for (Action* a : availableActions)
		if (a->CheckProceduralPreconditions())
			usableActions.push_back(a);

	std::vector<std::shared_ptr<PlanNode>> leaves;
	std::shared_ptr<PlanNode> start{ std::make_shared<PlanNode>(std::make_shared<PlanNode>() , 0, worldConditions, nullptr) };
	bool succes = BuildGraph(start, leaves, usableActions, goal);

	std::vector<Action*> result;
	
	if (!succes)
		return result;

	auto cheapest = *std::min_element(leaves.begin(), leaves.end());

	PlanNode* n = cheapest.get();
	while(n != nullptr)
	{
		if (n->action != nullptr)
			result.push_back(n->action);
		n = n->pParent.get();
	}
	
	
}

bool Planner::BuildGraph(std::shared_ptr<PlanNode> startNode, std::vector<std::shared_ptr<PlanNode>>& leaves, const std::vector<Action*>& usableActions,
	const map<std::string, bool>& goal)
{
	bool foundOne = false;

	
	for(Action* action : usableActions)
	{
		if (ConditionMatch(startNode->state, action->GetPreconditions()))
		{
			std::map<std::string, bool> currentState;	// change this with the found state
			std::shared_ptr<PlanNode> node{std::make_shared<PlanNode>( startNode,startNode->runningCost + action->GetCost(),  currentState, action) };

			if (ConditionMatch(currentState, goal))
			{
				leaves.push_back(node);
				foundOne = true;
			}
			else
			{
				// no solution test other actions
			}
		}
	}

	return foundOne;
}

bool Planner::ConditionMatch(const std::map<std::string, bool>& conditions,
	const std::map<std::string, bool>& preconditions)
{
	bool allMatch = true;
	for(auto t : preconditions)
	{
		bool match = false;
		for(auto s : conditions)
		{
			if (t == s)
			{
				match = true;
				break;
			}
		}
		if (!match)
			allMatch = false;
	}
	return allMatch;
}
