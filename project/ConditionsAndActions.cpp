#include "stdafx.h"
#include "ConditionsAndActions.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>


#include "Character.h"

Action::Action(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost)
	: m_Preconditions(preConditions) , m_Effects(effects), m_Cost(cost)
{
}

Elite::Vector2 Action::GetTarget(Character* pCharacter)
{
	return Elite::Vector2{};
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
