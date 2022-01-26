#include "stdafx.h"
#include "UseFood.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

UseFood::UseFood()
{
	m_Preconditions["HasFood"] = true;
	m_Preconditions["EnergyLow"] = true;

	m_Effects["EnergyLow"] = false;
	m_Effects["HasFood"] = false;
	m_Effects["Survive"] = true;

	m_Cost = 1.5f;
}

UseFood::UseFood(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
	:Action(preConditions, effects, cost)
{
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
