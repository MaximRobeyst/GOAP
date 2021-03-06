#include "stdafx.h"
#include "NeedsFood.h"
#include "Character.h"
#include "Helpers.h"

#include <IExamInterface.h>

NeedsFood::NeedsFood()
{
	m_Preconditions["InventoryFull"] = true;
	//m_Preconditions["EnergyLow"] = true;
	m_Preconditions["HasFood"] = false;
	m_Preconditions["HasItemTarget"] = true;

	m_Effects["Survive"] = true;

	m_Cost = 2.5f;
}

NeedsFood::NeedsFood(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
	:Action(preConditions, effects, cost)
{
}

bool NeedsFood::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool NeedsFood::ExecuteAction(float dt, Character* pCharacter)
{
	auto itemMemory = pCharacter->GetItemMemory();
	const auto entityInFov = pCharacter->GetEntitiesInFOV();

	const auto info = pCharacter->GetCurrentItemTarget();
	ItemInfo itemInfo;

	auto steering = pCharacter->GetSteeringOutput();
	steering.AutoOrient = true;

	const auto iter = std::find_if(entityInFov.begin(), entityInFov.end(), SameLocation{ info });
	if (iter == entityInFov.end())
		return false;

	pCharacter->GetInterface()->Item_GetInfo(*iter, itemInfo);

	if(pCharacter->HasAmountOfType(eItemType::PISTOL) > pCharacter->HasAmountOfType(eItemType::MEDKIT))
		pCharacter->RemoveItemFromInventory(pCharacter->GetIndexForType(eItemType::PISTOL));
	else
		pCharacter->RemoveItemFromInventory(pCharacter->GetIndexForType(eItemType::MEDKIT));

	
	if (IsInRange(pCharacter) && iter != entityInFov.end() && itemInfo.Type == eItemType::FOOD)
	{
		if (Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, pCharacter->GetCurrentItemTarget()) < (m_TooCloseRange * m_TooCloseRange))
		{
			steering.LinearVelocity = { iter->Location - pCharacter->GetAgentInfo().Position };
			steering.LinearVelocity.Normalize();
			steering.LinearVelocity *= pCharacter->GetAgentInfo().MaxLinearSpeed;
		}
		else
		{	
			if (pCharacter->GetInterface()->Item_Grab(*iter, itemInfo))
			{
				m_FoodAdded = pCharacter->GetInterface()->Inventory_AddItem(pCharacter->GetSlot(), itemInfo);
				if (m_FoodAdded)
				{
					pCharacter->PopItemFromMemory();
					pCharacter->ChangeCharacterState("HasFood", true);
				}

				pCharacter->SetSlot(pCharacter->GetSlot() + 1);
			}
		}
	}
	pCharacter->SetSteeringOutput(steering);

	return true;

}

bool NeedsFood::IsDone(Character* pCharacter)
{
	return m_FoodAdded;
}

bool NeedsFood::RequiresInRange() const
{
	return true;
}

bool NeedsFood::IsInRange(Character* pCharacter) const
{
	const float distanceSquared = Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, pCharacter->GetCurrentItemTarget());

	return distanceSquared < (pCharacter->GetAgentInfo().GrabRange* pCharacter->GetAgentInfo().GrabRange);
}

std::string NeedsFood::GetName() const
{
	return "Pick up food";
}

Elite::Vector2 NeedsFood::GetTarget(Character* pCharacter)
{
	return pCharacter->GetCurrentItemTarget();
}
