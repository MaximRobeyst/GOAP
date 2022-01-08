
#include "stdafx.h"
#include "Planner.h"

#include <algorithm>

#include "ConditionsAndActions.h"

Planner::PlanNode::PlanNode(PlanNode* pParent, float runningCost, std::map<std::string, bool> state,
	Action* action)
	: pParent(pParent), runningCost{ runningCost }, state{ state }, action{ action }
{
}

Planner::PlanNode::~PlanNode()
{
	pParent = nullptr;
}

bool Planner::PlanNode::operator<(const PlanNode& plannode) const
{
	return runningCost < plannode.runningCost;
}

std::vector<Action*> Planner::GetPlan(const std::vector<Action*>& availableActions, const std::map<std::string, bool>& worldConditions, const std::map<std::string, bool>& goal)
{
	// Check what precedural preconditions
	std::vector<Action*> usableActions{};

	for (auto iter = availableActions.begin(); iter < availableActions.end(); ++iter)
		if ((*iter)->CheckProceduralPreconditions())
			usableActions.push_back(*iter);

	std::vector<PlanNode*> leaves{};
	PlanNode* start = new PlanNode{ nullptr, 0, worldConditions, nullptr };
	const bool succes = BuildGraph(start, leaves, usableActions, goal);

	std::vector<Action*> result;

	if (!succes)
		return result;

	auto cheapest = leaves[0];
	for(auto l : leaves)
	{
		if (l->runningCost < cheapest->runningCost)
			cheapest = l;
	}
	
	PlanNode* n = cheapest;
	while (n != nullptr)
	{
		if (n->action != nullptr)
			result.push_back(n->action);
		n = n->pParent;
	}

	std::reverse(result.begin(), result.end());

	delete start;
	for (auto& i : leaves)
		delete i;
	leaves.clear();
	
	return result;
}

bool Planner::IsPlanValid(Action* action, const std::map<std::string, bool>& worldConditions)
{
	return (action->CheckProceduralPreconditions() && ConditionMatch(worldConditions, action->GetPreconditions()));
}

bool Planner::BuildGraph(PlanNode* startNode, std::vector<PlanNode*>& leaves, const std::vector<Action*>& usableActions, const std::map<std::string, bool>& goal)
{
	bool foundOne = false;

	for (Action* action : usableActions)
	{
		if (ConditionMatch(startNode->state, action->GetPreconditions()))
		{
			std::map<std::string, bool> currentState = PopulateState(startNode->state, action->GetEffects());	// change this with the found state
			PlanNode* node = new PlanNode{ startNode,startNode->runningCost + action->GetCost(),  currentState, action };

			if (ConditionMatch(currentState, goal))
			{
				leaves.push_back(node);
				foundOne = true;
			}
			else
			{
				// no solution test other actions
				std::vector<Action*> subset = usableActions;
				std::remove(subset.begin(), subset.end(), action);

				bool found = BuildGraph(node, leaves, subset, goal);
				if (found)
					foundOne = true;
			}
		}
	}

	return foundOne;
}

bool Planner::ConditionMatch(const std::map<std::string, bool>& conditions,
	const std::map<std::string, bool>& preconditions)
{
	bool allMatch = true;
	for (auto t : preconditions)
	{
		bool match = false;
		for (auto s : conditions)
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

std::map<std::string, bool> Planner::PopulateState(std::map<std::string, bool> currentState,
	std::map<std::string, bool> stateChange)
{
	std::map<std::string, bool> state{};

	for (auto s : currentState)
		state[s.first] = s.second;

	for (auto change : stateChange)
	{
		state[change.first] = change.second;
	}

	return state;
}
