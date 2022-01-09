#include "stdafx.h"
#include "ConditionsAndActions.h"

#include <Exam_HelperStructs.h>


#include "Character.h"
#include "Blackboard.h"

Action::Action(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects, float cost)
	: m_Preconditions(preConditions) , m_Effects(effects), m_Cost(cost)
{
}

std::map<std::string, bool> Action::GetPreconditions()
{
	return m_Preconditions;
}

std::map<std::string, bool> Action::GetEffects()
{
	return m_Effects;
}

float Action::GetCost()
{
	return m_Cost;
}

FleeFromEnemy::FleeFromEnemy()
	:Action()
{
	m_Preconditions["HasWeapon"] = false;
	m_Preconditions["EnemiesInFov"] = true;

	m_Effects["Survive"] = true;

	m_Cost = 5.f;
}

FleeFromEnemy::FleeFromEnemy(const std::map<std::string, bool>& preConditions,
	const std::map<std::string, bool>& effects, float cost)
		:Action(preConditions, effects, cost)
{
}

bool FleeFromEnemy::CheckProceduralPreconditions(Blackboard* pBlackboard) const
{
	return true;
}

bool FleeFromEnemy::ExecuteAction(Blackboard* pBlackboard)
{
	Character* pCharacter;
	pBlackboard->GetData("Character", pCharacter);
	EnemyInfo enemyInfo;
	pBlackboard->GetData("ClosestEnemy", enemyInfo);

	auto characterInfo = pCharacter->GetAgentInfo();
	
	characterInfo.LinearVelocity = enemyInfo.Location - characterInfo.Position;
	characterInfo.LinearVelocity.Normalize();
	characterInfo.LinearVelocity *= -characterInfo.MaxLinearSpeed;

	pCharacter->SetAgentInfo(characterInfo);
	
	return true;
}

bool FleeFromEnemy::IsDone()
{
	return true;
}

bool FleeFromEnemy::RequiresInRange() const
{
	return true;
}

ShootAtEnemy::ShootAtEnemy()
{
	m_Preconditions["HasWeapon"] = true;
	m_Preconditions["EnemiesInFov"] = true;

	m_Effects["Survive"] = true;

	m_Cost = 2.f;
}

ShootAtEnemy::ShootAtEnemy(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
		:Action(preConditions, effects, cost)
{
}

bool ShootAtEnemy::CheckProceduralPreconditions(Blackboard* pBlackboard) const
{
	return true;
}

bool ShootAtEnemy::ExecuteAction(Blackboard* pBlackboard)
{
	return true;
}

bool ShootAtEnemy::IsDone()
{
	return true;
}

bool ShootAtEnemy::RequiresInRange() const
{
	return false;
}

SearchForItems::SearchForItems()
{
	m_Preconditions["EnemiesInFov"] = false;

	m_Effects["Survive"] = true;

	m_Cost = 10.f;
}

SearchForItems::SearchForItems(const std::map<std::string, bool>& preConditions,
	const std::map<std::string, bool>& effects, float cost)
	:Action(preConditions, effects, cost)
{
}

bool SearchForItems::CheckProceduralPreconditions(Blackboard* pBlackboard) const
{
	return true;
}

bool SearchForItems::ExecuteAction(Blackboard* pBlackboard)
{
	Character* pCharacter;
	pBlackboard->GetData("Character", pCharacter);
	
	auto characterInfo = pCharacter->GetAgentInfo();
	
	m_WanderAngle += Elite::randomFloat(-Elite::ToRadians(45.f), Elite::ToRadians(45.f));
	Elite::Vector2 target =characterInfo.Position + characterInfo.LinearVelocity.GetNormalized() * 6.f + (Elite::Vector2(cosf(m_WanderAngle) * 4.f, sinf(m_WanderAngle) * 4.f));

	Elite::Vector2 targetDir{ target - characterInfo.Position };

	characterInfo.LinearVelocity = targetDir;
	characterInfo.LinearVelocity.Normalize();
	characterInfo.LinearVelocity *= characterInfo.MaxLinearSpeed;

	pCharacter->SetAgentInfo(characterInfo);
	return true;
}

bool SearchForItems::IsDone()
{
	return true;
}

bool SearchForItems::RequiresInRange() const
{
	return false;
}

EnterHouse::EnterHouse()
{
	m_Preconditions["HouseInFov"] = true;

	m_Effects["Survive"] = true;
	m_Effects["HasItems"] = true;

	m_Cost = 1.f;
}

EnterHouse::EnterHouse(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
		: Action(preConditions, effects, cost)
{
}

bool EnterHouse::CheckProceduralPreconditions(Blackboard* pBlackboard) const
{
	return true;
}

bool EnterHouse::ExecuteAction(Blackboard* pBlackboard)
{
	Character* pCharacter;
	pBlackboard->GetData("Character", pCharacter);
	HouseInfo houseInfo;
	pBlackboard->GetData("ClosestHouse", houseInfo);

	auto characterInfo = pCharacter->GetAgentInfo();

	characterInfo.LinearVelocity = houseInfo.Center - characterInfo.Position;
	characterInfo.LinearVelocity.Normalize();
	characterInfo.LinearVelocity *= -characterInfo.MaxLinearSpeed;

	pCharacter->SetAgentInfo(characterInfo);
	return true;
}

bool EnterHouse::IsDone()
{
	return true;
}

bool EnterHouse::RequiresInRange() const
{
	return true;
}
