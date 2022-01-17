#include "stdafx.h"
#include "ShootAtEnemy.h"

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

bool ShootAtEnemy::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool ShootAtEnemy::ExecuteAction(Character* pCharacter)
{
	return true;
}

bool ShootAtEnemy::IsDone(Character* pCharacter)
{
	return true;
}

bool ShootAtEnemy::RequiresInRange() const
{
	return false;
}

bool ShootAtEnemy::IsInRange(Character* pCharacter) const
{
	return true;
}
