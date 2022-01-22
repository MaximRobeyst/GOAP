#include "stdafx.h"
#include "MoveBackInBoundries.h"
#include "Character.h"

#include <IExamInterface.h>

MoveBackInBoundries::MoveBackInBoundries()
{
	m_Preconditions["OutsideBoundries"] = true;
	m_Preconditions["InHouse"] = false;

	m_Effects["OutsideBoundries"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 1.f;
}

bool MoveBackInBoundries::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool MoveBackInBoundries::ExecuteAction(float dt, Character* pCharacter)
{
	return true;
}

bool MoveBackInBoundries::IsDone(Character* pCharacter)
{
	return true;
}

bool MoveBackInBoundries::RequiresInRange() const
{
	return true;
}

bool MoveBackInBoundries::IsInRange(Character* pCharacter) const
{
	auto dimensions = pCharacter->GetInterface()->World_GetInfo().Dimensions;
	return (pCharacter->GetAgentInfo().Position.x > -dimensions.x / 3 &&
		pCharacter->GetAgentInfo().Position.x < (dimensions.x / 3) &&
		pCharacter->GetAgentInfo().Position.y >(-dimensions.y / 3) &&
		pCharacter->GetAgentInfo().Position.y < (dimensions.y / 3));
}

std::string MoveBackInBoundries::GetName() const
{
	return "Move back in boundries";
}

Elite::Vector2 MoveBackInBoundries::GetTarget(Character* pCharacter)
{
	return Elite::Vector2{};
}
