#include "stdafx.h"
#include "SearchForItems.h"
#include "Character.h"

SearchForItems::SearchForItems()
{
	m_Preconditions["EnemiesInFov"] = false;
	m_Preconditions["HouseInFov"] = false;

	m_Effects["Survive"] = true;

	m_Cost = 10.f;
}

SearchForItems::SearchForItems(const std::map<std::string, bool>& preConditions,
	const std::map<std::string, bool>& effects, float cost)
	:Action(preConditions, effects, cost)
{
}

bool SearchForItems::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool SearchForItems::ExecuteAction(Character* pCharacter)
{
	auto characterInfo = pCharacter->GetAgentInfo();

	m_WanderAngle += Elite::randomFloat(-Elite::ToRadians(45.f), Elite::ToRadians(45.f));
	Elite::Vector2 target = characterInfo.Position + characterInfo.LinearVelocity.GetNormalized() * 6.f + (Elite::Vector2(cosf(m_WanderAngle) * 4.f, sinf(m_WanderAngle) * 4.f));

	Elite::Vector2 targetDir{ target - characterInfo.Position };

	characterInfo.LinearVelocity = targetDir;
	characterInfo.LinearVelocity.Normalize();
	characterInfo.LinearVelocity *= characterInfo.MaxLinearSpeed;

	pCharacter->SetAgentInfo(characterInfo);
	return true;
}

bool SearchForItems::IsDone(Character* pCharacter)
{
	return true;
}

bool SearchForItems::RequiresInRange() const
{
	return false;
}

bool SearchForItems::IsInRange(Character* pBlackboard) const
{
	return true;
}
