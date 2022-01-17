#include "stdafx.h"
#include "FleeFromEnemy.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>


FleeFromEnemy::FleeFromEnemy()
	:Action()
{
	m_Preconditions["HasWeapon"] = false;
	m_Preconditions["EnemiesInFov"] = true;
	m_Preconditions["HouseInFov"] = false;

	m_Effects["Survive"] = true;

	m_Cost = 5.f;
}

FleeFromEnemy::FleeFromEnemy(const std::map<std::string, bool>& preConditions,
	const std::map<std::string, bool>& effects, float cost)
	:Action(preConditions, effects, cost)
{
}

bool FleeFromEnemy::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool FleeFromEnemy::ExecuteAction(Character* pCharacter)
{
	auto entityVector = pCharacter->GetEntitiesInFOV();
	EnemyInfo enemyInfo;
	for (auto& info : entityVector)
	{
		if (info.Type == eEntityType::ENEMY)
			pCharacter->GetInterface()->Enemy_GetInfo(info, enemyInfo);
	}



	auto characterInfo = pCharacter->GetAgentInfo();

	characterInfo.LinearVelocity = enemyInfo.Location - characterInfo.Position;
	characterInfo.LinearVelocity.Normalize();
	characterInfo.LinearVelocity *= -characterInfo.MaxLinearSpeed;

	pCharacter->SetAgentInfo(characterInfo);

	return true;
}

bool FleeFromEnemy::IsDone(Character* pCharacter)
{
	return true;
}

bool FleeFromEnemy::RequiresInRange() const
{
	return false;
}

bool FleeFromEnemy::IsInRange(Character* pCharacter) const
{
	return true;
}
