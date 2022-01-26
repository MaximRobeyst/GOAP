#include "stdafx.h"
#include "PatrolForEnemies.h"
#include "Character.h"

PatrolForEnemies::PatrolForEnemies()
{
	m_Preconditions["HasWeapon"] = true;
	m_Preconditions["InventoryFull"] = true;
	//m_Preconditions["HealthLow"] = false;
	//m_Preconditions["EnergyLow"] = false;
	m_Preconditions["EnemyInFov"] = false;
	m_Preconditions["InHouse"] = false;
	m_Preconditions["InPurgeZone"] = false;
	m_Preconditions["OutsideBoundries"] = false;

	m_Effects["Survive"] = true;
	m_Effects["EnemyInFov"] = true;

	m_Cost = 7.5f;
}

PatrolForEnemies::PatrolForEnemies(const std::map<std::string, bool>& preConditions,
	const std::map<std::string, bool>& effects, float cost)
	:Action(preConditions, effects, cost)
{
}

bool PatrolForEnemies::CheckProceduralPreconditions(Character* pCharacter) const
{

	if (pCharacter->GetConditions()["HasFood"] && pCharacter->GetConditions()["EnergyLow"])	// if these 2 values are true at the same time we want to heal
		return false;
	
	if (pCharacter->GetConditions()["HasMedkit"] && pCharacter->GetConditions()["HealthLow"])	// if these 2 values are true at the same time we want to heal
		return false;
	
	return true;
}

bool PatrolForEnemies::ExecuteAction(float dt, Character* pCharacter)
{
	auto steering = pCharacter->GetSteeringOutput();

	m_WanderAngle += Elite::randomFloat(-Elite::ToRadians(30.f), Elite::ToRadians(30.f));
	const Elite::Vector2 target = pCharacter->GetAgentInfo().Position + pCharacter->GetAgentInfo().LinearVelocity.GetNormalized() * 6.f + (Elite::Vector2(cosf(m_WanderAngle) * 4.f, sinf(m_WanderAngle) * 4.f));

	steering.AutoOrient = false;
	steering.AngularVelocity = pCharacter->GetAgentInfo().MaxAngularSpeed;
	
	steering.LinearVelocity = target - pCharacter->GetAgentInfo().Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= pCharacter->GetAgentInfo().MaxLinearSpeed;

	pCharacter->SetSteeringOutput(steering);
	
	return true;
}

bool PatrolForEnemies::IsDone(Character* pCharacter)
{
	return false;
}

bool PatrolForEnemies::RequiresInRange() const
{
	return false;
}

bool PatrolForEnemies::IsInRange(Character* pCharacter) const
{
	return true;
}

std::string PatrolForEnemies::GetName() const
{
	return "Patrol for enemies";
}
