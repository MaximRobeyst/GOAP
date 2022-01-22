#include "stdafx.h"
#include "EvadeEnemy.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>


EvadeEnemy::EvadeEnemy()
	:Action()
{
	//m_Preconditions["HasWeapon"] = false;
	m_Preconditions["EnemyInFov"] = true;
	//m_Preconditions["HouseInFov"] = false;
	m_Preconditions["InHouse"] = false;
	m_Preconditions["HasHouseTarget"] = false;
	m_Preconditions["HasWeapon"] = false;

	m_Effects["EnemyInFov"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 10.f;
}

EvadeEnemy::EvadeEnemy(const std::map<std::string, bool>& preConditions,
	const std::map<std::string, bool>& effects, float cost)
	:Action(preConditions, effects, cost)
{
}

bool EvadeEnemy::CheckProceduralPreconditions(Character* pCharacter) const
{
	return pCharacter->GetEnteredHouses().empty();
}

bool EvadeEnemy::ExecuteAction(float dt, Character* pCharacter)
{
	auto entityVector = pCharacter->GetEntitiesInFOV();
	auto steering = pCharacter->GetSteeringOutput();
	
	EnemyInfo enemyInfo;
	for (auto& info : entityVector)
	{
		if (info.Type == eEntityType::ENEMY)
			pCharacter->GetInterface()->Enemy_GetInfo(info, enemyInfo);
	}
	
	const auto agentInfo = pCharacter->GetAgentInfo();

	steering.LinearVelocity = enemyInfo.Location - agentInfo.Position;
	auto futurePos = enemyInfo.Location + enemyInfo.LinearVelocity * (steering.LinearVelocity.Magnitude() / agentInfo.MaxLinearSpeed);
	steering.LinearVelocity = agentInfo.Position - enemyInfo.Location;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed;
	
	pCharacter->SetSteeringOutput(steering);

	return true;
}

bool EvadeEnemy::IsDone(Character* pCharacter)
{
	//return !pCharacter->GetConditions()["EnemyChasing"];
	return true;
}

bool EvadeEnemy::RequiresInRange() const
{
	return false;
}

bool EvadeEnemy::IsInRange(Character* pCharacter) const
{
	return true;
}
