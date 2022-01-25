#include "stdafx.h"
#include "EscapePurgeZone.h"
#include "Character.h"
#include "Helpers.h"

#include <IExamInterface.h>

EscapePurgeZone::EscapePurgeZone()
{
	m_Preconditions["InPurgeZone"] = true;

	m_Effects["InPurgeZone"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 5.f;
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
	return false;
}

bool EscapePurgeZone::RequiresInRange() const
{
	return true;
}

bool EscapePurgeZone::IsInRange(Character* pCharacter) const
{
	auto entitiesInFov = pCharacter->GetEntitiesInFOV();

	auto purgeZone = std::find_if(entitiesInFov.begin(), entitiesInFov.end(), IsType<EntityInfo, eEntityType>(eEntityType::PURGEZONE));
	PurgeZoneInfo purgeZoneInfo{};
	pCharacter->GetInterface()->PurgeZone_GetInfo(*purgeZone, purgeZoneInfo);
	
	return Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, purgeZone->Location) < (purgeZoneInfo.Radius * purgeZoneInfo.Radius);
}

std::string EscapePurgeZone::GetName() const
{
	return "Escape purge zone";
}
