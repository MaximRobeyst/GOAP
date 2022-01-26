#include "stdafx.h"
#include "NeedsMedkit.h"
#include "Character.h"

#include <Exam_HelperStructs.h>
#include <IExamInterface.h>

#include "Helpers.h"

NeedsMedkit::NeedsMedkit()
{
	m_Preconditions["InventoryFull"] = true;
	//m_Preconditions["EnergyLow"] = true;
	m_Preconditions["HasMedkit"] = false;
	m_Preconditions["HasItemTarget"] = true;

	m_Effects["Survive"] = true;

	m_Cost = 2.5f;
}

bool NeedsMedkit::CheckProceduralPreconditions(Character* pCharacter) const
{
	return true;
}

bool NeedsMedkit::ExecuteAction(float dt, Character* pCharacter)
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

	if (pCharacter->HasAmountOfType(eItemType::FOOD) > pCharacter->HasAmountOfType(eItemType::PISTOL))
		pCharacter->RemoveItemFromInventory(pCharacter->GetIndexForType(eItemType::FOOD));
	else
		pCharacter->RemoveItemFromInventory(pCharacter->GetIndexForType(eItemType::PISTOL));


	if (IsInRange(pCharacter) && iter != entityInFov.end() && itemInfo.Type == eItemType::MEDKIT)
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
				m_MedkitAdded = pCharacter->GetInterface()->Inventory_AddItem(pCharacter->GetSlot(), itemInfo);
				if (m_MedkitAdded)
				{
					pCharacter->PopItemFromMemory();
					pCharacter->ChangeCharacterState("HasMedkit", true);
				}

				pCharacter->SetSlot(pCharacter->GetSlot() + 1);
			}
		}
	}
	pCharacter->SetSteeringOutput(steering);

	return true;
}

bool NeedsMedkit::IsDone(Character* pCharacter)
{
	return m_MedkitAdded;
}

bool NeedsMedkit::RequiresInRange() const
{
	return true;
}

bool NeedsMedkit::IsInRange(Character* pCharacter) const
{
	const float distanceSquared = Elite::DistanceSquared(pCharacter->GetAgentInfo().Position, pCharacter->GetCurrentItemTarget());

	return distanceSquared < (pCharacter->GetAgentInfo().GrabRange* pCharacter->GetAgentInfo().GrabRange);
}

std::string NeedsMedkit::GetName() const
{
	return "Get Medkit";
}

Elite::Vector2 NeedsMedkit::GetTarget(Character* pCharacter)
{
	return pCharacter->GetCurrentItemTarget();
}
