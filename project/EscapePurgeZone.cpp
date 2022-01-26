#include "stdafx.h"
#include "EscapePurgeZone.h"
#include "Character.h"
#include "Helpers.h"

#include <IExamInterface.h>

EscapePurgeZone::EscapePurgeZone()
{
	m_Preconditions["InPurgeZone"] = true;
	m_Preconditions["OutsideBoundries"] = false;

	m_Effects["InPurgeZone"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 5.f;
}

EscapePurgeZone::EscapePurgeZone(const std::map<std::string, bool>& preConditions,
	const std::map<std::string, bool>& effects, float cost)
	:Action(preConditions, effects, cost)
{
}

bool EscapePurgeZone::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool EscapePurgeZone::ExecuteAction(float dt, Character* pCharacter)
{
	return true;
}

bool EscapePurgeZone::IsDone(Character* pCharacter)
{
	return IsInRange(pCharacter);
}

bool EscapePurgeZone::RequiresInRange() const
{
	return true;
}

bool EscapePurgeZone::IsInRange(Character* pCharacter) const
{
	auto entitiesInFov = pCharacter->GetEntitiesInFOV();

	auto purgeZone = std::find_if(entitiesInFov.begin(), entitiesInFov.end(), IsType<EntityInfo, eEntityType>(eEntityType::PURGEZONE));
	if (purgeZone == entitiesInFov.end())
		return true;
	
	PurgeZoneInfo purgeZoneInfo{};
	pCharacter->GetInterface()->PurgeZone_GetInfo(*purgeZone, purgeZoneInfo);
	
	return Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, purgeZone->Location) > (purgeZoneInfo.Radius * purgeZoneInfo.Radius);
}

std::string EscapePurgeZone::GetName() const
{
	return "Escape purge zone";
}

Elite::Vector2 EscapePurgeZone::GetTarget(Character* pCharacter)
{
	auto entitiesInFov = pCharacter->GetEntitiesInFOV();

	auto purgeZone = std::find_if(entitiesInFov.begin(), entitiesInFov.end(), IsType<EntityInfo, eEntityType>(eEntityType::PURGEZONE)); if (purgeZone == entitiesInFov.end())
		return m_CurrentTarget;

	PurgeZoneInfo purgeZoneInfo{};
	pCharacter->GetInterface()->PurgeZone_GetInfo(*purgeZone, purgeZoneInfo);
	if (m_CurrentTarget == Elite::Vector2{} || Elite::DistanceSquared(purgeZoneInfo.Center, m_CurrentTarget) < (purgeZoneInfo.Radius * purgeZoneInfo.Radius))
		m_CurrentTarget = RandomPointInsideCirlce(pCharacter,purgeZoneInfo);

	pCharacter->GetInterface()->Draw_Point(m_CurrentTarget, 1.f, Elite::Vector3{ 1,1,1 }, 0.9f);
	
	return m_CurrentTarget;
}

Elite::Vector2 EscapePurgeZone::RandomPointInsideCirlce(Character* pCharacter, const PurgeZoneInfo& purgezoneInfo) const
{
	Elite::Vector2 target;
	do
	{
		const float angle = Elite::randomFloat(0, static_cast<float>(2.f * M_PI));
		const float radius = Elite::randomFloat(-purgezoneInfo.Radius, purgezoneInfo.Radius);

		target = Elite::Vector2{
			pCharacter->GetAgentInfo().Position.x + (cosf(angle) * radius),
			pCharacter->GetAgentInfo().Position.y + (sinf(angle) * radius)
		};

	}
	while (Elite::DistanceSquared(target, purgezoneInfo.Center) < (purgezoneInfo.Radius * purgezoneInfo.Radius));

	return target;
	
}
