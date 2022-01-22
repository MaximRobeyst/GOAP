#include "stdafx.h"
#include "CheckArea.h"
#include "Character.h"

CheckArea::CheckArea()
{
	//m_Preconditions["EnemyChasing"] = false;
	//m_Preconditions["HouseInFov"] = false;
	m_Preconditions["ItemInFov"] = false;
	m_Preconditions["InHouse"] = true;
	m_Preconditions["EnemyInFov"] = false;
	m_Preconditions["HasHouseTarget"] = false;
	m_Preconditions["InventoryFull"] = false;

	m_Effects["InventoryFull"] = false;
	m_Effects["ItemInFov"] = true;
	m_Effects["Survive"];

	m_Cost = 0.1f;
}

bool CheckArea::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool CheckArea::ExecuteAction(float dt, Character* pCharacter)
{
	m_Timer += dt;
	
	auto steering = pCharacter->GetSteeringOutput();
	steering.AutoOrient = false;
	steering.AngularVelocity = pCharacter->GetAgentInfo().AngularVelocity;
	pCharacter->SetSteeringOutput(steering);

	return true;
}

bool CheckArea::IsDone(Character* pCharacter)
{
	auto steering = pCharacter->GetSteeringOutput();
	steering.AutoOrient = true;
	pCharacter->SetSteeringOutput(steering);
	
	return m_Timer <= m_TimeToCheck;
}

bool CheckArea::RequiresInRange() const
{
	return false;
}

bool CheckArea::IsInRange(Character* pCharacter) const
{
	return true;
}

std::string CheckArea::GetName() const
{
	return "Check area";
}
