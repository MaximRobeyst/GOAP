#include "stdafx.h"
#include "EscapeHouse.h"
#include "Character.h"

EscapeHouse::EscapeHouse()
{
	//m_Preconditions["EnemyChasing"] = true;
	m_Preconditions["InHouse"] = true;
	m_Preconditions["itemInFov"] = false;
	//m_Preconditions["EnemyInFov"] = true;
	m_Preconditions["HasWeapon"] = false;
	m_Preconditions["HealthLow"] = true;
	
	//m_Preconditions["HasHouseTarget"] = false;
	//m_Preconditions["HouseInFov"] = false;

	m_Effects["Survive"] = true;

	m_Cost = 2.5f;
}

bool EscapeHouse::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool EscapeHouse::ExecuteAction(float dt, Character* pCharacter)
{
	return true;
}

bool EscapeHouse::IsDone(Character* pCharacter)
{
	return true;
}

bool EscapeHouse::RequiresInRange() const
{
	return true;
}

bool EscapeHouse::IsInRange(Character* pCharacter) const
{
	return false;
}

std::string EscapeHouse::GetName() const
{
	return "Get away from house";
}

Elite::Vector2 EscapeHouse::GetTarget(Character* pCharacter)
{
	return Elite::Vector2{};
}
