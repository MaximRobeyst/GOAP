#include "stdafx.h"
#include "UseFood.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

UseFood::UseFood()
{
	m_Preconditions["HasFood"] = true;
	m_Preconditions["EnergyLow"] = true;

	m_Effects["Energylow"] = false;
	m_Effects["HasFood"] = false;
	m_Effects["Survive"] = true;
}

bool UseFood::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool UseFood::ExecuteAction(float dt, Character* pCharacter)
{
	const auto index = pCharacter->GetIndexForType(eItemType::FOOD);
	ItemInfo itemInfo{};

	pCharacter->GetInterface()->Inventory_GetItem(index, itemInfo);
	pCharacter->GetInterface()->Inventory_UseItem(index);

	if (pCharacter->GetInterface()->Food_GetEnergy(itemInfo) <= 0)
	{
		pCharacter->RemoveItemFromInventory(index);
		pCharacter->ChangeCharacterState("HasFood", pCharacter->HasItemOfType(eItemType::FOOD));
		return false;
	}


	
	return true;
}

bool UseFood::IsDone(Character* pCharacter)
{
	return true;
}

bool UseFood::RequiresInRange() const
{
	return true;
}

bool UseFood::IsInRange(Character* pCharacter) const
{
	return true;
}

std::string UseFood::GetName() const
{
	return "Use Food";
}
