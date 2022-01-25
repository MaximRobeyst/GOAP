#include "stdafx.h"
#include "MoveBackInBoundaries.h"
#include "Character.h"

#include <IExamInterface.h>

MoveBackInBoundaries::MoveBackInBoundaries()
{
	m_Preconditions["OutsideBoundries"] = true;
	m_Preconditions["InHouse"] = false;

	m_Effects["OutsideBoundries"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 1.f;
}

bool MoveBackInBoundaries::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool MoveBackInBoundaries::ExecuteAction(float dt, Character* pCharacter)
{
	return true;
}

bool MoveBackInBoundaries::IsDone(Character* pCharacter)
{
	return true;
}

bool MoveBackInBoundaries::RequiresInRange() const
{
	return true;
}

bool MoveBackInBoundaries::IsInRange(Character* pCharacter) const
{
	auto dimensions = pCharacter->GetInterface()->World_GetInfo().Dimensions;
	return (pCharacter->GetAgentInfo().Position.x > -dimensions.x / 3 &&
		pCharacter->GetAgentInfo().Position.x < (dimensions.x / 3) &&
		pCharacter->GetAgentInfo().Position.y >(-dimensions.y / 3) &&
		pCharacter->GetAgentInfo().Position.y < (dimensions.y / 3));
}

std::string MoveBackInBoundaries::GetName() const
{
	return "Move back in boundries";
}

Elite::Vector2 MoveBackInBoundaries::GetTarget(Character* pCharacter)
{
	return Elite::Vector2{};
}
