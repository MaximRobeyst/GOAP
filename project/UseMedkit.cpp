#include "stdafx.h"
#include "UseMedkit.h"
#include "Character.h"

#include <IExamInterface.h>

UseMedkit::UseMedkit()
{
	m_Preconditions["HasMedkit"] = true;
	m_Preconditions["HealthLow"] = true;

	m_Effects["HasMedkit"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 1.5f;
}

bool UseMedkit::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool UseMedkit::ExecuteAction(float dt, Character* pCharacter)
{
	const auto index = pCharacter->GetIndexForType(eItemType::MEDKIT);
	ItemInfo itemInfo{};

	if (index >= pCharacter->GetInterface()->Inventory_GetCapacity())
		return false;
	
	pCharacter->GetInterface()->Inventory_GetItem(index, itemInfo);

	if (pCharacter->GetInterface()->Medkit_GetHealth(itemInfo) <= 0)
	{
		pCharacter->RemoveItemFromInventory(index);
		pCharacter->ChangeCharacterState("HasMedkit", pCharacter->HasItemOfType(eItemType::MEDKIT));
		return false;
	}
	
	pCharacter->GetInterface()->Inventory_UseItem(index);
	
	return true;
}

bool UseMedkit::IsDone(Character* pCharacter)
{
	return true;
}

bool UseMedkit::RequiresInRange() const
{
	return false;
}

bool UseMedkit::IsInRange(Character* pCharacter) const
{
	return true;
}

std::string UseMedkit::GetName() const
{
	return "Use Medkit";
}
