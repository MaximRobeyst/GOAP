#include "stdafx.h"
#include "NeedsGun.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

#include "Helpers.h"

NeedsGun::NeedsGun()
{
	m_Preconditions["InventoryFull"] = true;
	m_Preconditions["HasWeapon"] = false;
	m_Preconditions["HasItemTarget"] = true;

	m_Effects["Survive"] = true;

	m_Cost = 2.5f;
}

NeedsGun::NeedsGun(const std::map<std::string, bool>& preConditions, const std::map<std::string, bool>& effects,
	float cost)
	:Action(preConditions, effects, cost)
{
}

bool NeedsGun::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool NeedsGun::ExecuteAction(float dt, Character* pCharacter)
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

	if (pCharacter->HasAmountOfType(eItemType::FOOD) > pCharacter->HasAmountOfType(eItemType::MEDKIT))
		pCharacter->RemoveItemFromInventory(pCharacter->GetIndexForType(eItemType::FOOD));
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
				m_GunAdded = pCharacter->GetInterface()->Inventory_AddItem(pCharacter->GetSlot(), itemInfo);
				if (m_GunAdded)
				{
					pCharacter->PopItemFromMemory();
					pCharacter->ChangeCharacterState("HasWeapon", true);
				}

				pCharacter->SetSlot(pCharacter->GetSlot() + 1);
			}
		}
	}
	pCharacter->SetSteeringOutput(steering);

	return true;
}

bool NeedsGun::IsDone(Character* pCharacter)
{
	return m_GunAdded;
}

bool NeedsGun::RequiresInRange() const
{
	return true;
}

bool NeedsGun::IsInRange(Character* pCharacter) const
{
	const float distanceSquared = Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, pCharacter->GetCurrentItemTarget());

	return distanceSquared < (pCharacter->GetAgentInfo().GrabRange* pCharacter->GetAgentInfo().GrabRange);
}

std::string NeedsGun::GetName() const
{
	return "Pick up gun";
}

Elite::Vector2 NeedsGun::GetTarget(Character* pCharacter)
{
	return pCharacter->GetCurrentItemTarget();
}

