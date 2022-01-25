#include "stdafx.h"
#include "LookBehindForEnemies.h"
#include "Character.h"

LookBehindForEnemies::LookBehindForEnemies()
{
	m_Preconditions["WasBitten"] = true;
	m_Preconditions["EnemyInFov"] = false;
	m_Preconditions["HasWeapon"] = true;
	
	m_Effects["Survive"] = true;
	m_Effects["EnemyInFov"] = true;

	m_Cost = 5.f;
}

bool LookBehindForEnemies::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool LookBehindForEnemies::ExecuteAction(float dt, Character* pCharacter)
{
	const auto agentInfo = pCharacter->GetAgentInfo();
	auto steering = pCharacter->GetSteeringOutput();

	steering.RunMode = true;
	steering.AutoOrient = false;
	steering.AngularVelocity = agentInfo.MaxAngularSpeed;

	pCharacter->SetSteeringOutput(steering);
	
	return true;
}

bool LookBehindForEnemies::IsDone(Character* pCharacter)
{

	auto steering = pCharacter->GetSteeringOutput();

	steering.RunMode = false;
	steering.AutoOrient = true;

	pCharacter->SetSteeringOutput(steering);
	
	return !pCharacter->GetEntitiesInFOV().empty();
}

bool LookBehindForEnemies::RequiresInRange() const
{
	return false;
}

bool LookBehindForEnemies::IsInRange(Character* pCharacter) const
{
	return true;
}

std::string LookBehindForEnemies::GetName() const
{
	return "Look Enemies for behind";
}
