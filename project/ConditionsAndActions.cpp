#include "stdafx.h"
#include "ConditionsAndActions.h"

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
