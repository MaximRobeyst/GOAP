#include "stdafx.h"
#include "ConditionsAndActions.h"

Action::Action(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost)
	: m_Preconditions(preConditions) , m_Effects(effects), m_Cost(cost)
{
}

std::map<std::string, bool> Action::GetPreconditions()
{
	return m_Preconditions;
}

std::map<std::string, bool> Action::GetEffects()
{
	return m_Effects;
}

float Action::GetCost()
{
	return m_Cost;
}

GetAxe::GetAxe(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
                   float cost)
	: Action(preConditions, effects, cost)
{
}

bool GetAxe::CheckProceduralPreconditions() const
{
	return true;
}

bool GetAxe::ExecuteAction()
{
	return true;
}

bool GetAxe::IsDone()
{
	return true;
}

bool GetAxe::RequiresInRange() const
{
	return false;
}

std::string GetAxe::GetName() const
{
	return "GetAxe";
}

ChopLogs::ChopLogs(const std::map<std::string, bool>& preConditions,
	const std::map<std::string, bool>& effects, float cost)
		: Action(preConditions, effects, cost)
{
}

bool ChopLogs::CheckProceduralPreconditions() const
{
	return true;
}

bool ChopLogs::ExecuteAction()
{
	return true;
}

bool ChopLogs::IsDone()
{
	return true;
}

bool ChopLogs::RequiresInRange() const
{
	return false;
}

std::string ChopLogs::GetName() const
{
	return "ChopLogs";
}

CollectBraches::CollectBraches(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
	: Action(preConditions, effects, cost)
{
}

bool CollectBraches::CheckProceduralPreconditions() const
{
	return true;
}

bool CollectBraches::ExecuteAction()
{
	return true;
}

bool CollectBraches::IsDone()
{
	return true;
}

bool CollectBraches::RequiresInRange() const
{
	return false;
}

std::string CollectBraches::GetName() const
{
	return "CollectBraches";
}
