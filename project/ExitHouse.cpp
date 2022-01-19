#include "stdafx.h"
#include "ExitHouse.h"
#include "Character.h"

#include <Exam_HelperStructs.h>

ExitHouse::ExitHouse()
{
	m_Preconditions["EnemiesInFov"] = true;
	m_Preconditions["HouseInFov"] = true;
	m_Preconditions["ItemInFov"] = false;
	m_Preconditions["InHouse"] = true;

	m_Effects["Survive"] = true;
	m_Effects["InHouse"] = false;

	m_Cost = 5.0f;
}

bool ExitHouse::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool ExitHouse::ExecuteAction(Character* pCharacter)
{
	
	return true;
}

bool ExitHouse::IsDone(Character* pCharacter)
{
	return IsInRange(pCharacter);
}

bool ExitHouse::RequiresInRange() const
{
	return true;
}

bool ExitHouse::IsInRange(Character* pCharacter) const
{
	if (pCharacter->GetHousesInFOV().size() <= 0)
		return true;

	HouseInfo houseInfo = pCharacter->GetHousesInFOV()[0];


	if (pCharacter->GetAgentInfo().Position.x < houseInfo.Center.x - (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.x > houseInfo.Center.x + (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.y < houseInfo.Center.y - (houseInfo.Size.y / 2) &&
		pCharacter->GetAgentInfo().Position.y > houseInfo.Center.y + (houseInfo.Size.y / 2))
	{
		pCharacter->ChangeCharacterState("InHouse", false);
		return true;
	}
	
	return false;

}

Elite::Vector2 ExitHouse::GetTarget(Character* pCharacter)
{
	return Elite::Vector2{};
}
