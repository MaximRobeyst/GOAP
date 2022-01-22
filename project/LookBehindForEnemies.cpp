#include "stdafx.h"
#include "LookBehindForEnemies.h"
#include "Character.h"

LookBehindForEnemies::LookBehindForEnemies()
{
	m_Preconditions["EnemyChasing"] = true;
	m_Preconditions["WasBitten"] = true;
	m_Preconditions["EnemyInFov"] = false;
	m_Preconditions["HasWeapon"] = true;
	
	m_Effects["Survive"] = true;
	m_Effects["EnemiesInFov"] = true;

	m_Cost = 1.f;
}

bool LookBehindForEnemies::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool LookBehindForEnemies::ExecuteAction(float dt, Character* pCharacter)
{
	auto agentInfo = pCharacter->GetAgentInfo();
	auto steering = pCharacter->GetSteeringOutput();

	steering.AutoOrient = false;
	steering.AngularVelocity = agentInfo.MaxAngularSpeed;

	pCharacter->SetSteeringOutput(steering);
	
	return true;
}

bool LookBehindForEnemies::IsDone(Character* pCharacter)
{
	return true;
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
