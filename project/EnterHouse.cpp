#include "stdafx.h"
#include "EnterHouse.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

EnterHouse::EnterHouse()
{
	m_Preconditions["HouseInFov"] = true;
	m_Preconditions["HasWeapon"] = false;
	m_Preconditions["InHouse"] = false;

	m_Effects["InHouse"] = true;
	m_Effects["EnemiesInFov"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 1.f;
}

EnterHouse::EnterHouse(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
	: Action(preConditions, effects, cost)
{
}

bool EnterHouse::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool EnterHouse::ExecuteAction(Character* pCharacter)
{
	HouseInfo houseInfo;
	houseInfo = pCharacter->GetHousesInFOV()[0];

	auto characterInfo = pCharacter->GetAgentInfo();

	characterInfo.LinearVelocity = houseInfo.Center - characterInfo.Position;
	characterInfo.LinearVelocity.Normalize();
	characterInfo.LinearVelocity *= -characterInfo.MaxLinearSpeed;

	pCharacter->SetAgentInfo(characterInfo);
	return true;
}

bool EnterHouse::IsDone(Character* pCharacter)
{
	return IsInRange(pCharacter);
}

bool EnterHouse::RequiresInRange() const
{
	return true;
}

bool EnterHouse::IsInRange(Character* pCharacter) const
{
	if (pCharacter->GetHousesInFOV().size() <= 0)
		return false;
	
	HouseInfo houseInfo = pCharacter->GetHousesInFOV()[0];


	if (pCharacter->GetAgentInfo().Position.x > houseInfo.Center.x - (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.x < houseInfo.Center.x + (houseInfo.Size.x / 2) &&
		pCharacter->GetAgentInfo().Position.y > houseInfo.Center.y - (houseInfo.Size.y / 2) &&
		pCharacter->GetAgentInfo().Position.y < houseInfo.Center.y + (houseInfo.Size.y / 2))
		pCharacter->ChangeCharacterState("InHouse", true);

	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x - (houseInfo.Size.x / 2) ,  houseInfo.Center.y - (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);
	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x + (houseInfo.Size.x / 2) ,  houseInfo.Center.y - (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);
	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x - (houseInfo.Size.x / 2) ,  houseInfo.Center.y + (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);
	pCharacter->GetInterface()->Draw_Point(Elite::Vector2{ houseInfo.Center.x + (houseInfo.Size.x / 2) ,  houseInfo.Center.y + (houseInfo.Size.y / 2) }, 4.f, { 1,0,0 }, 0.9f);

	
	return Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, houseInfo.Center) < (m_Range * m_Range);

}

Elite::Vector2 EnterHouse::GetTarget(Character* pCharacter)
{
	if (m_Target == Elite::Vector2{})
	{
		m_Target = pCharacter->GetHousesInFOV()[0].Center;
	}

	return m_Target;
}
