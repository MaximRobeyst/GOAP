#include "stdafx.h"
#include "SearchForItems.h"
#include "Character.h"

#include <IExamInterface.h>

SearchForItems::SearchForItems()
{
	//m_Preconditions["EnemyChasing"] = false;
	//m_Preconditions["HouseInFov"] = false;
	m_Preconditions["HasHouseTarget"] = false;
	m_Preconditions["HasItemTarget"] = false;
	//m_Preconditions["EnemyInFov"] = false;
	
	m_Effects["Survive"] = true;

	m_Cost = 20.f;

}

SearchForItems::SearchForItems(const std::map<std::string, bool>& preConditions,
	const std::map<std::string, bool>& effects, float cost)
	:Action(preConditions, effects, cost)
{
}

bool SearchForItems::CheckProceduralPreconditions(Character* pCharacter) const
{
	if (pCharacter->GetConditions()["HasWeapon"] && (pCharacter->GetConditions()["WasBitten"] || pCharacter->GetConditions()["EnemyInFov"]))	// Special case where if these 2 are true at the same time we cannot execute
		return false;

	if (pCharacter->GetConditions()["HasMedkit"] && pCharacter->GetConditions()["HealthLow"])	// if these 2 values are true at the same time we want to heal
		return false;
	
	return true;
}

bool SearchForItems::ExecuteAction(float dt, Character* pCharacter)
{
	const auto characterInfo = pCharacter->GetAgentInfo();
	auto steering = pCharacter->GetSteeringOutput();

	if(Elite::DistanceSquared(characterInfo.Position, m_CurrentTarget) < (m_Range * m_Range) || steering.LinearVelocity.SqrtMagnitude() < 1.f
		|| !pCharacter->GetHousesInFOV().empty() || m_Timer >= m_Patience)
	{
		GenerateNewTarget(pCharacter->GetInterface()->World_GetInfo());
	}
	const Elite::Vector2 targetDir{ pCharacter->GetInterface()->NavMesh_GetClosestPathPoint(m_CurrentTarget) - characterInfo.Position };

	pCharacter->GetInterface()->Draw_Point(m_CurrentTarget, 1.f, Elite::Vector3{ 1,0.5f, 0.f }, 0.4f);

	if (pCharacter->GetConditions()["HasWeapon"])
	{
		steering.AutoOrient = false;
		steering.AngularVelocity = characterInfo.MaxAngularSpeed;
	}
	
	steering.RunMode = pCharacter->GetConditions()["WasBitten"];

	if (m_WasAttackedTimer < m_WasAttackTimerMax)
	{
		m_WasAttackedTimer += dt;
		RotateAgent(pCharacter);
	}

	steering.LinearVelocity = targetDir;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= characterInfo.MaxLinearSpeed;

	pCharacter->SetSteeringOutput(steering);

	m_Timer += dt;
	
	return true;
}

bool SearchForItems::IsDone(Character* pCharacter)
{
	//auto steering = pCharacter->GetSteeringOutput();
	//steering.AutoOrient = false;
	//pCharacter->SetSteeringOutput(steering);
	
	return false;
}

bool SearchForItems::RequiresInRange() const
{
	return false;
}

bool SearchForItems::IsInRange(Character* pBlackboard) const
{
	return true;
}

void SearchForItems::RotateAgent(Character* pCharacter)
{
	auto steering = pCharacter->GetSteeringOutput();

	steering.AutoOrient = false;
	steering.AngularVelocity = pCharacter->GetAgentInfo().MaxAngularSpeed;

	pCharacter->SetSteeringOutput(steering);
}

void SearchForItems::GenerateNewTarget(const WorldInfo& worldInfo)
{
	m_CurrentTarget = 
		Elite::Vector2(
			Elite::randomFloat(-worldInfo.Dimensions.x / 2, worldInfo.Dimensions.x / 2),
			Elite::randomFloat(-worldInfo.Dimensions.y / 2, worldInfo.Dimensions.y / 2)
		);

	m_Timer = 0;
}
