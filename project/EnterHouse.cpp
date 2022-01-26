#include "stdafx.h"
#include "EnterHouse.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

#include "Helpers.h"

EnterHouse::EnterHouse()
{
	m_Preconditions["EnemyChasing"] = true;
	//m_Preconditions["HouseInFov"] = true;
	//m_Preconditions["HasWeapon"] = false;
	m_Preconditions["HasHouseTarget"] = true;
	m_Preconditions["ItemInFov"] = false;
	m_Preconditions["InPurgeZone"] = false;
	m_Preconditions["InventoryFull"] = false;
	//m_Preconditions["InHouse"] = false;

	m_Effects["InHouse"] = true;
	m_Effects["EnemyChasing"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 2.f;
}

EnterHouse::EnterHouse(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
	: Action(preConditions, effects, cost)
{
}

bool EnterHouse::CheckProceduralPreconditions(Character* pCharacter) const
{
	if (pCharacter->GetConditions()["HasWeapon"] && pCharacter->GetConditions()["EnemyInFov"])
		return false;
	
	auto houseInfo = pCharacter->GetCurrentHouseTarget();

	// if house has been entered recently don't enter it again
	if (!pCharacter->GetEnteredHouses().empty())
	{
		auto enteredHouses = pCharacter->GetEnteredHouses();

		if (find_if(enteredHouses.begin(), enteredHouses.end(), SameLocation(pCharacter->GetAgentInfo().Position)) != enteredHouses.end())
		{
			return false;
		}
	}
	return true;
}

bool EnterHouse::ExecuteAction(float dt, Character* pCharacter)
{
	GetTarget(pCharacter);
	//HouseInfo houseInfo;
	//houseInfo = pCharacter->GetHousesInFOV()[0];

	const auto agentInfo = pCharacter->GetAgentInfo();
	auto steering = pCharacter->GetSteeringOutput();

	steering.LinearVelocity = pCharacter->GetCurrentHouseTarget().Center - agentInfo.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -agentInfo.MaxLinearSpeed;

	//steering.AutoOrient = false;
	//steering.AngularVelocity = agentInfo.MaxAngularSpeed;
	
	pCharacter->SetSteeringOutput(steering);
	return true;
}

bool EnterHouse::IsDone(Character* pCharacter)
{
	const auto entitiesInFov = pCharacter->GetEntitiesInFOV();
	const auto item = std::find_if(entitiesInFov.begin(), entitiesInFov.end(), IsType<EntityInfo, eEntityType>(eEntityType::ITEM));
	if(item != entitiesInFov.end())
	{
		pCharacter->AddEnteredHouse(pCharacter->GetCurrentHouseTarget().Center);
		pCharacter->ChangeCharacterState("HasHouseTarget", false);
		pCharacter->SetCurrentHouseTarget(HouseInfo{});
		pCharacter->ChangeCharacterState("InHouse", true);
	}

	return IsInRange(pCharacter) || item != entitiesInFov.end();
}

bool EnterHouse::RequiresInRange() const
{
	return true;
}

bool EnterHouse::IsInRange(Character* pCharacter) const
{
	if (pCharacter->GetHousesInFOV().empty())
		return false;
	
	HouseInfo houseInfo = pCharacter->GetHousesInFOV()[0];


	if (pCharacter->GetAgentInfo().Position.x > houseInfo.Center.x - (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.x < houseInfo.Center.x + (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.y > houseInfo.Center.y - (houseInfo.Size.y / 2) &&
		pCharacter->GetAgentInfo().Position.y < houseInfo.Center.y + (houseInfo.Size.y / 2))
	{
		pCharacter->ChangeCharacterState("InHouse", true);
	}

	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x - (houseInfo.Size.x / 2) ,  houseInfo.Center.y - (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);
	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x + (houseInfo.Size.x / 2) ,  houseInfo.Center.y - (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);
	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x - (houseInfo.Size.x / 2) ,  houseInfo.Center.y + (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);
	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x + (houseInfo.Size.x / 2) ,  houseInfo.Center.y + (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);

	auto distance = Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, houseInfo.Center);

	if(distance < (m_Range * m_Range))
	{
		pCharacter->AddEnteredHouse(houseInfo.Center);
		pCharacter->ChangeCharacterState("HasHouseTarget", false);
		pCharacter->SetCurrentHouseTarget(HouseInfo{});
		pCharacter->ChangeCharacterState("InHouse", true);
	}
	
	return distance < (m_Range * m_Range);

}

Elite::Vector2 EnterHouse::GetTarget(Character* pCharacter)
{
	m_Target = pCharacter->GetCurrentHouseTarget().Center;

	return m_Target;
}
